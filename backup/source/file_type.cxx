////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: file_type.cxx                                                               ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "file_type.hxx"

// Include the headers of STL.
#include <filesystem>
#include <fnmatch.h>
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////////////
// FileType: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

FileType::FileType(void)
{   // {{{

    // This class determines mime type from suffix of a file using `/usr/share/mime/globs`
    // in which matching of suffixes and mime types are written. In the constructor of FileType,
    // the file `/usr/share/mime/globs` will be parsed and the matching information is stored
    // to member variables.

    // Open the file.
    std::ifstream ifs("/usr/share/mime/globs");

    // Temporal string for reading the file.
    String line;

    while (std::getline(ifs, line))
    {
        // Skip if commented out.
        if ((line.size() == 0) || (line[0] == '#') || (line.find(':') == String::npos))
            continue;

        // Find the location of colon because each line of the file is a "colon-seperated" value.
        const String::size_type index_colon = line.find_first_of(':');

        // Parse line to mime type and pattern string.
        const String mime_type = line.substr(0, index_colon);
        const String pattern   = line.substr(index_colon + 1, String::npos);

        this->emplace_back(mime_type, pattern);
    }

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// FileType: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

String FileType::mime(const String& path) const noexcept
{   // {{{

    // Do nothing if the path is not a file.
    if (std::filesystem::is_directory(path))
        return "inode/directory";

    // Get file name as a preprocessing for pattern matching.
    const String name = std::filesystem::path(path).filename();

    // Returns if matched item found.
    for (const auto& pair : *this)
    {
        const String& mime_type = pair.first;
        const String& pattern   = pair.second;

        if (fnmatch(pattern.c_str(), name.c_str(), 0) == 0)
            return mime_type;
    }

    // Returns empty string if no matched item found.
    return "text/plain";

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
