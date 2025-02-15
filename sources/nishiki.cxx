////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: nishiki.cxx                                                                 ///
///                                                                                              ///
/// Main function of NiShiKi.                                                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "nishiki.hxx"

// Include the headers of STL.
#include <iostream>
#include <map>

// Include the header of the cxxopts library.
#include <cxxopts.hpp>

// Include the headers of custom modules.
#include "command_runner.hxx"
#include "config.hxx"
#include "file_chooser.hxx"
#include "history_manager.hxx"
#include "pkpy_engine.hxx"
#include "string_x.hxx"
#include "text_chooser.hxx"
#include "term_reader.hxx"
#include "term_writer.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static std::map<std::string, std::string>
parse_args(const int32_t argc, const char* argv[])
// [Abstract]
//   Parse command line arguments.
//
// [Args]
//   argc (const int32_t): The number of strings pointed to by argv.
//   argv (const char*)  : The strings of command line arguments.
//
// [Returns]
//   (std::map<std::string, std::string>): Parsed command line arguments.
//
{   // {{{

    // Create parser instance.
    cxxopts::Options options("nishiki", "NiShiKi - a simple shell wrapper\n");

    // Configure parser.
    options.add_options()
        ("c,config",  "Path to config file",        cxxopts::value<std::string>()->default_value(std::string("auto")))
        ("p,plugin",  "Launch plugin and exit",     cxxopts::value<std::string>()->default_value(std::string("")))
        ("h,help",    "Show help message and exit", cxxopts::value<bool>())
        ("v,version", "Show version info and exit", cxxopts::value<bool>());

    // Parse and get results.
    cxxopts::ParseResult result;
    try
    {
        result = options.parse(argc, argv);
    }
    catch (const std::exception& err)
    {
        std::cout << "\033[33mNiShiKi: Error: " << __FILE__ << " L." << __LINE__ << ": parse_args(): " << err.what() << "\033[m" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize output variable.
    std::map<std::string, std::string> args;

    // Option: -c,--config and -p,--plugin.
    args["config"] = result["config"].as<std::string>();
    args["plugin"] = result["plugin"].as<std::string>();

    // Option: -h,--help and -v,--version.
    if (result.count("help")   ) { std::cout << options.help() << std::endl; exit(EXIT_SUCCESS); }
    if (result.count("version")) { std::cout << VERSION        << std::endl; exit(EXIT_SUCCESS); }

    return args;

}   // }}}

static StringX
readline(TextBuffer& buffer, HistoryManager& hist, const CommandRunner& runner, const PkPyEngine& py_engine) noexcept
// [Abstract]
//   Read user input with rich interface.
//
// [Args]
//   buffer    (TextBuffer&)    : Text buffer.
//   hist      (HistoryManager&): History manager.
//   luaman    (LuaManager&)    : Lua manager.
//   runner    (CommandRunner&) : Command runner.
//   py_engine (PkPyEngine&)    : PocketPy engine.
//
// [Returns]
//   (std::map<std::string, std::string>): Parsed command line arguments.
//
{   // {{{

    TermReader reader = TermReader();
    TermWriter writer = TermWriter(config.area_height - 1);
    EditHelper helper = EditHelper(config.area_height - 2);

    // Create new buffer.
    buffer.create(runner.get_next_lhs(), runner.get_next_rhs());

    // Set editing mode to INSERT mode.
    buffer.set_mode(TextBuffer::Mode::INSERT);

    // Update cache for history completions.
    // This function should be called for every time when starting editing because
    // previous editing result should be contained in the history cache.
    hist.set_completion_cache(buffer.get_storage());

    // Get the prompt string 1.
    const std::string ps1 = py_engine.get_prompt(1);

    while (true)
    {
        // Get editing buffer.
        const StringX& lhs = buffer.get_lhs();
        const StringX& rhs = buffer.get_rhs();

        // Re-draw terminal.
        writer.write(ps1, lhs, rhs, helper.candidate(lhs), hist.complete(lhs));

        // Get user input.
        const CharX cx = reader.getch();

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
                hist.append(lhs + rhs);
                return (lhs + rhs);

            // Otherwise update editing buffer.
            default: buffer.edit(cx);
        }
    }

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
nishiki_main(const int32_t argc, const char* argv[])
{   // {{{

    // Parse command line arguments.
    auto args = parse_args(argc, argv);

    // Create PkPyEngine instance which manages PocketPy engine.
    PkPyEngine py_engine;

    // Load config values from the config file.
    py_engine.setup_config(&config);

    // Call plugin and exit if --plugin option is specified.
    if      (args["plugin"] == "filechooser") choose_files_and_exit(StringX("."));
    else if (args["plugin"] == "histchooser") choose_hists_and_exit();
    else if (args["plugin"] == "procchooser") choose_procs_and_exit();
    else if (args["plugin"] != ""           ) print_message_and_exit("NiShiKi: Error: unknown plugin");

    // Create Text buffer instance which stores text buffers.
    TextBuffer buffer;

    // Create History manager instance which manages command histories.
    HistoryManager hist;

    // Create CommandRunner instance which runs user input command.
    CommandRunner runner;

    // Show welcome message.
    std::cout << "Welcome to ";
    std::cout << "\033[31mN \033[35mI \033[32mS \033[33mH \033[35mI \033[36mK \033[35mI\033[m !!";
    std::cout << std::endl;

    // Append all histories to the editing buffer.
    for (const StringX& line : hist.read_history_file())
        buffer.create(StringX(""), line);

    while (true)
    {
        // Print the prompt0.
        std::cout << '\n' << py_engine.get_prompt(0) << std::endl;

        // Read user input. Returns value is lhs and rhs.
        // Use command runner's lhs and rhs string as a initial editing string.
        const StringX input = readline(buffer, hist, runner, py_engine);

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
