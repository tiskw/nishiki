////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: directory.cxx
//
// This file defines the class `Directory` which manages directory.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "directory.hxx"

#include "path_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Directory: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

Directory::Directory(void)
{ /* Do nothing */ }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Directory: Getter and setter functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Get path of the current directory.
//
// [Args]
//   void
//
// [Returns]
//   (const std::filesystem::path&): Current directory path.
//
const std::filesystem::path&
Directory::get(void)
const noexcept
{
    return this->path;
}

///// FUNCTION /////
//
// Get relative path of index-th item.
//
// [Args]
//   index (const uint32_t)              : [IN] Index of the target item.
//   root  (const std::filesystem::path&): [IN] Root of the relative path.
//
// [Returns]
//   (std::string): Relative path.
//
std::filesystem::path
Directory::get_relative(const uint32_t index, const std::filesystem::path& root)
const noexcept
{
    return std::filesystem::relative(this->path / (*this)[index % this->size()], root);
}

///// FUNCTION /////
//
// Set directory path.
//
// [Args]
//   path (const std::filesystem::path&): [IN] Destination path.
//
// [Returns]
//   void
//
void
Directory::set(const std::filesystem::path& path)
noexcept
{
    this->path = std::filesystem::canonical(path);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Directory: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Append to the path and move to it.
//
// [Args]
//   name (const std::string&): [IN] File/directory name to be appended.
//
// [Returns]
//   (bool): True if the directory is actually changed.
//
bool
Directory::append(const std::string& name)
noexcept
{
    // Move to the specified directory.
    this->path.append(name);

    // Check that the target path is a directory.
    bool is_directory = std::filesystem::is_directory(this->path);

    // Move back to the parent directory if the target path is not a directory.
    if (!is_directory)
        this->path = std::filesystem::canonical(this->path.parent_path());

    // Directory is changed if and only if the target path was a directory path.
    return is_directory;
}

///// FUNCTION /////
//
// Get color of the item.
//
// [Args]
//   index (const uint32_t): [IN] Index of the specified item.
//
// [Returns]
//   (uint8_t): ANSI-like color index.
//
uint8_t
Directory::color(const uint32_t index)
const noexcept
{
    // Create copy of myself.
    std::filesystem::path path = this->path;

    // Append file name and create the target path.
    path.append((*this)[index % this->size()]);

    // Check the file property and returns color index.
    //   0: Black, 1: Red,     2: Green, 3: Yellow,
    //   4: Blue,  5: Magenta, 6: Cyan,  7: White,
    if      (std::filesystem::is_regular_file(path))   return 0;
    else if (std::filesystem::is_block_file(path))     return 3;
    else if (std::filesystem::is_character_file(path)) return 3;
    else if (std::filesystem::is_directory(path))      return 4;
    else if (std::filesystem::is_fifo(path))           return 3;
    else if (std::filesystem::is_socket(path))         return 3;
    else if (std::filesystem::is_symlink(path))        return 6;
    else                                               return 0;
}

///// FUNCTION /////
//
// Update contents of directories.
//
// [Args]
//   void
//
// [Returns]
//   (bool): Returns true if directory contents is actually changed.
//
bool
Directory::update(void)
noexcept
{
    // Do nothing if actual directory change is not detected.
    if (this->path == this->cache) return false;

    // Clear items.
    this->clear();

    // Get directory contents.
    for (const std::string& path : PathX(this->path).listdir())
        this->push_back(path);

    // Add marker if the current directory is empty.
    if (this->size() == 0)
        this->push_back(DIRECTORY_NO_ITEM);

    // Remember the current directory to the cache.
    this->cache = this->path;

    return true;
}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
