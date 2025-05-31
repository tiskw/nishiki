////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: edit_helper.cxx                                                             ///
///                                                                                              ///
/// This file defines the class `EditHelper` which will be used for computing command/path       ///
/// completion candidate, and complete it to users for more confirtable command editing.         ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef EDIT_HELPER_HXX
#define EDIT_HELPER_HXX

// Include the headers of custom modules.
#include "dtypes.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definitioin
////////////////////////////////////////////////////////////////////////////////////////////////////

class EditHelper
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Data types
        ////////////////////////////////////////////////////////////////////////////////////////////

        enum class CompType { COMMAND, OPTION, PATH, PREVIEW, SHELL, SUBCMD, NONE };

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        explicit EditHelper(const TermSize area);
        // [Abstract]
        //   Default constructor of CharX.
        //
        // [Args]
        //   height (uint16_t): [IN] height of completion area.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        Vector<StringX> candidate(const StringX& lhs) noexcept;
        // [Abstract]
        //   Returns condidates of completion.
        //
        // [Args]
        //   lhs (const StringX&): [IN] Left-hand-side of the user input.
        //
        // [Returns]
        //   (std::vector<StringX>): Array of lines (strings) for showing completion candidates to users.

        StringX complete(const StringX& lhs) const noexcept;
        // [Abstract]
        //   Execute completion.
        //   This function should be called after calling `candidate` function.
        //
        // [Args]
        //   lhs (const StringX&): [IN] Left-hand-side of the user input.
        //
        // [Returns]
        //   (StringX): Completed left-hand-side string.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        TermSize area;
        // Size of drawing area.

        Vector<Pair<StringX, StringX>> cands;
        // Completion candidates.

        Vector<StringX> lines;
        // Completion lines.

        Vector<StringX> cache_commands;
        // Cache of available command names.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Functions to compute candidates for each use case.
        void cands_command(const Vector<StringX>& tokens, const String& option) noexcept;
        // [Abstract]
        //   Compute command completion candidates.
        //   The result candidates will be stored in `this->cands`.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Parsed tokens of the user input.
        //   option (const std::string&)         : [IN] Optional string of the completion.

        void cands_filepath(const Vector<StringX>& tokens) noexcept;
        // [Abstract]
        //   Compute file path completion candidates.
        //   The result candidates will be stored in `this->cands`.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Parsed tokens of the user input.

        void cands_option(const Vector<StringX>& tokens) noexcept;
        // [Abstract]
        //   Compute command option candidates.
        //   The result candidates will be stored in `this->cands`.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Parsed tokens of the user input.

        void cands_preview(const Vector<StringX>& tokens) noexcept;
        // [Abstract]
        //   Compute completion candidates for preview.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Parsed tokens of the user input.
        //   option (const std::string)          : [IN] Optional string for the preview.

        void cands_shell(const Vector<StringX>& tokens, const String& option) noexcept;
        // [Abstract]
        //   Compute completion candidates from shell command.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Parsed tokens of the user input.
        //   option (const std::string&)         : [IN] Optional string (normally it is a shell command).

        void cands_subcmd(const Vector<StringX>& tokens, const String& option) noexcept;
        // [Abstract]
        //   Compute completion candidates from sub command.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Parsed tokens of the user input.
        //   option (const std::string&)         : [IN] Optional string.

        void lines_from_cands(const Vector<Pair<StringX, StringX>>& cands) noexcept;
        // [Abstract]
        //   Convert candidate to strings that will be shown to users.
        //
        // [Args]
        //   cands (const std::vector<std::pair<StringX, StringX>>&): [IN] Completion candidates.
        //
        // [Returns]
        //   (std::vector<StringX>): Array of lines (strings) for showing completion candidates to users.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
