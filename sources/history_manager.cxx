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
#include "command_runner.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

HistoryManager::HistoryManager(void)
{   // {{{

    // Determine the path to the history file.
    this->path = std::filesystem::path(getenv("HOME")) / ".config" / "nishiki" / "history.txt";

    // Check that the history file can be exist.
    this->can_exists = std::filesystem::exists(this->path.parent_path());

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Getter and setter functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
HistoryManager::read_history_file(void) const noexcept
{   // {{{

    // Initialize the returned vector.
    std::vector<StringX> result;

    // Do nothing if the history file cannot exists.
    if (not this->can_exists) return result;

    // Open the history file with read permission.
    std::ifstream ifs(this->path.string());

    // Read the history file and memorize it.
    for (std::string line; getline(ifs, line); line.clear())
        result.emplace_back(line);

    return result;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
HistoryManager::append(const StringX& command) const noexcept
{   // {{{

    // Do nothing if no need to add the given command to the history file.
    if (not HistoryManager::is_history_target(command)) return;

    // Open the history file as an append mode.
    std::ofstream ofs(this->path.string(), std::ios_base::app);

    // Write to the history file.
    ofs << command.strip().string() << std::endl; 

}   // }}}

void
HistoryManager::normalize_and_write(void) const noexcept
{   // {{{

    // Initialize a vector to store histories.
    std::vector<StringX> histories;

    // Open the history file with read permission.
    std::ifstream ifs(this->path.string());

    // Read the history file and memorize it.
    for (std::string line; getline(ifs, line); line.clear())
        histories.push_back(StringX(line).strip());

    // Close the file.
    ifs.close();

    // Reverse the order of the histories and 
    // create a vector to store deduplicated histories.
    std::vector<StringX> histories_deduplicated;
    for (auto iter = histories.rbegin(); iter != histories.rend(); ++iter)
        if (not std::ranges::contains(histories_deduplicated, *iter))
            histories_deduplicated.push_back(*iter);

    // Restore the order of the histories.
    std::reverse(histories_deduplicated.begin(), histories_deduplicated.end());

    // Open the history file again with write permission.
    std::ofstream ofs(this->path.string());

    // Write to the history file.
    for (const StringX& hist : histories_deduplicated)
        ofs << hist << std::endl;

}   // }}}

void
HistoryManager::set_completion_cache(const std::vector<std::pair<StringX, StringX>>& storage) noexcept
{   // {{{

    // Clear the cache.
    this->hist_cache.clear();

    // Update the cache.
    for (const auto& pair : storage)
        this->hist_cache.push_back(pair.first + pair.second);

}   // }}}

StringX
HistoryManager::complete(const StringX& lhs) const noexcept
{   // {{{

    // Returns empty string immediately if no completion query is given.
    if (lhs.size() == 0) return StringX("");

    // Search the matched history from the end.
    for (auto iter = this->hist_cache.rbegin(); iter != this->hist_cache.rend(); ++iter)
        if (iter->startswith(lhs))
            return iter->substr(lhs.size()).strip(false, true);

    return StringX("");

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

bool
HistoryManager::is_history_target(const StringX& command) noexcept
{   // {{{

    if      (command.size() == 0                           ) return false;  // Empty command
    else if (command.startswith(StringX(NISHIKI_CMD_DELIM))) return false;  // NiShiKi command.
    else if (command.startswith(StringX(" "))              ) return false;  // Black-started command.
    else                                                     return true ;  // Others.

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
