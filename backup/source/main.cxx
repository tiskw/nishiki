////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: main.cxx                                                                    ///
///                                                                                              ///
/// Main function of NiShiKi.                                                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Define version.
#define VERSION "2025.04.05"

// Include the headers of STL.
#include <iostream>

// Include the header of the cxxopts library.
#include <cxxopts.hpp>

// Include the headers of custom modules.
#include "config.hxx"
#include "dtypes.hxx"
#include "term_reader.hxx"
#include "term_writer.hxx"
#include "utils.hxx"
#include "history_manager.hxx"
#include "command_runner.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static Map<String, String>
parse_args(int32_t argc, const char* argv[])
// [Abstract]
//   Parse command line arguments.
//
// [Args]
//   argc (int32_t)    : The number of strings pointed to by argv.
//   argv (const char*): The strings of command line arguments.
//
// [Returns]
//   (std::Map<String, String>): Parsed command line arguments.
//
{   // {{{

    // Create parser instance.
    cxxopts::Options options("nishiki", "NiShiKi - a simple shell wrapper\n");

    // Configure parser.
    options.add_options()
        ("filenames", "The filename(s) to process", cxxopts::value<std::vector<std::string>>())
        ("c,config",  "Path to config file",        cxxopts::value<std::string>()->default_value(std::string("auto")))
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
    Map<String, String> args;

    // Option: -c,--config.
    args["config"] = result["config"].as<std::string>();

    // Option: -h,--help and -v,--version.
    if (result.count("help")   ) { std::cout << options.help() << std::endl; exit(EXIT_SUCCESS); }
    if (result.count("version")) { std::cout << VERSION        << std::endl; exit(EXIT_SUCCESS); }

    return args;

}   // }}}

static StringX
readline(const HistoryManager& hist, const CommandRunner& runner, const char* user_inputs) noexcept
// [Abstract]
//   Read user input with rich interface.
//
// [Args]
//   hist       (const HistoryManager&): History manager.
//   runner     (const CommandRunner&) : Command runner.
//   user_input (const char*)          : User input (mainly used for uinttest).
//
// [Returns]
//   (std::map<std::string, std::string>): Parsed command line arguments.
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
    const std::string ps1 = "=>> ";

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

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
main(int32_t argc, const char* argv[], const char* envp)
// [Abstract]
//   Main function of NiSHiKi.
//
// [Args]
//   argc (const int32_t): Number of command line arguments.
//   argv (const char*[]): Contents of command line arguments.
//   envp (const char*[]): Pointer to environment variables.
//
// [Returns]
//   (int32_t): EXIT_SUCCESS or EXIT_FAILURE.
//
{   // {{{

    // Parse command line arguments.
    Map<String, String> args = parse_args(argc, argv);

    //
    HistoryManager hist;
    CommandRunner runner;

    // Show welcome message.
    std::cout << "Welcome to ";
    std::cout << "\033[31mN \033[35mI \033[32mS \033[33mH \033[35mI \033[36mK \033[35mI\033[m !!";
    std::cout << std::endl;

    while (true)
    {
        // Print the prompt0.
        // std::cout << '\n' << py_engine.get_prompt(0) << std::endl;

        // Read user input. Returns value is lhs and rhs.
        // Use command runner's lhs and rhs string as a initial editing string.
        const StringX input = readline(hist, runner, "");

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

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
