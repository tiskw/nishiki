////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: path_x.hxx                                                                  ///
///                                                                                              ///
/// This file defines the class `PathX` that provides utility functions related to file path.    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PATH_X_HXX
#define PATH_X_HXX

// Include the headers of STL.
#include <filesystem>

// Include the headers of custom modules.
#include "dtypes.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definitions
////////////////////////////////////////////////////////////////////////////////////////////////////

class PathX : public Path
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Converting constructors.
        explicit PathX(const std::filesystem::path& path);
        // [Abstract]
        //   Constructor of PathX.
        //
        // [Args]
        //   path (const std::filesystem::path&): [IN] File path.

        explicit PathX(const char* path);
        // [Abstract]
        //   Constructor of PathX.
        //
        // [Args]
        //   path (const char*): [IN] File path.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        PathX operator / (const String& name) const noexcept;
        // [Abstract]
        //
        // [Args]
        //
        // [Returns]

        bool exists(void) const noexcept;
        // [Abstract]
        //   Returns true if the filepath exists.
        //
        // [Returns]
        //   (bool): True if the filepath exists.

        Vector<String> listdir(uint32_t n_max_items = 128) const noexcept;
        // [Abstract]
        //   Returns a list of names of the entries in the given directory path.
        //   The list is sorted in ascending order.
        //
        // [Args]
        //   n_max_items (uint32_t): [IN] The maximum number of items to be listed.
        //
        // [Returns]
        //   (std::vector<std::string>): List of names of the entries.

        String shorten(void) const noexcept;
        // [Abstract]
        //   Returns shorten expression of the path.
        //
        // [Returns]
        //   (String): Shorten expression of the path.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Tuple<PathX, String> split_to_target_and_query(const Vector<StringX>& tokens) noexcept;
// [Abstract]
//   Split the path to completion target and query.
//
// [Args]
//   tokens (const std::vector<StringX>&): [IN] User input tokens.
//
// [Returns]
//   (PathX)      : Completion target.
//   (std::string): Completion query.
//

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
