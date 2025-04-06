////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: hist_cmp.cxx                                                                ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "hist_comp.hxx"

// Include the headers of custom modules.
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// HistCompleter: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void HistCompleter::set_hists(const Vector<Pair<StringX, StringX>>& storage) noexcept
{   // {{{

    // Clear the cache.
    this->hists.clear();

    // Update the cache.
    for (const auto& pair : storage)
        this->hists.push_back(pair.first + pair.second);

}   // }}}

StringX HistCompleter::complete(const StringX& lhs) const noexcept
{   // {{{

    // Returns empty string immediately if no completion query is given.
    if (lhs.size() == 0)
        return StringX("");

    // Search the matched history from the end.
    // If matched string is found, returns the rest of the matched history.
    for (auto iter = this->hists.rbegin(); iter != this->hists.rend(); ++iter)
        if (iter->startswith(lhs))
            return iter->substr(lhs.size()).strip(false, true);

    // Returns empty string if no matched history found.
    return StringX("");

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
