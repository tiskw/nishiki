////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: command_runner.hxx                                                          ///
///                                                                                              ///
/// A command runner for NiShiKi to run user input in a backend. The command runner can manage   ///
/// both normal shell command and NiShiKi-special command, for example, "cd", "set", and "var".  ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CMD_RUNNER_HXX
#define CMD_RUNNER_HXX

// Include the headers of custom modules.
#include "string_x.hxx"

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
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
