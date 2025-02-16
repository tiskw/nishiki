////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: command_runner.hxx                                                          ///
///                                                                                              ///
/// A command runner for NiShiKi to run user input in a backend. The command runner can manage   ///
/// both normal shell command and NiShiKi-special command. See the comments of                   ///
/// `CommandRunner::command_nishiki` function for more details about the NiShiKi-special command.///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_RUNNER_HXX
#define COMMAND_RUNNER_HXX

// Include the headers of STL.
#include <cstdint>

// Include the headers of custom modules.
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////////////////////////

#define NISHIKI_CMD_DELIM ":NISHIKI:"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class CommandRunner
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and descructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        CommandRunner(void);
        // [Abstract]
        //   Default constructor of CommandRunner.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Getter and setter functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        const StringX& get_next_lhs(void) const noexcept;
        const StringX& get_next_rhs(void) const noexcept;
        // [Abstract]
        //   This class contains a pair of string, left/right hand side of editing buffer, that
        //   should be used as a initial buffer of the next user editing area. These getter functions
        //   return these strings.
        //
        // [Returns]
        //   (const StringX&): Left or right hand side of editing buffer that should be used
        //                     to the initial buffer of the next user editing area.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        int32_t
        run(const StringX& input) noexcept;
        // [Abstract]
        //   Run the given command. New strings for the next editing buffer will be set to
        //   `this->lhs_next` and `this->rhs_next`. This function can accept both shell command and
        //   NiShiKi-special command that starts with the string NISHIKI_CMD_DELIM (defined in
        //   command_runner.hxx).
        //
        // [Args]
        //   input (const StringX&): [IN] Input string.
        //
        // [Returns]
        //   (int32_t): EXIT_SUCCESS if succeeded otherwise EXIT_FAILURE.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        StringX lhs_next, rhs_next;
        // Next Left/right hand side of the editing text. This variables will be passed to
        // the EditBuffer instance and used as an initial value of the editing text.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        int32_t
        command_alias(void) const noexcept;
        // [Abstract]
        //   Callback function of "alias" command.
        //   This function prints all aliases values to STDOUT.
        //
        // [Returns]
        //   (int32_t): EXIT_SUCCESS if succeeded otherwise EXIT_FAILURE.

        int32_t
        command_cd(const std::vector<StringX>& tokens) const noexcept;
        // [Abstract]
        //   Callback function of "cd" command.
        //   This function changes current directory.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] List of tokens.
        //
        // [Returns]
        //   (int32_t): EXIT_SUCCESS if succeeded otherwise EXIT_FAILURE.

        int32_t
        command_exec(const std::string& command) const noexcept;
        // [Abstract]
        //   Callback function of the other command.
        //   This function executes the given command using the backend shell.
        //
        // [Args]
        //   command (const std::string&): [IN] Called command.
        //
        // [Returns]
        //   (int32_t): Return value of the called external command.

        int32_t
        command_nishiki(const std::string& command) noexcept;
        // [Abstract]
        //   Callback function of the NiShiKi-special command.
        //   This funtion parse NiShiKi-special command and run it.
        //
        // [Args]
        //   command (const std::string&): [IN] Called command.
        //
        // [Returns]
        //   (int32_t): Return value of the called external command.
        //
        // [Notes]
        //   NiShiKi-special command is a commans to modify editing buffer.
        //   The modified editing buffer will be stored in `this->lhs_next` and `this->rhs_next`,
        //   and these two member variables will be passed to the EditBuffer instance.
        //
        //   There are two types of NiShiKi-special command, internal command and external command.
        //   As for the internal command, 3 types of command is allowed, file chooser, process chooser,
        //   and history chooser. These commands are known as "plugins". See the online document for more
        //   details about these plugins. As for external command, any external command can be specified.
        //   The result of the external command is added or directly used to the current editing buffer.
        //
        //   The format of the NiShiKi-special command is:
        //
        //     ::NISHIKI::{type}::NISHIKI::{lhs}::NISHIKI::{rhs}::NISHIKI::{cmd}
        //
        //   where "::NISHIKI::" is a delimiter of the NiShiKi-special command and defined
        //   as NISHIKI_CMD_DELIM macro in `command_runner.hxx`. The meaning of each tokens are the following:
        //
        //   - type: Command type. Either of "int" (internal command) or "ext" (external command).
        //   - lhs : Left-hand-side of the current editing buffer.
        //   - rhs : Right-hand-side of the current editing buffer.
        //   - cmd : Command contents.

        int32_t
        command_set(const std::vector<StringX>& tokens) const noexcept;
        // [Abstract]
        //   Callback function of "set" command.
        //   This function manages environment variables.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] List of tokens.
        //
        // [Returns]
        //   (int32_t): EXIT_SUCCESS if succeeded otherwise EXIT_FAILURE.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
