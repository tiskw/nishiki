////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: hist_comp.hxx                                                               ///
///                                                                                              ///
/// This file defines the class `HistCompleter` which completes user history.                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HIST_COMP_HXX
#define HIST_COMP_HXX

// Include the headers of custom modules.
#include "dtypes.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definitions
////////////////////////////////////////////////////////////////////////////////////////////////////

class HistCompleter
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        void set_hists(const Vector<Pair<StringX, StringX>>& storage) noexcept;
        // [Abstract]
        //   Set the completion cache where the `storage` comes from TextBuffer.
        //
        // [Args]
        //   storage (const std::vector<std::pair<StringX, StringX>>&): Source of histories.

        StringX complete(const StringX& lhs) const noexcept;
        // [Abstract]
        //   Returns completion result.
        //   Note that the `lhs` is not contained in the returned value.
        //
        // [Args]
        //   lhs (const StringX&): Left-hand-side of the editing buffer, i.e. completion query.
        //
        // [Returns]
        //   (StringX): Completion result where `lhs` itself is not contained in the result.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        Vector<StringX> hists;
        // A list of histories.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
