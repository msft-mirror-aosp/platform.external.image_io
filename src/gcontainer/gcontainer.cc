#include "image_io/gcontainer/gcontainer.h"

#include <fstream>

#include "image_io/base/data_segment.h"
#include "image_io/base/data_segment_data_source.h"
#include "image_io/base/message_handler.h"
#include "image_io/base/ostream_data_destination.h"
#include "image_io/jpeg/jpeg_info.h"
#include "image_io/jpeg/jpeg_info_builder.h"
#include "image_io/jpeg/jpeg_scanner.h"
#include "image_io/utils/file_utils.h"

namespace photos_editing_formats {
namespace image_io {
namespace gcontainer {
namespace {

using photos_editing_formats::image_io::DataRange;
using photos_editing_formats::image_io::DataSegment;
using photos_editing_formats::image_io::DataSegmentDataSource;
using photos_editing_formats::image_io::JpegInfoBuilder;
using photos_editing_formats::image_io::JpegScanner;
using photos_editing_formats::image_io::Message;
using photos_editing_formats::image_io::MessageHandler;
using photos_editing_formats::image_io::OStreamDataDestination;
using photos_editing_formats::image_io::ReportErrorPolicy;
using std::string;

// Populates first_image_range with the first image (from the header metadata
// to the EOI marker) present in the JPEG file input_file_name. Returns true if
// such a first image is found, false otherwise.
//
// input_file_name must be a JPEG file.
// image_data_segment is populated with the DataSegment for
// input_file_name, and is populated only in the successful case.
// first_image_range is populated with the first image found in the input file,
// only if such an image is found.
bool ExtractFirstImageInJpeg(const string& input_file_name,
                             std::shared_ptr<DataSegment>* image_data_segment,
                             DataRange* first_image_range) {
  if (first_image_range == nullptr) {
    return false;
  }

  // Get the input and output setup.
  MessageHandler::Get()->ClearMessages();
  auto data_segment =
      ReadEntireFile(input_file_name, ReportErrorPolicy::kReportError);
  if (!data_segment) {
    return false;
  }

  // Get the jpeg info and first image range from the input.
  DataSegmentDataSource data_source(data_segment);
  JpegInfoBuilder jpeg_info_builder;
  jpeg_info_builder.SetImageLimit(1);
  JpegScanner jpeg_scanner;
  jpeg_scanner.Run(&data_source, &jpeg_info_builder);
  if (jpeg_scanner.HasError()) {
    return false;
  }

  const auto& jpeg_info = jpeg_info_builder.GetInfo();
  const auto& image_ranges = jpeg_info.GetImageRanges();
  if (image_ranges.empty()) {
    MessageHandler::Get()->ReportMessage(Message::kPrematureEndOfDataError,
                                         "No Images Found");
    return false;
  }

  *image_data_segment = data_segment;
  *first_image_range = image_ranges[0];
  return true;
}

}  // namespace

bool WriteImageAndFiles(const string& input_file_name,
                        const std::vector<string>& other_files,
                        const string& output_file_name) {
  auto output_stream =
      OpenOutputFile(output_file_name, ReportErrorPolicy::kReportError);
  if (!output_stream) {
    return false;
  }

  OStreamDataDestination output_destination(std::move(output_stream));
  output_destination.SetName(output_file_name);

  DataRange image_range;
  std::shared_ptr<DataSegment> data_segment;
  if (!ExtractFirstImageInJpeg(input_file_name, &data_segment, &image_range)) {
    return false;
  }

  output_destination.StartTransfer();
  DataSegmentDataSource data_source(data_segment);
  data_source.TransferData(image_range, image_range.GetLength(),
                           &output_destination);

  size_t bytes_transferred = image_range.GetLength();
  for (const string& tack_on_file : other_files) {
    if (tack_on_file.empty()) {
      continue;
    }
    auto tack_on_data_segment =
        ReadEntireFile(tack_on_file, ReportErrorPolicy::kReportError);
    if (!tack_on_data_segment) {
      continue;
    }

    DataSegmentDataSource tack_on_source(tack_on_data_segment);
    DataRange tack_on_range = tack_on_data_segment->GetDataRange();
    bytes_transferred += tack_on_range.GetLength();
    tack_on_source.TransferData(tack_on_range, tack_on_range.GetLength(),
                                &output_destination);
  }

  output_destination.FinishTransfer();
  return output_destination.GetBytesTransferred() == bytes_transferred &&
         !output_destination.HasError();
}

bool ParseFileAfterImage(const string& input_file_name,
                         size_t file_start_offset, size_t file_length,
                         string* out_file_contents) {
  if (out_file_contents == nullptr || file_start_offset < 0 ||
      file_length == 0) {
    return false;
  }

  DataRange image_range;
  std::shared_ptr<DataSegment> data_segment;
  if (!ExtractFirstImageInJpeg(input_file_name, &data_segment, &image_range)) {
    return false;
  }

  size_t image_bytes_end_offset = image_range.GetEnd();
  size_t image_file_end = data_segment->GetEnd();
  size_t file_start_in_image = image_bytes_end_offset + file_start_offset;
  size_t file_end_in_image = file_start_in_image + file_length;
  if (image_file_end < file_end_in_image) {
    // Requested file is past the end of the image file.
    return false;
  }

  // Get the file's contents.
  const DataRange file_range(file_start_in_image, file_end_in_image);
  size_t file_range_size = file_range.GetLength();
  // TODO(miraleung): Consider subclassing image_io/data_destination.h and
  // transferring bytes directly into the string. TBD pending additional mime
  // type getters.
  std::ifstream input_file_stream(input_file_name);
  input_file_stream.seekg(file_range.GetBegin());
  out_file_contents->resize(file_range_size);
  input_file_stream.read(&(*out_file_contents)[0], file_range_size);
  return true;
}

}  // namespace gcontainer
}  // namespace image_io
}  // namespace photos_editing_formats
