////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: path_x.cxx
//
// This file defines the class `PathX` that provides utility functions related to file path.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "path_x.hxx"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>

#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// PathX: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

PathX::PathX(const std::filesystem::path& path) : std::filesystem::path(path)
{ /* Do nothing */ }

PathX::PathX(const char* path) : std::filesystem::path(path)
{ /* Do nothing */ }

////////////////////////////////////////////////////////////////////////////////////////////////////
// PathX: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Returns a list of names of the entries in the given directory path.
// The list is sorted in ascending order.
//
// [Args]
//   void
//
// [Returns]
//   (std::vector<std::string>): List of names of the entries.
//
std::vector<std::string>
PathX::listdir(uint32_t n_max_items)
const noexcept
{
    // Initilize returned vector.
    std::vector<std::string> result;

    // Empty path is will be regarded as a current file.
    PathX target = (strlen(this->c_str()) > 0) ? *this : PathX("./");

    // Replace "~" to home directory.
    std::string target_str = replace(target.string(), "~", getenv("HOME"));
    target = PathX(target_str.c_str());

    // Returns empty vector if the target directory does not exist.
    if (not std::filesystem::is_directory(target))
        return result;

    // Collect all files and directories inside the target directory.
    for (const auto& entry : std::filesystem::directory_iterator(target))
    {
        // Get relative path.
        auto path_relative = entry.path().lexically_relative(target);

        // Add to the returned vector.
        result.push_back(path_relative.string() + (entry.is_directory() ? "/" : ""));

        // Stop directory search if exceeds the maximum number of items.
        // Becase it taks too long time for seaching a big directory and
        // it tend to prevent user's confortable command editing.
        if (result.size() > n_max_items)
            break;
    }

    // Define sorting key function.
    // The sorting rule is the same as `ls --group-directories-first` command.
    constexpr auto sort_key_func = [](const std::string& s1, const std::string& s2) -> bool
    {
        bool is_dir1 = (s1.size() > 0) and (s1.back() == '/');
        bool is_dir2 = (s2.size() > 0) and (s2.back() == '/');

        if      (is_dir1 and is_dir2) return (s1 < s2);
        else if (is_dir1            ) return true;
        else if (            is_dir2) return false;
        else                          return (s1 < s2);
    };

    // Sort list results.
    std::sort(result.begin(), result.end(), sort_key_func);

    return result;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Split the path to completion target and query.
//
// [Args]
//   tokens (const std::vector<StringX>&): [IN] User input tokens.
//
// [Returns]
//   (PathX)      : Completion target.
//   (std::string): Completion query.
//
std::tuple<PathX, std::string>
split_to_target_and_query(const std::vector<StringX>& tokens)
noexcept
{
    // Initialize the target path.
    PathX basepath = PathX("");

    // Update the base path if the token is not empty.
    if (tokens.size() > 0)
        basepath = PathX(tokens.back().strip().string().c_str());

    // Split the target token to parent path and file name.
    return std::make_tuple(PathX(basepath.parent_path()), basepath.filename());
}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
