////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: parse_args.cxx                                                              ///
///                                                                                              ///
/// Parse command line arguments                                                                 ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "parse_args.hxx"

// Include the headers of STL.
#include <iostream>

// Include the header of the cxxopts library.
#include <cxxopts.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Map<String, String> parse_args(int32_t argc, const char* argv[], const char* version)
{   // {{{

    // Create parser instance.
    cxxopts::Options options("nishiki", "NiShiKi - a simple shell wrapper\n");

    // Configure parser.
    options.add_options()
        ("i,input",   "Use the given string instead of user input", cxxopts::value<String>()->default_value(""))
        ("h,help",    "Show help message and exit",                 cxxopts::value<bool>())
        ("v,version", "Show version info and exit",                 cxxopts::value<bool>());

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

    // Option: -c,--command.
    args["input"] = result["input"].as<String>();

    // Option: -h,--help and -v,--version.
    if (result.count("help")   ) { std::cout << options.help() << std::endl; exit(EXIT_SUCCESS); }
    if (result.count("version")) { std::cout << version        << std::endl; exit(EXIT_SUCCESS); }

    return args;

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
