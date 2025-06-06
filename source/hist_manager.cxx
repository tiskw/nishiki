////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: hist_manager.cxx                                                         ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "hist_manager.hxx"

// Include the headers of custom modules.
#include "config.hxx"
#include "dtypes.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistManager: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

HistManager::HistManager()
{   // {{{

    this->path = Path(replace(config.path_history, "~", getenv("HOME")));

    // Create directory if not exists.
    if (not std::filesystem::exists(this->path.parent_path()))
        std::filesystem::create_directories(this->path.parent_path());

    // Load file contents if exists.
    if (std::filesystem::exists(this->path))
        for (const String& line : read_lines(this->path.string()))
            this->hists.emplace_back(line.c_str());

}   // }}}

HistManager::~HistManager()
{   // {{{

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistManager: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void HistManager::append(const StringX& storage) noexcept
{   // {{{

    constexpr auto append_text = [](const Path& path, const String& str) noexcept -> void
    // [Abstract]
    //   Append a text to a file.
    //
    // [Args]
    //   path (const Path&)  : [IN] Path to target file.
    //   str  (const String&): [IN] string to write.
    {
        // Open the file with append mode.
        FILE *ofp = fopen(path.c_str(), "at");
        if (ofp == NULL)
            return;

        // Write the given string.
        fputs(str.c_str(), ofp);
        fflush(ofp);

        // Close the file.
        fclose(ofp);

    };

    // Append to the member variable.
    this->hists.push_back(storage);

    // Append to the history file.
    append_text(this->path, storage.string() + '\n');

}   // }}}

const Deque<StringX>& HistManager::get_hists() const noexcept
{   // {{{

    return this->hists;

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
