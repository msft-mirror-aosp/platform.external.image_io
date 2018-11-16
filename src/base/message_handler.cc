#include "image_io/base/message_handler.h"

#include <memory>
#include <string>
#include <utility>

#include "image_io/base/cout_message_writer.h"

namespace photos_editing_formats {
namespace image_io {

using std::string;
using std::unique_ptr;

/// The message handler. No effort made to delete it at program's end.
static MessageHandler* gMessageHandler = nullptr;

void MessageHandler::Init(std::unique_ptr<MessageWriter> message_writer,
                          std::unique_ptr<MessageStore> message_store) {
  auto* old_handler = gMessageHandler;
  gMessageHandler = new MessageHandler;
  gMessageHandler->SetMessageWriter(std::move(message_writer));
  gMessageHandler->SetMessageStore(std::move(message_store));
  delete old_handler;
}

MessageHandler* MessageHandler::Get() {
  if (!gMessageHandler) {
    gMessageHandler = new MessageHandler;
    gMessageHandler->SetMessageWriter(
        unique_ptr<MessageWriter>(new CoutMessageWriter));
    gMessageHandler->SetMessageStore(
        unique_ptr<MessageStore>(new VectorMessageStore));
  }
  return gMessageHandler;
}

MessageHandler::~MessageHandler() {
  if (gMessageHandler == this) {
    gMessageHandler = nullptr;
  }
}

void MessageHandler::SetMessageWriter(
    std::unique_ptr<MessageWriter> message_writer) {
  message_writer_ = std::move(message_writer);
}

void MessageHandler::SetMessageStore(
    std::unique_ptr<MessageStore> message_store) {
  message_store_ = std::move(message_store);
}

void MessageHandler::ReportMessage(Message::Type type, const string& text) {
  int system_errno = (type == Message::kStdLibError) ? errno : 0;
  ReportMessage(Message(type, system_errno, text));
}

void MessageHandler::ReportMessage(const Message& message) {
  if (message_store_) {
    message_store_->AddMessage(message);
  }
  if (message_writer_) {
    message_writer_->WriteMessage(message);
  }
}

}  // namespace image_io
}  // namespace photos_editing_formats
