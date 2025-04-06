////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: command_runner.hxx                                                          ///
///                                                                                              ///
/// A command runner for NiShiKi to run user input in a backend. The command runner can manage   ///
/// both normal shell command and NiShiKi-special command. See the comments of                   ///
/// `CommandRunner::command_nishiki` function for more details about the NiShiKi-special command.///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CMD_RUNNER_HXX
#define CMD_RUNNER_HXX

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
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        Tuple<StringX, StringX> run(const StringX& command) const noexcept;
        // [Abstract]
        //   Run the given command. New strings for the next editing buffer will be returned. This
        //   function can accept both shell command and NiShiKi-special command (cd, alias, plugin
        //   command, etc.)
        //
        // [Args]
        //   input (const StringX&): [IN] Command string.
        //
        // [Returns]
        //   (Tuple<StringX, StringX>): Next editing buffer (left and right hand side strings).

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        Tuple<StringX, StringX> command_alias(void) const noexcept;
        // [Abstract]
        //   Callback function of "alias" command.
        //   This function prints all aliases values to STDOUT.
        //
        // [Returns]
        //   (Tuple<StringX, StringX>): Next editing buffer (left and right hand side strings).

        Tuple<StringX, StringX> command_cd(const Vector<StringX>& tokens) const noexcept;
        // [Abstract]
        //   Callback function of "cd" command.
        //   This function changes current directory.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Command tokens.
        //
        // [Returns]
        //   (Tuple<StringX, StringX>): Next editing buffer (left and right hand side strings).

        Tuple<StringX, StringX> command_exec(const Vector<StringX>& tokens) const noexcept;
        // [Abstract]
        //   Callback function of the other command.
        //   This function executes the given command using the backend shell.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Command tokens.
        //
        // [Returns]
        //   (Tuple<StringX, StringX>): Next editing buffer (left and right hand side strings).

        Tuple<StringX, StringX> command_plugin(const Vector<StringX>& tokens) const noexcept;
        // [Abstract]
        //   Callback function of the plugin command.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Command tokens.
        //
        // [Returns]
        //   (Tuple<StringX, StringX>): Next editing buffer (left and right hand side strings).

        Tuple<StringX, StringX> command_set(const Vector<StringX>& tokens) const noexcept;
        // [Abstract]
        //   Callback function of "set" command.
        //   This function manages environment variables.
        //
        // [Args]
        //   tokens (const std::vector<StringX>&): [IN] Command tokens.
        //
        // [Returns]
        //   (Tuple<StringX, StringX>): Next editing buffer (left and right hand side strings).
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
