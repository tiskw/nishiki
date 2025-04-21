////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: parse_args.cxx                                                              ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "parse_args.hxx"

// Include the header of the cxxopts library.
#include <getopt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Map<String, String> parse_args(int32_t argc, char* const argv[], const char* version)
{   // {{{

    // Initialize output variable.
    Map<String, String> args;

    // Define short options.
    const char* opt_string = "hi:v";

    // Define option details.
    constexpr struct option long_opts[] = {
        {"input",   required_argument, NULL, 'i'},
        {"help",    no_argument,       NULL, 'h'},
        {"version", no_argument,       NULL, 'v'},
        {NULL,      0,                 NULL,  0 }
    };

    // Define help message.
    const char* help_message =
        "NiShiKi - a simple shell wrapper\n"
        "\n"
        "Usage:\n"
        "  nishiki [OPTION...]\n"
        "\n"
        "  -i, --input arg  Use the given string instead of user input (default: '')\n"
        "  -h, --help       Show help message and exit\n"
        "  -v, --version    Show version info and exit";

    while (true)
    {
        // Declare index for long option parsing.
        int32_t long_index = 0;

        // Parse a command line argument and returns it's short option name.
        int32_t c = getopt_long(argc, argv, opt_string, long_opts, &long_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 'i': args["input"] = optarg;   break;
            case 'h': std::puts(help_message);  exit(EXIT_SUCCESS);
            case 'v': std::puts(version);       exit(EXIT_SUCCESS);
            default : std::puts(help_message);  exit(EXIT_FAILURE);
        }
    }

    return args;

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
