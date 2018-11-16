#include "image_io/base/istream_data_source.h"

#include "image_io/base/data_destination.h"
#include "image_io/base/data_segment.h"

namespace photos_editing_formats {
namespace image_io {

void IStreamDataSource::Reset() {
  istream_->clear();
  istream_->seekg(0);
  current_data_segment_.reset();
}

std::shared_ptr<DataSegment> IStreamDataSource::GetDataSegment(
    size_t begin, size_t min_size) {
  if (current_data_segment_ && current_data_segment_->Contains(begin)) {
    return current_data_segment_;
  }
  current_data_segment_ = Read(begin, min_size);
  return current_data_segment_;
}

DataSource::TransferDataResult IStreamDataSource::TransferData(
    const DataRange &data_range, size_t best_size,
    DataDestination *data_destination) {
  bool data_transferred = false;
  DataDestination::TransferStatus status = DataDestination::kTransferDone;
  if (data_destination && data_range.IsValid()) {
    size_t min_size = std::min(data_range.GetLength(), best_size);
    if (current_data_segment_ &&
        current_data_segment_->GetLength() >= min_size &&
        current_data_segment_->GetDataRange().Contains(data_range)) {
      status = data_destination->Transfer(data_range, *current_data_segment_);
      data_transferred = true;
    } else {
      istream_->clear();
      size_t chunk_size = min_size;
      for (size_t begin = data_range.GetBegin(); begin < data_range.GetEnd();
           begin += chunk_size) {
        size_t segment_length = 0;
        size_t end = std::min(data_range.GetEnd(), begin + chunk_size);
        std::shared_ptr<DataSegment> data_segment = Read(begin, end - begin);
        if (data_segment) {
          segment_length = data_segment->GetLength();
          if (segment_length) {
            status = data_destination->Transfer(data_segment->GetDataRange(),
                                                *data_segment);
            data_transferred = true;
          }
        }
        if (status != DataDestination::kTransferOk || segment_length == 0) {
          break;
        }
      }
    }
  }
  if (data_transferred) {
    return status == DataDestination::kTransferError ? kTransferDataError
                                                     : kTransferDataSuccess;
  } else {
    return data_destination ? kTransferDataNone : kTransferDataError;
  }
}

std::shared_ptr<DataSegment> IStreamDataSource::Read(size_t begin,
                                                     size_t count) {
  std::shared_ptr<DataSegment> shared_data_segment;
  istream_->seekg(begin);
  if (istream_->rdstate() == std::ios_base::goodbit) {
    Byte *buffer = new Byte[count];
    istream_->read(reinterpret_cast<char *>(buffer), count);
    size_t bytes_read = istream_->gcount();
    shared_data_segment =
        DataSegment::Create(DataRange(begin, begin + bytes_read), buffer);
  }
  return shared_data_segment;
}

}  // namespace image_io
}  // namespace photos_editing_formats
