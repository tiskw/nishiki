////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: read_command.hxx                                                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "read_command.hxx"

// Include the headers of STL.
#include <iostream>

// Include the headers of custom modules.
#include "config.hxx"
#include "command_runner.hxx"
#include "edit_helper.hxx"
#include "term_reader.hxx"
#include "term_writer.hxx"
#include "text_buffer.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// File local macros
////////////////////////////////////////////////////////////////////////////////////////////////////

// Chack the given element is contained in the given map.
#define contains(map, elem) (map.find(elem) != map.end())

////////////////////////////////////////////////////////////////////////////////////////////////////
// ReadCommand: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

ReadCommand::ReadCommand(void)
{   // {{{

    // Append all histories to the editing buffer.
    for (const StringX& line : this->hist.read_history_file())
        this->buffer.create(StringX(""), line);

}   // }}}

ReadCommand::~ReadCommand(void)
{   // {{{

    // Squash the history file.
    this->hist.normalize();

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// ReadCommand: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX
ReadCommand::read(const StringX& lhs_ini, const StringX& rhs_ini)
noexcept
{   // {{{

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // [Abstract]
    //   Returns encoded NiShiKi-internal command.
    //   The returned values are delimiter-separated value of the followings:
    //
    //     1. command type ("int" or "ext")
    //     2. left-hand-side of the editing buffer
    //     3. right-hand-side of the editing buffer
    //     4. command contents
    //
    // [Args]
    //   command (const std::string&): [IN] Input command.
    //
    // [Returns]
    //   (StringX): Encoded NiShiKi-internal command.
    //
    constexpr auto create_nishiki_command = [](const std::string& command, const StringX& lhs, const StringX& rhs)
    {
        // Sepertor of the NiShiKi-internal command.
        const StringX delim = StringX(NISHIKI_CMD_DELIM);

        // Find the index of the first whitespace.
        const std::string::size_type i = command.find(' ');

        // Returns encoded NiShiKi-inernal command.
        if ((i != std::string::npos) and ((command.substr(0, i) == "!int") or (startswith(command.substr(0, i), "!ext"))))
        {
            const StringX cmd_type = StringX(command.substr(1, i-1));
            const StringX cmd_cont = StringX(command.substr(i, std::string::npos)).strip();

            return (delim + cmd_type + delim + lhs + delim + rhs + delim + cmd_cont);
        }

        // Parse error.
        else
            std::cout << "\033[33mNiShiKi: Error while parsing NiShiKi-special command \033[m" << std::endl;

        return StringX("");
    };

    TermReader reader = TermReader();
    TermWriter writer = TermWriter();
    EditHelper helper = EditHelper();

    // Create new buffer.
    this->buffer.create(lhs_ini, rhs_ini);

    // Set editing mode to INSERT mode.
    this->buffer.set_mode(TextBuffer::Mode::INSERT);

    // Update cache for history completions.
    // This function should be called for every time when starting editing because
    // previous editing result should be contained in the history cache.
    this->hist.set_completion_cache(this->buffer.get_storage());

    while (true)
    {
        // Get editing buffer.
        const StringX& lhs = this->buffer.get_lhs();
        const StringX& rhs = this->buffer.get_rhs();

        // Re-draw terminal.
        writer.write(lhs, rhs, this->buffer.get_mode(), helper.candidate(lhs), this->hist.complete(lhs));

        // Get user input.
        CharX cx = reader.getch();

        // Check if the given key is registered in the keybinds.
        // If true, then returns the NiShiKi-internal command.
        if (contains(config.keybind, cx.printable()))
            return create_nishiki_command(config.keybind[cx.printable()], lhs, rhs);

        // Process input character.
        switch (cx.value)
        {
            // Exit function if Ctrl-D is pressed.
            case 0x04:
                return StringX("^D");

            // Execute completion if Ctrl-I (= horizontal tab) is pressed.
            case 0x09:
                this->buffer.set(helper.complete(lhs), rhs);
                break;

            // History completion if Ctrl-N is pressed.
            case 0x0E:
                this->buffer.set(lhs + this->hist.complete(lhs) + CharX(" "), rhs);
                break;

            // Exit function if ENTER is pressed.
            case '\n':
            case '\r':
                this->hist.append(lhs + rhs);
                return (lhs + rhs);

            // Otherwise update editing buffer.
            default: this->buffer.edit(cx);
        }
    }

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
