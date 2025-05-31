////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: history_manager.cxx                                                         ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "history_manager.hxx"

// Include the headers of STL.
#include <algorithm>
#include <fstream>
#include <set>

// Include the headers of custom modules.
#include "config.hxx"
#include "command_runner.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

HistoryManager::HistoryManager(void)
{   // {{{

    // Determine the path to the history file.
    this->path = std::filesystem::path(getenv("HOME")) / ".config" / "nishiki" / "history.txt";

    // Load the history file is exists.
    if (std::filesystem::exists(this->path))
    {
        // Open the history file with read permission.
        std::ifstream ifs(this->path.string());

        // Read the history file and memorize it.
        for (std::string line; getline(ifs, line); line.clear())
            this->histories.emplace_back(StringX(line).strip());
    }
    // If the history file does not exists but can exists, create it.
    else if (std::filesystem::exists(this->path.parent_path()))
        std::ofstream{this->path.string()};

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Getter and setter functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
HistoryManager::get_histories(void) const noexcept
{ return this->histories; }

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
HistoryManager::append(const StringX& cmd) noexcept
{   // {{{

    // Append to the history vector.
    this->histories.emplace_back(cmd);

    // Do nothing if no need to add the given command to the history file.
    if (not HistoryManager::is_history_target(cmd)) return;

    // Open the history file as an append mode and write the command.
    std::ofstream ofs(this->path.string(), std::ios_base::app);
    ofs << cmd.strip().string() << std::endl; 

}   // }}}

void
HistoryManager::normalize_and_write(void) const noexcept
{   // {{{

    // Initialize a vector to store histories.
    std::vector<StringX> hists;

    // Open the history file with read permission.
    std::ifstream ifs(this->path.string());

    // Read the history file and memorize it.
    for (std::string line; getline(ifs, line); line.clear())
        hists.push_back(StringX(line).strip());

    // Close the file.
    ifs.close();

    // Reverse the order of the histories and create a vector to store deduplicated histories.
    std::vector<StringX> hists_deduplicated;
    for (auto iter = hists.rbegin(); iter != hists.rend(); ++iter)
        if (not std::ranges::contains(hists_deduplicated, *iter))
            hists_deduplicated.push_back(*iter);

    // Restore the order of the histories.
    std::reverse(hists_deduplicated.begin(), hists_deduplicated.end());

    // Drop histories from the front if the size of the history is too large.
    while (hists_deduplicated.size() > config.max_hist_size)
        hists_deduplicated.erase(hists_deduplicated.begin());

    // Open the history file again with write permission.
    std::ofstream ofs(this->path.string());

    // Write to the history file.
    for (const StringX& hist : hists_deduplicated)
        ofs << hist << std::endl;

}   // }}}

StringX
HistoryManager::complete(const StringX& lhs) const noexcept
{   // {{{

    // Returns empty string immediately if no completion query is given.
    if (lhs.size() == 0) return StringX("");

    // Search the matched history from the end.
    for (auto iter = this->histories.rbegin(); iter != this->histories.rend(); ++iter)
        if (iter->startswith(lhs))
            return iter->substr(lhs.size()).strip(false, true);

    return StringX("");

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

bool
HistoryManager::is_history_target(const StringX& cmd) noexcept
{   // {{{

    if      (cmd.size() == 0                           ) return false;  // Empty command
    else if (cmd.startswith(StringX(NISHIKI_CMD_DELIM))) return false;  // NiShiKi command.
    else if (cmd.startswith(StringX(" "))              ) return false;  // Black-started command.
    else                                                 return true ;  // Others.

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
