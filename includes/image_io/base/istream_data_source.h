#ifndef IMAGE_IO_BASE_ISTREAM_DATA_SOURCE_H_  // NOLINT
#define IMAGE_IO_BASE_ISTREAM_DATA_SOURCE_H_  // NOLINT

#include <iostream>
#include <memory>
#include <utility>

#include "image_io/base/data_source.h"

namespace photos_editing_formats {
namespace image_io {

/// A DataSource that obtains data from an istream.
class IStreamDataSource : public DataSource {
 public:
  /// Constructs an IStreamDataSource using the given istream.
  /// @param istram_ptr The istream from which to read.
  explicit IStreamDataSource(std::unique_ptr<std::istream> istream_ptr)
      : istream_(std::move(istream_ptr)) {}

  void Reset() override;
  std::shared_ptr<DataSegment> GetDataSegment(size_t begin,
                                              size_t min_size) override;
  TransferDataResult TransferData(const DataRange& data_range, size_t best_size,
                                  DataDestination* data_destination) override;

 private:
  /// The worker function to create a DataSegment and fill it with the given
  /// number of bytes read from the istream, starting at the given location.
  /// @param begin The location in the istream at which to start reading.
  /// @param count The number of bytes to read.
  /// @return A DataSegment pointer, or nullptr if the read failed.
  std::shared_ptr<DataSegment> Read(size_t begin, size_t count);

 private:
  /// The istream from which to read.
  std::unique_ptr<std::istream> istream_;

  /// The current data segment that was read in the GetDataSegment() function.
  std::shared_ptr<DataSegment> current_data_segment_;
};

}  // namespace image_io
}  // namespace photos_editing_formats

#endif // IMAGE_IO_BASE_ISTREAM_DATA_SOURCE_H_  // NOLINT
