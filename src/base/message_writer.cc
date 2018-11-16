#include "image_io/base/message_writer.h"

#include <cstring>
#include <sstream>
#include <string>

namespace photos_editing_formats {
namespace image_io {

using std::string;
using std::stringstream;

string MessageWriter::GetFormattedMessage(const Message& message) const {
  stringstream message_stream;
  message_stream << GetTypeCategory(message.GetType()) << ":"
                 << GetTypeDescription(message.GetType(),
                                       message.GetSystemErrno())
                 << ":" << message.GetText();
  return message_stream.str();
}

string MessageWriter::GetTypeCategory(Message::Type type) const {
  if (type == Message::kStatus) {
    return "STATUS";
  } else {
    return "ERROR";
  }
}

string MessageWriter::GetTypeDescription(Message::Type type,
                                         int system_errno) const {
  string description;
  switch (type) {
    case Message::kStatus:
      break;
    case Message::kStdLibError:
      description = system_errno > 0 ? std::strerror(system_errno) : "Unknown";
      break;
    case Message::kPrematureEndOfDataError:
      description = "Premature end of data";
      break;
    case Message::kStringNotFoundError:
      description = "String not found";
      break;
    case Message::kDecodingError:
      description = "Decoding error";
      break;
    case Message::kSyntaxError:
      description = "Syntax error";
      break;
    case Message::kValueError:
      description = "Value error";
      break;
    case Message::kInternalError:
      description = "Internal error";
      break;
  }
  return description;
}

}  // namespace image_io
}  // namespace photos_editing_formats
