////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: directory.cxx                                                               ///
///                                                                                              ///
/// This file defines the class `Directory` which manages directory.                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef DIRECTORY_HXX
#define DIRECTORY_HXX

// Include the headers of STL.
#include <cstdint>
#include <filesystem>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////////////////////////

#define DIRECTORY_NO_ITEM ("NO FILES/DIRECTORIES FOUND")

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class Directory : public std::vector<std::string>
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        Directory(void);
        // [Abstract]
        //   Default constructor of Directory.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Getter and setter functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        const std::filesystem::path&
        get(void) const noexcept;
        // [Abstract]
        //   Get path of the current directory.
        //
        // [Returns]
        //   (const std::filesystem::path&): Current directory path.

        std::filesystem::path
        get_relative(const uint32_t index, const std::filesystem::path& root) const noexcept;
        // [Abstract]
        //   Get relative path of index-th item.
        //
        // [Args]
        //   index (const uint32_t)              : [IN] Index of the target item.
        //   root  (const std::filesystem::path&): [IN] Root of the relative path.
        //
        // [Returns]
        //   (std::string): Relative path.

        void
        set(const std::filesystem::path& path) noexcept;
        // [Abstract]
        //   Set directory path.
        //
        // [Args]
        //   path (const std::filesystem::path&): [IN] Destination path.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        bool
        append(const std::string& name) noexcept;
        // [Abstract]
        //   Append to the path and move to it.
        //
        // [Args]
        //   name (const std::string&): [IN] File/directory name to be appended.
        //
        // [Returns]
        //   (bool): True if the directory is actually changed.

        uint8_t
        color(const uint32_t index) const noexcept;
        // [Abstract]
        //   Get color of the item.
        //
        // [Args]
        //   index (const uint32_t): [IN] Index of the specified item.
        //
        // [Returns]
        //   (uint8_t): ANSI-like color index.

        bool
        update(void) noexcept;
        // [Abstract]
        //   Update contents of directories.
        //
        // [Returns]
        //   (bool): Returns true if directory contents is actually changed.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::filesystem::path path;
        // Directory path.

        std::filesystem::path cache;
        // Path of the current item.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
