////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: history_manager.hxx                                                         ///
///                                                                                              ///
/// This file defines the class `HistoryManager` which manages history file.                     ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HISTORY_MANAGER_HXX
#define HISTORY_MANAGER_HXX

#include <filesystem>

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


        ////////////////////////////////////////////////////////////////////////////////////////////
        // Getter and setter functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<StringX>
        read_history_file(void)
        const noexcept;
        // [Abstract]
        //   Read history file and returns a vector of histories.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (std::vector<StringX>): A vector of histories.


        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        void
        append(const StringX& command)
        const noexcept;
        // [Abstract]
        //   Append the given command to the history file.
        //
        // [Args]
        //   command (const StringX&): Command history.
        //
        // [Returns]
        //   void

        // Normalize the history file.
        void
        normalize(void)
        const noexcept;
        // [Abstract]
        //   Normalize the history file.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   void

        // Set the completion cache where the `storage` comes from TextBuffer.
        void
        set_completion_cache(const std::vector<std::pair<StringX, StringX>>& storage)
        noexcept;
        // [Abstract]
        //   Set the completion cache where the `storage` comes from TextBuffer.
        //
        // [Args]
        //   storage (const std::vector<std::pair<StringX, StringX>>&): Source of histories.
        //
        // [Returns]
        //   void

        StringX
        complete(const StringX& lhs)
        const noexcept;
        // [Abstract]
        //   Returns completion result.
        //   Note that the `lhs` is not contained in the returned value.
        //
        // [Args]
        //   lhs (const StringX&): Left-hand-side of the editing buffer, i.e. completion query.
        //
        // [Returns]
        //   (StringX): Completion result where `lhs` itself is not contained in the result.


        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        static bool
        is_history_target(const StringX& command)
        noexcept;
        // [Abstract]
        //   Returns true if the given command should be contained in the history file.
        //
        // [Args]
        //   command (const StringX&): Target command.
        //
        // [Returns]
        //   (bool): True if the given command should contained in the history file.


    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::filesystem::path path;
        // [Abstract]
        //   Path to the history file.

        bool can_exists;
        // [Abstract]
        //   True if the history file can be exist.

        std::vector<StringX> hist_cache;
        // [Abstract]
        //   History cache for completion.
};


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
