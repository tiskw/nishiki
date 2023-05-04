////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: edit_helper.cxx
//
// This file defines the class `EditHelper` which will be used for computing command/path
// completion candidate, and complete it to users for more confirtable command editing.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef EDIT_HELPER_HXX
#define EDIT_HELPER_HXX

#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// EditHelper: A class to support command editing
////////////////////////////////////////////////////////////////////////////////////////////////////
class EditHelper
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Data types
        ////////////////////////////////////////////////////////////////////////////////////////////

        enum class CompType { COMMAND, OPTION, PATH, PREVIEW, SHELL, SUBCMD, NONE };

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        EditHelper(void);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Returns condidates of completion.
        std::vector<StringX>
        candidate(const StringX& lhs)
        noexcept;

        // Execute completion.
        // This function should be called after calling `candidate` function.
        StringX
        complete(const StringX& lhs)
        const noexcept;

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Terminal size.
        uint16_t wid, hgt;

        // Completion candidates.
        std::vector<std::pair<StringX, StringX>> cands;

        // Completion lines.
        std::vector<StringX> lines;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Functions to compute candidates for each use case.
        void cands_command (const std::vector<StringX>& tokens, const std::string& option) noexcept;
        void cands_filepath(const std::vector<StringX>& tokens, const std::string& option) noexcept;
        void cands_option  (const std::vector<StringX>& tokens, const std::string& option) noexcept;
        void cands_preview (const std::vector<StringX>& tokens, const std::string& option) noexcept;
        void cands_shell   (const std::vector<StringX>& tokens, const std::string& option) noexcept;
        void cands_subcmd  (const std::vector<StringX>& tokens, const std::string& option) noexcept;

        // Convert candidate to strings that will be shown to users.
        void lines_from_cands(const std::vector<std::pair<StringX, StringX>>& cands) noexcept;
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
