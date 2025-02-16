////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: nishiki.cxx                                                                 ///
///                                                                                              ///
/// Main function of NiShiKi.                                                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "nishiki.hxx"

// Include the headers of STL.
#include <iostream>

// Include the headers of custom modules.
#include "config.hxx"
#include "file_chooser.hxx"
#include "term_reader.hxx"
#include "term_writer.hxx"
#include "text_chooser.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX
readline(const HistoryManager& hist, const CommandRunner& runner, const PkPyEngine& py_engine, const char* user_inputs) noexcept
{   // {{{

    TextBuffer buffer;
    TermReader reader = TermReader();
    TermWriter writer = TermWriter(config.area_height - 1);
    EditHelper helper = EditHelper(config.area_height - 2);

    // Create new editing buffers.
    for (const StringX& line : hist.get_histories())
        buffer.create(StringX(""), line);
    buffer.create(runner.get_next_lhs(), runner.get_next_rhs());

    // Set editing mode to INSERT mode.
    buffer.set_mode(TextBuffer::Mode::INSERT);

    // Get the prompt string 1.
    const std::string ps1 = py_engine.get_prompt(1);

    // Get the copy of input string.
    char* uin = const_cast<char*>(user_inputs);

    while (true)
    {
        // Get editing buffer.
        const StringX& lhs = buffer.get_lhs();
        const StringX& rhs = buffer.get_rhs();

        // Re-draw terminal.
        writer.write(ps1, lhs, rhs, helper.candidate(lhs), hist.complete(lhs));

        // Get user input. Use user_inputs if exists.
        const CharX cx = ((uin != nullptr) and (*uin != '\0')) ? CharX(uin) : reader.getch();

        // Move the pointer forward if used.
        if ((uin != nullptr) and (*uin != '\0'))
            uin += cx.size;

        // Check if the given key is registered in the keybinds.
        // If true, then returns the NiShiKi-internal command.
        if (config.keybind.contains(cx.string()))
        {
            // Get command corresponds to the keybind.
            const std::string command = config.keybind[cx.string()];

            // Sepertor of the NiShiKi-internal command.
            const StringX delim = StringX(NISHIKI_CMD_DELIM);

            // Find the index of the first whitespace.
            const std::string::size_type i = command.find(' ');

            // Returns encoded NiShiKi-inernal command.
            if ((i != std::string::npos) and ((command.substr(0, i) == "!int") or (command.substr(0, i).starts_with("!ext"))))
            {
                const StringX cmd_type = StringX(command.substr(1, i-1));
                const StringX cmd_cont = StringX(command.substr(i, std::string::npos)).strip();

                // Create NiShiKi internal command and return it.
                return (delim + cmd_type + delim + lhs + delim + rhs + delim + cmd_cont);
            }

            // Parse error.
            return StringX("echo 'NiShiKi: Error while parsing keybind: " + cx.string() + "'");
        }

        // Process input character.
        switch (cx.value)
        {
            // Exit function if Ctrl-D is pressed.
            case 0x04:
                return StringX("^D");

            // Execute completion if Ctrl-I (= horizontal tab) is pressed.
            case 0x09:
                buffer.set(helper.complete(lhs), rhs);
                break;

            // History completion if Ctrl-N is pressed.
            case 0x0E:
                buffer.set(lhs + hist.complete(lhs) + CharX(" "), rhs);
                break;

            // Exit function if ENTER is pressed.
            case '\n':
            case '\r':
                return (lhs + rhs);

            // Otherwise update editing buffer.
            default: buffer.edit(cx);
        }
    }

}   // }}}

int32_t
nishiki_main(const std::map<std::string, std::string>& args) noexcept
{   // {{{

    // Create PkPyEngine instance which manages PocketPy engine.
    PkPyEngine py_engine;

    // Load config values from the config file.
    py_engine.setup_config(&config);

    // Call plugin and exit if --plugin option is specified.
    if      (args.at("plugin") == "filechooser") choose_files_and_exit(StringX("."));
    else if (args.at("plugin") == "histchooser") choose_hists_and_exit();
    else if (args.at("plugin") == "procchooser") choose_procs_and_exit();
    else if (args.at("plugin") != ""           ) print_message_and_exit("NiShiKi: Error: unknown plugin");

    HistoryManager hist;
    CommandRunner runner;

    // Show welcome message.
    std::cout << "Welcome to ";
    std::cout << "\033[31mN \033[35mI \033[32mS \033[33mH \033[35mI \033[36mK \033[35mI\033[m !!";
    std::cout << std::endl;

    while (true)
    {
        // Print the prompt0.
        std::cout << '\n' << py_engine.get_prompt(0) << std::endl;

        // Read user input. Returns value is lhs and rhs.
        // Use command runner's lhs and rhs string as a initial editing string.
        const StringX input = readline(hist, runner, py_engine);

        // Append the user input to the history.
        hist.append(input);

        // Exit command loop if exit command is specified.
        if (input == StringX("exit") or input == StringX("^D"))
            break;

        // Print command.
        std::cout << config.datetime_pre << get_date() << " " << get_time() << config.datetime_post;
        std::cout << " " << input.colorize() << std::endl;

        // Run command.
        runner.run(input);
    }

    // Update the history file.
    hist.normalize_and_write();

    // Show farewell message.
    std::cout << "See you!" << std::endl;

    return EXIT_SUCCESS;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
