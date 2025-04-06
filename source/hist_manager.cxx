////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: hist_manager.cxx                                                         ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "hist_manager.hxx"

// Include the headers of STL.
#include <fstream>

// Include the headers of custom modules.
#include "config.hxx"
#include "dtypes.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistManager: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

HistManager::HistManager()
{   // {{{

    this->path = Path(replace(config.path_history, "~", getenv("HOME")));

    // Create directory if not exists.
    if (not std::filesystem::exists(this->path.parent_path()))
        std::filesystem::create_directories(this->path.parent_path());

    // Load file contents if exists.
    if (std::filesystem::exists(this->path))
        for (const String& line : read_lines(this->path.string()))
            this->hists.emplace_back(line);

}   // }}}

HistManager::~HistManager()
{   // {{{

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistManager: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void HistManager::append(const StringX& storage) noexcept
{   // {{{

    this->hists.push_back(storage);

    std::ofstream ofs(this->path.c_str(), std::ios::app);
    ofs << storage << '\n';

}   // }}}

const Deque<StringX>& HistManager::get_hists() const noexcept
{   // {{{

    return this->hists;

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
