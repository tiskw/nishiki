////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: main.cxx                                                                    ///
///                                                                                              ///
/// Entry point of NiShiKi.                                                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <cxxopts.hpp>

#include "command_runner.hxx"
#include "config.hxx"
#include "file_chooser.hxx"
#include "text_chooser.hxx"
#include "read_command.hxx"
#include "string_x.hxx"
#include "utils.hxx"
#include "version.hxx"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Definition of static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static std::map<std::string, std::string>
parse_args(const int32_t argc, const char* argv[]);
// [Abstract]
//   Parse command line arguments.
//
// [Args]
//   argc (const int32_t): [IN]  Number of the command line arguments.
//   argv (const char*[]): [IN]  Contents of the command arguments.
//
// [Returns]
//   (std::map<std::string, std::string>): Parsed command line arguments.

int32_t
main(const int32_t argc, const char* argv[]);
// [Abstract]
//   Entry point of NiShiKi.
//
// [Args]
//   argc (const int32_t): [IN]  Number of the command line arguments.
//   argv (const char*[]): [IN]  Contents of the command arguments.
//
// [Returns]
//   (int32_t): EXIT_SUCCESS if Nishiki successfully finished otherwise EXIT_FAILURE.


////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static std::map<std::string, std::string>
parse_args(const int32_t argc, const char* argv[])
{   // {{{

    // Create parser instance.
    cxxopts::Options options("nishiki", "NiShiKi - a simple shell wrapper\n");

    // Configure parser.
    options.add_options()
        ("c,config",  "Path to config file",        cxxopts::value<std::string>()->default_value(std::string("auto")))
        ("p,plugin",  "Launch plugin and exit",     cxxopts::value<std::string>()->default_value(std::string("none")))
        ("h,help",    "Show help message and exit", cxxopts::value<bool>())
        ("v,version", "Show version info and exit", cxxopts::value<bool>())
    ;

    // Parse and get results.
    cxxopts::ParseResult result;
    try
    {
        result = options.parse(argc, argv);
    }
    catch (const std::exception& err)
    {
        printf("\033[33mNiShiKi: Error: %s L.%d: parse_args(): %s\033[m", __FILE__, __LINE__, err.what());
        exit(EXIT_FAILURE);
    }

    // Initialize output variable.
    std::map<std::string, std::string> args;

    // Option: -c, --config
    args["config"] = result["config"].as<std::string>();

    // Option: -p, --plugin
    args["plugin"] = result["plugin"].as<std::string>();

    // Option: -h, --help
    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(EXIT_SUCCESS);
    }

    // Option: -v, --version
    if (result.count("version"))
    {
      std::cout << VERSION << std::endl;
      exit(EXIT_SUCCESS);
    }

    return args;

}   // }}}

int32_t
main(const int32_t argc, const char* argv[])
{   // {{{

    // Parse command line arguments.
    auto args = parse_args(argc, argv);

    // Call plugin and exit if --plugin option is specified.
    if      (args["plugin"] == "filechooser") choose_files_and_exit(StringX("."));
    else if (args["plugin"] == "histchooser") choose_hists_and_exit();
    else if (args["plugin"] == "procchooser") choose_procs_and_exit();

    // Show welcome message.
    std::cout << "Welcome to ";
    std::cout << "\033[31mN \033[35mI \033[32mS \033[33mH \033[35mI \033[36mK \033[35mI\033[m !!";
    std::cout << std::endl;

    // Load config file.
    load_config(args["config"]);

    // Create ReadCommand instance which reads user input with rich interface.
    ReadCommand readcmd;

    // Create CommandRunner instance which runs user input command.
    CommandRunner runner;

    while (true)
    {
        // Read user input. Returns value is lhs and rhs.
        // Use command runner's lhs and rhs string as a initial editing string.
        const StringX input = readcmd.read(runner.get_next_lhs(), runner.get_next_rhs());

        // Exit command loop if exit command is specified.
        if (input == StringX("exit") or input == StringX("^D"))
            break;

        // Print command.
        std::cout << "\033[37m[" << get_date() << " " << get_time() << "]\033[m " << input.colorize() << std::endl;

        // Run command.
        runner.run(input);
    }

    // Show farewell message.
    std::cout << "See you!" << std::endl;

    return EXIT_SUCCESS;

}   // }}}


// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
