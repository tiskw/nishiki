////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: history_manager.hxx                                                         ///
///                                                                                              ///
/// This file defines the class `HistoryManager` which manages history file.                     ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HISTORY_MANAGER_HXX
#define HISTORY_MANAGER_HXX

// Include the headers of STL.
#include <filesystem>

// Include the headers of custom modules.
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definitions
////////////////////////////////////////////////////////////////////////////////////////////////////

class HistoryManager
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        HistoryManager(void);
        // [Abstract]
        //   Default constructor of HistoryManager.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Getter and setter functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<StringX>
        get_histories(void) const noexcept;
        // [Abstract]
        //   Returns a vector of histories.
        //
        // [Returns]
        //   (std::vector<StringX>): A vector of histories.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        void
        append(const StringX& cmd) noexcept;
        // [Abstract]
        //   Append the given command to the history file.
        //
        // [Args]
        //   command (const StringX&): [IN] Command history.

        void
        normalize_and_write(void) const noexcept;
        // [Abstract]
        //   Normalize the history file.

        StringX
        complete(const StringX& lhs) const noexcept;
        // [Abstract]
        //   Returns completion result.
        //   Note that the `lhs` is not contained in the returned value.
        //
        // [Args]
        //   lhs (const StringX&): [IN] Left-hand-side of the editing buffer, i.e. completion query.
        //
        // [Returns]
        //   (StringX): Completion result where `lhs` itself is not contained in the result.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        static bool
        is_history_target(const StringX& cmd) noexcept;
        // [Abstract]
        //   Returns true if the given command should be contained in the history file.
        //
        // [Args]
        //   command (const StringX&): [IN] Target command.
        //
        // [Returns]
        //   (bool): True if the given command should contained in the history file.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::filesystem::path path;
        // Path to the history file.

        std::vector<StringX> histories;
        // History cache for completion.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
