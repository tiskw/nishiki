////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: file_type.hxx                                                               ///
///                                                                                              ///
/// This file defines the class `FileType` that can judge mime type of the given file.           ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FILE_TYPE_HXX
#define FILE_TYPE_HXX

// Include the headers of STL.
#include <string>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class FileType : public std::vector<std::pair<std::string, std::string>>
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        FileType(void);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::string
        mime(const std::string& path) const noexcept;
        // [Abstract]
        //   Returns mime type of the given file.
        //
        // [Args]
        //   path (const std::string&): [IN] Path to a file.
        //
        // [Returns]
        //   (std::string): Mime type information.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
