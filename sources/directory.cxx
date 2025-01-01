////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: directory.cxx                                                               ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "directory.hxx"

// Include the headers of custom modules.
#include "path_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Directory: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

Directory::Directory(void)
{ /* Do nothing */ }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Directory: Getter and setter functions
////////////////////////////////////////////////////////////////////////////////////////////////////

const std::filesystem::path&
Directory::get(void) const noexcept
{   // {{{

    return this->path;

}   // }}}

std::filesystem::path
Directory::get_relative(const uint32_t index, const std::filesystem::path& root) const noexcept
{   // {{{

    return std::filesystem::relative(this->path / (*this)[index % this->size()], root);

}   // }}}

void
Directory::set(const std::filesystem::path& path) noexcept
{   // {{{

    this->path = std::filesystem::canonical(path);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Directory: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Directory::append(const std::string& name) noexcept
{   // {{{

    // Move to the specified directory.
    this->path.append(name);

    // Check that the target path is a directory.
    const bool is_directory = std::filesystem::is_directory(this->path);

    // Move back to the parent directory if the target path is not a directory.
    if (!is_directory)
        this->path = std::filesystem::canonical(this->path.parent_path());

    // Directory is changed if and only if the target path was a directory path.
    return is_directory;

}   // }}}

uint8_t
Directory::color(const uint32_t index) const noexcept
{   // {{{

    // Create copy of myself.
    std::filesystem::path target = this->path;

    // Append file name and create the target path.
    target.append((*this)[index % this->size()]);

    // Check the file property and returns color index.
    //   0: Black, 1: Red,     2: Green, 3: Yellow,
    //   4: Blue,  5: Magenta, 6: Cyan,  7: White,
    if      (std::filesystem::is_block_file(target)    ) return 3;
    else if (std::filesystem::is_character_file(target)) return 3;
    else if (std::filesystem::is_directory(target)     ) return 4;
    else if (std::filesystem::is_fifo(target)          ) return 3;
    else if (std::filesystem::is_socket(target)        ) return 3;
    else if (std::filesystem::is_symlink(target)       ) return 6;
    else  /* std::filesystem::is_regular_file(target) */ return 0;

}   // }}}

bool
Directory::update(void) noexcept
{   // {{{

    // Do nothing if actual directory change is not detected.
    if (this->path == this->cache) return false;

    // Clear items.
    this->clear();

    // Get directory contents.
    for (const std::string& p : PathX(this->path).listdir())
        this->push_back(p);

    // Add marker if the current directory is empty.
    if (this->size() == 0)
        this->push_back(DIRECTORY_NO_ITEM);

    // Remember the current directory to the cache.
    this->cache = this->path;

    return true;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
