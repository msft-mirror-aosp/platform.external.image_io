#ifndef IMAGE_IO_XML_XML_TOKEN_CONTEXT_H_  // NOLINT
#define IMAGE_IO_XML_XML_TOKEN_CONTEXT_H_  // NOLINT

#include <string>

#include "image_io/base/data_context.h"
#include "image_io/base/data_match_result.h"
#include "image_io/base/data_range.h"
#include "image_io/xml/xml_portion.h"

namespace photos_editing_formats {
namespace image_io {

class XmlActionContext;

/// A token context is passed from the action of an XmlTerminal to an XmlHandler
/// associated with the XmlActionContext used to call the action function.
class XmlTokenContext : public DataContext {
 public:
  explicit XmlTokenContext(const XmlActionContext& context);
  XmlTokenContext(size_t location, const DataRange& range,
                  const DataSegment& segment, const DataLineMap& data_line_map,
                  const DataMatchResult& result, const DataRange& token_range,
                  const XmlPortion& token_portion);

  /// @return The result associated with the context.
  const DataMatchResult& GetResult() const { return result_; }

  /// @return The token range for the token. Note that the token range may not
  /// be a subrange of the context's GetRange() or even the context's segment's
  /// data range. Such would be the case when a token's value is split across
  /// two or more data segments.
  const DataRange& GetTokenRange() const { return token_range_; }

  /// @return The portion of the token that this context represents. This
  /// portion value can be the bitwise or of any of the XmlPortion bit values.
  const XmlPortion& GetTokenPortion() const { return token_portion_; }

  /// Builds the string value of the token. If the context's token portion has
  /// the XmlPortion::kBegin bit set, the string value is first cleared. Then
  /// the string is extracted from the context's data source and appended onto
  /// the value. Remember that some token values (especially attribute values)
  /// can be quite long so care should be excercised when obtaining values with
  /// this function.
  /// @param value The value of the token being built.
  /// @return Whether the token value is complete (i.e., the context's portion
  /// had the XmlPortion::kEnd bit set).
  bool BuildTokenValue(std::string* value) const;

  static XmlPortion ComputeTokenPortion(size_t token_scan_count,
                                        DataMatchResult::Type result_type);

 private:
  DataMatchResult result_;
  DataRange token_range_;
  XmlPortion token_portion_;
};

}  // namespace image_io
}  // namespace photos_editing_formats

#endif // IMAGE_IO_XML_XML_TOKEN_CONTEXT_H_  // NOLINT
