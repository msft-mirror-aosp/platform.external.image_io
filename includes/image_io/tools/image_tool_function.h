#ifndef IMAGE_IO_TOOLS_IMAGE_TOOL_FUNCTION_H_  // NOLINT
#define IMAGE_IO_TOOLS_IMAGE_TOOL_FUNCTION_H_  // NOLINT

#include <functional>
#include <string>

namespace photos_editing_formats {
namespace image_io {

/// All output of the ImageTool() function and the underlying image_io functions
/// are sent to this type of function that is passed to ImageTool(). Client code
/// can use a function that writes the line to stdout or to a log file. The
/// str parameter may have embedded new line characters in it. The function
/// should not write its own new line at the end of the str.
using ImageToolOutputter = std::function<void(const std::string& str)>;

/// The ImageTool entry point, easily callable from a main() type function.
/// @param argc The number of strings in the argv array.
/// @param argv The options and values used in the command line.
/// @param outputter A function to output the strings produced by ImageTool().
/// @return A zero value for successful, non-zero for an error.
int ImageTool(int argc, const char* argv[],
              const ImageToolOutputter& outputter);

}  // namespace image_io
}  // namespace photos_editing_formats

#endif // IMAGE_IO_TOOLS_IMAGE_TOOL_FUNCTION_H_  // NOLINT
