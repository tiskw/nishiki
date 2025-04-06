////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: main.cxx                                                                    ///
///                                                                                              ///
/// Main function of NiShiKi.                                                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "main.hxx"

// Include the headers of STL.
#include <iostream>
#include <regex>
#include <signal.h>

// Include the headers of custom modules.
#include "config.hxx"
#include "cmd_runner.hxx"
#include "hist_manager.hxx"
#include "parse_args.hxx"
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

    constexpr auto get_value = [](const String& target, Map<String, String>& var_cache) noexcept -> String
    // [Abstract]
    //   Get the specified value.
    //
    // [Args]
    //   target    (String)             : Query string.
    //   var_cache (Map<String, String>): Cache of the values.
    //
    // [Returns]
    //   (String): Value corresponding to the query.
    {
        // Returns the corresponding value immediately if it exists in the "var_cache".
        if (var_cache.contains(target))
            return var_cache[target];

        // Get the corresponding value and store it inside the "var_cache".
        switch (hash(target.c_str()))
        {
            case hash("{cwd}") : var_cache[target] = get_cwd();         break;
            case hash("{date}"): var_cache[target] = get_date();        break;
            case hash("{host}"): var_cache[target] = getenv("NAME");    break;
            case hash("{git}") : var_cache[target] = get_git_info();    break;
            case hash("{time}"): var_cache[target] = get_time();        break;
            case hash("{user}"): var_cache[target] = getenv("LOGNAME"); break;
            default            : var_cache[target] = "???";             break;
        }

        // Returns the corresponding value.
        return var_cache[target];
    };

    constexpr auto process_ps = [get_value](const String& ps, Map<String, String>& var_cache) noexcept -> String
    // [Abstract]
    //   Process one prompt string.
    //
    // [Args]
    //   ps        (String)             : Base prompt string.
    //   var_cache (Map<String, String>): Cache of values.
    //
    // [Returns]
    //   (String): Prompt string.
    {
        // Make a copy of the input prompt string.
        String str = ps;

        // Define the pattern of variables in the prompt strings.
        std::regex pattern("\\{[\\w]+\\}");

        // Replace the variables in the prompt string to the actual values.
        for (std::sregex_iterator iter(ps.begin(), ps.end(), pattern), end; iter != end; ++iter)
            str = replace(str, iter->str(), get_value(iter->str(), var_cache));

        return str;
    };

    // Define a cache of variables in the prompt strings.
    Map<String, String> var_cache;

    return {process_ps(config.ps0,  var_cache),
            process_ps(config.ps1i, var_cache),
            process_ps(config.ps1n, var_cache),
            process_ps(config.ps2,  var_cache)};

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t main(int32_t argc, const char* argv[])
{   // {{{

    // Ignore Ctrl-C signal.
    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
        exit(-1);

    HistManager   histmn;
    CommandRunner runner;

    // Get terminal size.
    TermSize term_size = get_terminal_size();

    // Parse command line arguments.
    Map<String, String> args = parse_args(argc, argv, VERSION);

    // Show welcome message.
    std::cout << "Welcome to ";
    std::cout << "\x1B[31mN \x1B[35mI \x1B[32mS \x1B[33mH \x1B[35mI \x1B[36mK \x1B[35mI\x1B[m !!";
    std::cout << std::endl;

    // Initialize the left and right hand side strings.
    StringX lhs, rhs;

    // Prepare the input string.
    StringX input_str = StringX(replace(args["input"], "\\n", "\n"));

    while (true)
    {
        // Get prompt strings.
        const auto [ps0, ps1i, ps1n, ps2] = get_prompt_strings();

        // Print the zero-th prompt.
        std::cout << '\n' << ps0 << std::endl;

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
        histmn.append(input);

        // Erase the zero-th prompt.
        std::cout << "\x1B[1F" << "\x1B[0K";

        // Draw a horizontal line.
        std::cout << config.horiz_line_color;
        for (uint16_t n = 0; n < (term_size.cols - 1); ++n)
            std::cout << config.horiz_line_char;
        std::cout << "\x1B[m\n";

        // Print command.
        std::cout << config.datetime_pre << get_date() << " " << get_time() << config.datetime_post;
        std::cout << " " << input.colorize() << std::endl;

        // Run command.
        std::tie(lhs, rhs) = runner.run(input);
    }

    // Show farewell message.
    std::cout << "See you!" << std::endl;

    return EXIT_SUCCESS;

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
