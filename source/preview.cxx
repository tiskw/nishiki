////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: preview.cxx                                                                 ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "preview.hxx"

// Include the headers of STL.
#include <filesystem>
#include <regex>

// Include the headers of custom modules.
#include "config.hxx"
#include "file_type.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Vector<StringX> preview(const String& path, uint16_t width, uint16_t height) noexcept
{   // {{{

    // FileType instance is reusable, so defined as a static variable.
    static FileType filetype;

    // Create instance for return value.
    Vector<StringX> result;

    // Do nothing if the target file path does not exist.
    if (not std::filesystem::exists(path))
        return result;

    // Compute mime type of the preview target.
    const String mime_type = filetype.mime(path);

    // The predicate function for the "std::find_if" below.
    const auto predicate = [mime_type](const std::pair<String, String>& pv)
    { return regex_match(mime_type, std::regex(pv.first)); };

    // Find matched preview method.
    const auto pv_iter = std::find_if(config.previews.begin(), config.previews.end(), predicate);

    // Do nothing if no matched preview method found.
    if (pv_iter == config.previews.end())
        return result;

    // Get command to be run.
    const String command = replace(pv_iter->second, "{path}", path);

    // Get command output and split it into lines.
    Vector<String> preview_lines = split(run_command(command), "\n");

    // Copy preview lines to the result variable.
    for (uint32_t idx = 0; (idx < preview_lines.size()) && (idx < height); ++idx)
    {
        // Replace TAB to 4 white spaces.
        preview_lines[idx] = replace(preview_lines[idx], "\t", "    ");

        // Append to result list.
        result.push_back(StringX(preview_lines[idx].c_str()).clip(width));
    }

    // Exit function if one preview procedure finished.
    return result;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
