////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: read_command.cxx                                                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "read_cmd.hxx"

// Include the headers of STL.
#include <signal.h>

// Include the headers of custom modules.
#include "config.hxx"
#include "edit_helper.hxx"
#include "hist_comp.hxx"
#include "term_reader.hxx"
#include "term_writer.hxx"
#include "text_buffer.hxx"
#include "string_x.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static variables
////////////////////////////////////////////////////////////////////////////////////////////////////

bool is_not_interrupted = true;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static void signal_handler(int32_t signal)
{   // {{{

    if (signal == SIGINT)
        is_not_interrupted = false;

}   // }}}

static StringX generate_keybind_command(CharX cx, const StringX& lhs, const StringX& rhs)
{   // {{{

    String command = config.keybinds[cx.value];

    command = replace(command, "{lhs}",  lhs.string());
    command = replace(command, "{rhs}",  rhs.string());
    command = replace(command, "{hist}", config.path_history);

    return StringX(command.c_str());

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Published functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Tuple<StringX, StringX>
readcmd(const StringX& lhs_ini, const StringX& rhs_ini, const Deque<StringX>& hists,
        uint8_t area_height, const String& ps1i, const String& ps1n, const String& ps2,
        const char* histhint_pre, const char* histhint_post, StringX& input)
{   // {{{

    // Get terminal size.
    TermSize term_size = get_terminal_size();

    // Define drawing area of NiShiKi.
    TermSize area = {static_cast<uint16_t>(area_height), term_size.cols};

    // Instanciate necessary objects.
    TermReader    reader  = TermReader();
    TermWriter    writer  = TermWriter(area);
    EditHelper    helper  = EditHelper(area);
    TextBuffer    buffer  = TextBuffer(lhs_ini, rhs_ini, hists);
    HistCompleter histcmp = HistCompleter();

    // Covert the prompt strings to StringX.
    const StringX ps1i_x = StringX(ps1i.c_str());
    const StringX ps1n_x = StringX(ps1n.c_str());
    const StringX ps2_x  = StringX(ps2.c_str());

    // Set editing mode to INSERT mode.
    buffer.set_mode(TextBuffer::Mode::INSERT);

    // Update cache for history completions.
    // This function should be called for every time when starting editing because
    // previous editing result should be contained in the history cache.
    histcmp.set_hists(buffer.get_storage());

    // Set signal handler for SIGINT.
    signal(SIGINT, signal_handler);
    is_not_interrupted = true;

    while (is_not_interrupted)
    {
        // Get editing buffer.
        const StringX& lhs = buffer.get_lhs();
        const StringX& rhs = buffer.get_rhs();

        // Select ps1 buffer.
        const StringX& ps1_x = (buffer.get_mode() == TextBuffer::Mode::INSERT) ? ps1i_x : ps1n_x;

        // Re-draw terminal.
        writer.write(lhs, rhs, ps1_x, ps2_x, helper.candidate(lhs), histcmp.complete(lhs), histhint_pre, histhint_post);

        // Get user input.
        const CharX cx = (input.size() > 0) ? input.pop(StringX::Pos::BEGIN) : reader.getch(is_not_interrupted);

        // Exit if one of the step key is typed.
        if (config.keybinds.contains(cx.value))
        {
            buffer.set(generate_keybind_command(cx, lhs, rhs), StringX(""));
            goto end_of_the_function;
        }

        // Process input character.
        switch (cx.value)
        {
            case 0x03:
                kill(getpid(), SIGINT);
                break;

            // Exit function if Ctrl-D is pressed.
            case 0x04:
                buffer.set(StringX("^D"), StringX(""));
                goto end_of_the_function;

            // Execute completion if Ctrl-I (= horizontal tab) is pressed.
            case 0x09:
                buffer.set(helper.complete(lhs), rhs);
                break;

            // History completion if Ctrl-N is pressed.
            case 0x0E:
                buffer.set(lhs + histcmp.complete(lhs) + CharX(' '), rhs);
                break;

            // Exit function if ENTER is pressed.
            case '\n':
            case '\r':
                goto end_of_the_function;

            // Otherwise update editing buffer.
            default: buffer.edit(cx);
        }
    }

end_of_the_function:

    // Update the buffer if interrupted by Ctrl-C.
    if (not is_not_interrupted)
        buffer.set(StringX("^C"), StringX(""));

    // Restore signal handler for SIGINT.
    signal(SIGINT, SIG_DFL);

    return {buffer.get_lhs(),  buffer.get_rhs()};

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
