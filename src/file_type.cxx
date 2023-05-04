////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: file_type.cxx
//
// This file defines the class `FileType` that can judge mime type of the given file.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "file_type.hxx"

#include <filesystem>
#include <fstream>

#include <glob.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
// FileType: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

FileType::FileType(void)
{
    // This class determines mime type from suffix of a file using `/usr/share/mime/globs`
    // in which matching of suffixes and mime types are written. In the constructor of FileType,
    // the file `/usr/share/mime/globs` will be parsed and the matching information is stored
    // to member variables.

    // Open the file.
    std::ifstream ifs("/usr/share/mime/globs");

    // Temporal string for reading the file.
    std::string line;

    while (std::getline(ifs, line))
    {
        // Skip if commented out.
        if (line.size() > 0 && (line[0] == '#'))
            continue;

        // Find the location of colon because each line of the file is a "colon-seperated" value.
        std::string::size_type index_colon = line.find_first_of(':');

        // Do nothing if ':' not found in a line (syntax error on the globs file?).
        if (index_colon == std::string::npos)
            continue;

        // Parse line to mime type and pattern string.
        std::string mime    = line.substr(0, index_colon);
        std::string pattern = line.substr(index_colon + 1, std::string::npos);

        this->emplace_back(std::make_pair(mime, pattern));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// FileType: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Returns mime type of the given file.
//
// [Args]
//   path (const std::string&): [IN] Path to a file.
//
// [Returns]
//   (std::string): Mime type information.
//
std::string
FileType::mime(const std::string& path)
const noexcept
{
    // Do nothing if the path is not a file.
    if (std::filesystem::is_directory(path))
        return "inode/directory";

    // Do nothing if the path is not a file.
    if (!std::filesystem::exists(path))
        return "";

    // Get file name as a preprocessing for pattern matching.
    const std::string name = std::filesystem::path(path).filename();

    // Returns if matched item found.
    for (const auto& [mime, pattern] : *this)
        if (glob::fnmatch(name, pattern))
            return mime;

    // Returns empty string if no matched item found.
    return "text/plain";
}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
