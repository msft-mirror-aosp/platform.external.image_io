#include "image_io/base/ostream_data_destination.h"

#include "image_io/base/data_range.h"
#include "image_io/base/data_segment.h"
#include "image_io/base/message_handler.h"

namespace photos_editing_formats {
namespace image_io {

using std::ostream;

void OStreamDataDestination::StartTransfer() {}

DataDestination::TransferStatus OStreamDataDestination::Transfer(
    const DataRange& transfer_range, const DataSegment& data_segment) {
  if (ostream_ && transfer_range.IsValid() && !HasError()) {
    size_t bytes_written = 0;
    size_t bytes_to_write = transfer_range.GetLength();
    const Byte* buffer = data_segment.GetBuffer(transfer_range.GetBegin());
    if (buffer) {
      ostream::pos_type prewrite_pos = ostream_->tellp();
      ostream_->write(reinterpret_cast<const char*>(buffer), bytes_to_write);
      ostream::pos_type postwrite_pos = ostream_->tellp();
      if (postwrite_pos != EOF) {
        bytes_written = ostream_->tellp() - prewrite_pos;
        bytes_transferred_ += bytes_written;
      }
    }
    if (bytes_written != bytes_to_write) {
      MessageHandler::Get()->ReportMessage(Message::kStdLibError, name_);
      has_error_ = true;
      return kTransferError;
    }
  }
  return kTransferOk;
}

void OStreamDataDestination::FinishTransfer() {
  if (ostream_) {
    ostream_->flush();
  }
}

}  // namespace image_io
}  // namespace photos_editing_formats
