////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: main.cxx                                                                    ///
///                                                                                              ///
/// Entry point of NiShiKi.                                                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the headers of STL.
#include <iostream>

// Include the header of the cxxopts library.
#include <cxxopts.hpp>

// Include the headers of custom modules.
#include "nishiki.hxx"

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

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
main(const int32_t argc, const char* argv[])
// [Abstract]
//   Entry point of NiSHiKi. This function just call "nishiki_main" function (written in 
//   nishiki.cxx). This structure will not be changed because the test code rely on the structure.
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

    // Parse command line arguments and call the NiShiKi's main function.
    return nishiki_main(parse_args(argc, argv));

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
