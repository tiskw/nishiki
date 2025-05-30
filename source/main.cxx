////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: main.cxx                                                                    ///
///                                                                                              ///
/// Main function of NiShiKi.                                                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "main.hxx"

// Include the headers of STL.
#include <cstdio>
#include <regex>

// Include the headers of custom modules.
#include "config.hxx"
#include "cmd_runner.hxx"
#include "hist_manager.hxx"
#include "parse_args.hxx"
#include "path_x.hxx"
#include "read_cmd.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static Tuple<String, String, String, String> get_prompt_strings()
// [Abstract]
//   Get prompt strings after filling placeholders.
//
// [Returns]
//   (Tuple<String, String, String, String>): A tuple of prompt strings.
//
{   // {{{

    // Initialize prompt strings.
    String ps0  = config.ps0;
    String ps1i = config.ps1i;
    String ps1n = config.ps1n;
    String ps2  = config.ps2;

    // Compute path to the "getpstr" plugin.
    PathX path_getpstr = PathX(config.path_plugins) / "getpstr";

    // Use the config values if "getpstr" does not exist.
    if (not path_getpstr.exists())
        return {ps0, ps1i, ps1n, ps2};

    // Run the "getpstr" plugin and get it's outputs.
    Vector<String> lines = split(strip(run_command(path_getpstr.string())), "\n");

    // Replace the config values.
    if (lines.size() > 0) ps0  = lines[0];
    if (lines.size() > 1) ps1i = lines[1];
    if (lines.size() > 2) ps1n = lines[2];
    if (lines.size() > 3) ps2  = lines[3];

    return {ps0, ps1i, ps1n, ps2};

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t main(int32_t argc, char* const argv[])
{   // {{{

    // Instanciate necessary classes.
    HistManager   histmn;
    CommandRunner runner;

    // Get terminal size.
    TermSize term_size = get_terminal_size();

    // Parse command line arguments.
    Map<String, String> args = parse_args(argc, argv, VERSION);

    // Show welcome message.
    PathX path_welcome = PathX(config.path_plugins) / "welcome";
    if (path_welcome.exists()) std::printf("%s\n", run_command(path_welcome.string(), true).c_str());
    else                       std::printf("%s\n", "Welcome to NiShiKi!");

    // Initialize the left and right hand side strings.
    StringX lhs, rhs;

    // Prepare the input string.
    StringX input_str = StringX(replace(args["input"], "\\n", "\n").c_str());

    while (true)
    {
        // Get prompt strings.
        const auto [ps0, ps1i, ps1n, ps2] = get_prompt_strings();

        // Print the zero-th prompt.
        std::puts("");
        if (ps0.size() > 0)
            std::puts(ps0.c_str());

        // Read user input. Returns value is lhs and rhs.
        // Use command runner's lhs and rhs string as a initial editing string.
        std::tie(lhs, rhs) = readcmd(lhs, rhs, histmn.get_hists(), config.area_height, ps1i, ps1n,
                                     ps2, config.histhint_pre, config.histhint_post, input_str);

        // Concatenate the left and right hand side of the user input.
        StringX input = lhs + rhs;

        // Exit command loop if exit command is specified.
        if (input == StringX("exit") or input == StringX("^D"))
            break;

        // Append the user input to the history manager.
        // NOTE: Plugin commands (starts with "!") are ignored.
        if ((input.size() > 0) and (input[0].value != '!'))
            histmn.append(input);

        // Erase the zero-th prompt.
        if (ps0.size() > 0)
            std::printf("\x1B[1F\x1B[0K");

        // Draw a horizontal line.
        std::fputs(config.horizontal_line_color, stdout);
        for (uint16_t n = 0; n < (term_size.cols - 1); ++n)
            std::fputs(config.horizontal_line_char, stdout);
        std::fputs("\x1B[m\n", stdout);

        // Print command.
        std::printf("%s%s %s%s", config.datetime_pre, get_date().c_str(), get_time().c_str(), config.datetime_post);
        std::printf(" %s\n", input.colorize().string().c_str());

        // Run command.
        std::tie(lhs, rhs) = runner.run(input);
    }

    // Show farewell message.
    std::printf("See you!\n");

    return EXIT_SUCCESS;

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
