////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: history_manager.cxx
//
// This file defines the class `HistoryManager` which manages history file.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "history_manager.hxx"

#include <algorithm>
#include <fstream>
#include <set>

#include "command_runner.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// File local macros
////////////////////////////////////////////////////////////////////////////////////////////////////

// Chack the given element is contained in the given set.
#define contains(map, elem) (map.find(elem) != map.end())

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

HistoryManager::HistoryManager(void)
{
    // Determine the path to the history file.
    this->path = std::filesystem::path(getenv("HOME")) / ".config" / "nishiki" / "history.txt";

    // Check that the history file can be exist.
    this->can_exists = std::filesystem::exists(this->path.parent_path());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Getter and setter functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Read history file and returns a vector of histories.
//
// [Args]
//   void
//
// [Returns]
//   (std::vector<StringX>): A vector of histories.
//
std::vector<StringX>
HistoryManager::read_history_file(void)
const noexcept
{
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Append the given command to the history file.
//
// [Args]
//   command (const StringX&): Command history.
//
// [Returns]
//   void
//
void
HistoryManager::append(const StringX& command)
const noexcept
{
    // Do nothing if no need to add the given command to the history file.
    if (not HistoryManager::is_history_target(command)) return;

    // Open the history file as an append mode.
    std::ofstream ofs(this->path.string(), std::ios_base::app);

    // Write to the history file.
    ofs << command.strip().string() << std::endl; 
}

///// FUNCTION /////
//
// Normalize the history file.
//
// [Args]
//   void
//
// [Returns]
//   void
//
void
HistoryManager::normalize(void)
const noexcept
{
    // Initialize a vector to store histories.
    std::vector<StringX> histories;

    // Open the history file with read permission.
    std::ifstream ifs(this->path.string());

    // Read the history file and memorize it.
    for (std::string line; getline(ifs, line); line.clear())
        histories.push_back(StringX(line).strip());

    // Close the file.
    ifs.close();

    // Reverse the order of the histories.
    std::reverse(histories.begin(), histories.end());

    // Initialize a vector to store deduplicated histories.
    std::vector<StringX> histories_deduplicated;

    // Initialize a set to judge deduplication.
    std::set<StringX> hist_cache;

    // Deduplicate the histories.
    for (const StringX& hist : histories)
    {
        if ((hist.size() > 0) and (not contains(hist_cache, hist)))
        {
            histories_deduplicated.push_back(hist);
            hist_cache.insert(hist);
        }
    }

    // Restore the order of the histories.
    std::reverse(histories_deduplicated.begin(), histories_deduplicated.end());

    // Open the history file again with write permission.
    std::ofstream ofs(this->path.string());

    // Write to the history file.
    for (const StringX& hist : histories_deduplicated)
        ofs << hist << std::endl;
}

///// FUNCTION /////
//
// Set the completion cache where the `storage` comes from TextBuffer.
//
// [Args]
//   storage (const std::vector<std::pair<StringX, StringX>>&): Source of histories.
//
// [Returns]
//   void
//
void
HistoryManager::set_completion_cache(const std::vector<std::pair<StringX, StringX>>& storage)
noexcept
{
    // Clear the cache.
    this->hist_cache.clear();

    // Update the cache.
    for (const auto& [lhs, rhs] : storage)
        this->hist_cache.push_back(lhs + rhs);
}

///// FUNCTION /////
//
// Returns completion result.
// Note that the `lhs` is not contained in the returned value.
//
// [Args]
//   lhs (const StringX&): Left-hand-side of the editing buffer, i.e. completion query.
//
// [Returns]
//   (StringX): Completion result where `lhs` itself is not contained in the result.
//
StringX
HistoryManager::complete(const StringX& lhs)
const noexcept
{
    // Returns empty string immediately if no completion query is given.
    if (lhs.size() == 0) return StringX("");

    // Search the matched history from the end.
    for (auto iter = this->hist_cache.rbegin(); iter != this->hist_cache.rend(); ++iter)
        if (iter->startswith(lhs))
            return iter->substr(lhs.size());

    return StringX("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistoryManager: Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Returns true if the given command should be contained in the history file.
//
// [Args]
//   command (const StringX&): Target command.
//
// [Returns]
//   (bool): True if the given command should contained in the history file.
//
bool
HistoryManager::is_history_target(const StringX& command)
noexcept
{
    if      (command.size() == 0                           ) return false;  // Empty command
    else if (command.startswith(StringX(NISHIKI_CMD_DELIM))) return false;  // NiShiKi command.
    else if (command.startswith(StringX(" "))              ) return false;  // Black-started command.
    else                                                     return true ;  // Others.
}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
