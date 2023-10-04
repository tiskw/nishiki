////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: read_command.hxx                                                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "read_command.hxx"

#include <iostream>

#include "config.hxx"
#include "command_runner.hxx"
#include "edit_helper.hxx"
#include "term_reader.hxx"
#include "term_writer.hxx"
#include "text_buffer.hxx"


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

    constexpr auto create_nishiki_command = [](const std::string& command, const StringX& lhs, const StringX& rhs)
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
    {
        // Sepertor of the NiShiKi-internal command.
        const StringX delim = StringX(NISHIKI_CMD_DELIM);

        // Returns encoded NiShiKi-inernal command.
        if ((command.length() > 3) && ((command.substr(0, 4) == "!int") || (command.substr(0, 4) == "!ext")))
            return (delim + StringX(command.substr(1, 3)) + delim + lhs + delim + rhs
                  + delim + StringX(command.substr(4, std::string::npos)).strip());

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
                this->buffer.set_lhs(helper.complete(lhs));
                break;

            // History completion if Ctrl-N is pressed.
            case 0x0E:
                this->buffer.set_lhs(lhs + this->hist.complete(lhs) + CharX(" "));
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


////////////////////////////////////////////////////////////////////////////////////////////////////
// ReadCommand: Private member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
