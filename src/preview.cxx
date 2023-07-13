////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: preview.cxx
//
// This file defines the function `preview` that returns preview result as a vector of string.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "preview.hxx"

#include <filesystem>
#include <regex>

#include "config.hxx"
#include "file_type.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Returns preview contents of the given file.
//
// [Args]
//   path   (const std::string&): [IN] Path to the target file.
//   width  (int16_t)           : [IN] Width of preview window.
//   height (int16_t)           : [IN] Height of preview window.
//
// [Returns]
//   (std::vector<std::string>): Lines of preview contents.
//
std::vector<StringX>
preview(const std::string& path, uint16_t width, uint16_t height)
noexcept
{
    // FileType instance is reusable, so defined as a static variable.
    static FileType filetype;

    // Create instance for return value.
    std::vector<StringX> result;

    // Do nothing if the target file path does not exist.
    if (not std::filesystem::exists(path))
        return result;

    // Compute mime type of the preview target.
    // const std::string mime_type = get_mime_type(path);
    const std::string mime_type = filetype.mime(path);

    // Find matched preview method and run it.
    for (const auto& pv : config.previews)
    {
        if (regex_match(mime_type, std::regex(pv.first)))
        {
            // Get command to be run.
            const std::string command = replace(pv.second, "{path}", path);

            // Get command output and split it into lines.
            std::vector<std::string> preview_lines = split(run_command(command), "\n");

            // Copy preview lines to the result variable.
            for (uint32_t idx = 0; (idx < preview_lines.size()) && (idx < height); ++idx)
            {
                // Replace TAB to 4 white spaces.
                preview_lines[idx] = replace(preview_lines[idx], "\t", "    ");

                // Append to result list.
                result.push_back(StringX(preview_lines[idx]).clip(width));
            }

            // Exit function if one preview procedure finished.
            return result;
        }
    }

    return result;
}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
