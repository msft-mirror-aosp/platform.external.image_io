#ifndef IMAGE_IO_BASE_BYTE_DECODER_H_  // NOLINT
#define IMAGE_IO_BASE_BYTE_DECODER_H_  // NOLINT

#include <cstddef>
#include <string>

#include "image_io/base/message.h"
#include "image_io/base/message_handler.h"
#include "image_io/base/types.h"

namespace photos_editing_formats {
namespace image_io {

/// A base class for decoders that scan bytes for strings and numbers.
class ByteArrayDecoder {
 public:
  /// @param message_handler An optional message handler used to report error
  /// and warning conditions encountered while decoding the bytes.
  explicit ByteArrayDecoder(MessageHandler* message_handler);

  /// @param bytes The bytes to examine. The memory pointed to must be valid
  /// throughout the lifetime of this decoder.
  /// @param count The number of bytes to examine.
  void SetBytesAndCount(const Byte* bytes, size_t count) {
    bytes_ = bytes;
    count_ = count;
  }

  /// @return The message handler.
  MessageHandler* GetMessageHandler() const { return message_handler_; }

  /// @return Whether the decoder is set up for big endian values.
  bool IsBigEndian() const { return is_big_endian_; }

  /// @return The address of the start of the byte array.
  const Byte* GetBytes() const { return bytes_; }

  /// @return The size of the byte array.
  size_t GetCount() const { return count_; }

  /// @param is_big_endian Whether the integer data is big_endian.
  void SetIsBigEndian(bool is_big_endian) { is_big_endian_ = is_big_endian; }

  /// @param index The index of the byte to check.
  /// @param count The number of bytes in the block.
  /// @return Whether the bytes in the block [index:index+count) can be read.
  bool IsValidRange(size_t index, size_t count) const;

  /// @param count The number of bytes to skip.
  /// @param index The index at which to start skipping. The count value is
  /// added to the index if the skip is valid.
  /// @return Whether skipping the bytes is valid.
  bool SkipBytes(size_t count, size_t* index);

  /// @param index The index from which to compare the bytes.
  /// @param byteArray The bytes to use in the comparison.
  /// @param count The number of bytes to conpare.
  /// @return Whether the bytes were compared successfully.
  bool CompareBytes(size_t index, const Byte* byteArray, size_t count);

  /// @param expecting A description of the expected UInt32 value.
  /// @param index The index at which to get the UInt32 value. If a value is
  /// obtained successfully, the index is incremented by 4.
  /// @param value The UInt32 pointer to receive the value.
  /// @return Whether a UInt32 value was successfully obtained.
  bool GetValue(const std::string& expecting, size_t* index, UInt32* value);

  /// @param expecting A description of the expected UInt16 value.
  /// @param index The index at which to get the UInt16 value. If a value is
  /// obtained successfully, the index is incremented by 2.
  /// @param value The UInt16 pointer to receive the value.
  /// @return Whether a UInt16 value was successfully obtained.
  bool GetValue(const std::string& expecting, size_t* index, UInt16* value);

  /// @param expecting A description of the expected UInt8 value.
  /// @param index The index at which to get the UInt8 value. If a value is
  /// obtained successfully, the index is incremented by 1.
  /// @param value The UInt8 pointer to receive the value.
  /// @return Whether a UInt8 value was successfully obtained.
  bool GetValue(const std::string& expecting, size_t* index, UInt8* value);

  /// @param index The index from which to obtain a byte value
  /// @return A string that contains "value=XX" or "the end of the buffer"
  std::string GetByteValueString(size_t index) const;

  /// Reports an error message describing what was expected at the index.
  /// @param expecting A description of what was expected.
  /// @param index The index at which something was expected.
  /// @return A false value.
  bool ReportExpectedError(const std::string& expecting, size_t index);

  /// Reports an error message expected at the index.
  /// @param message A description of the error.
  /// @param index The index at which the error occurred
  /// @return A false value.
  bool ReportError(const std::string& message, size_t index);

  /// Reports an error message not related to parsing at an index.
  /// @param message A description of the error.
  /// @return A false value.
  bool ReportError(const std::string& message) {
    return ReportError(message, GetCount());
  }

  /// @param message The message to report to the message handler.
  void ReportMessage(const Message& message);

 private:
  MessageHandler* message_handler_;
  const Byte* bytes_;
  size_t count_;
  bool is_big_endian_;
};

}  // namespace image_io
}  // namespace photos_editing_formats

#endif  // IMAGE_IO_BASE_BYTE_DECODER_H_  // NOLINT
