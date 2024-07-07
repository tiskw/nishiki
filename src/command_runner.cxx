////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: command_runner.cxx                                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "command_runner.hxx"

// Include the headers of STL.
#include <iostream>

// Include the headers of custom modules.
#include "config.hxx"
#include "file_chooser.hxx"
#include "text_chooser.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// File local macros
////////////////////////////////////////////////////////////////////////////////////////////////////

// Chack the given element is contained in the given set.
#define contains(str, ch) (str.find(ch) != std::string::npos)

////////////////////////////////////////////////////////////////////////////////////////////////////
// CommandRunner: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

CommandRunner::CommandRunner(void)
{ /* Do nothing */ }

////////////////////////////////////////////////////////////////////////////////////////////////////
// CommandRunner: Getter and setter functions
////////////////////////////////////////////////////////////////////////////////////////////////////

const StringX& CommandRunner::get_next_lhs(void) const noexcept { return this->lhs_next; }
const StringX& CommandRunner::get_next_rhs(void) const noexcept { return this->rhs_next; }

////////////////////////////////////////////////////////////////////////////////////////////////////
// CommandRunner: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
CommandRunner::run(const StringX& input)
noexcept
{   // {{{

    // Clear next editing buffer.
    this->lhs_next.clear();
    this->rhs_next.clear();

    // Do nothing if the given command is just a comment.
    if (input.strip().startswith(StringX("#")))
        return EXIT_SUCCESS;

    // Split user input to tokens.
    std::vector<StringX> tokens = input.strip().tokenize();

    // Apply alias.
    if (tokens.size() > 0 and config.aliases.find(tokens[0]) != config.aliases.end())
        tokens[0] = config.aliases[tokens[0]];

    // Concat all tokens and create command string.
    const std::string command = StringX("").join(tokens).string();

    // Drop white-space tokens.
    drop_whitespace_tokens(tokens);

    // Do nothing if the given command has no token.
    if (tokens.size() == 0)
        return EXIT_SUCCESS;

    // Process command "alias": list up all aliases.
    else if (tokens[0] == StringX("alias"))
        return this->command_alias();

    // Process command "cd": change directory.
    else if (tokens[0] == StringX("cd"))
        return this->command_cd(tokens);

    // Process NiShiKi-special command: modify editing buffer.
    else if (tokens[0].startswith(StringX(NISHIKI_CMD_DELIM)))
        return this->command_nishiki(command);

    // Process other commands: call external command.
    else
        return this->command_exec(command);

}; // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CommandRunner: Private member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
CommandRunner::command_alias(void)
const noexcept
{   // {{{

    // Print all aliases.
    for (const auto& pair : config.aliases)
        std::cout << pair.first << '\t' << pair.second << std::endl;

    return EXIT_SUCCESS;

}   // }}}

int32_t
CommandRunner::command_cd(const std::vector<StringX>& tokens)
const noexcept
{   // {{{

    // A variable to store a path to move.
    std::string target;

    // Set home directory if no directory path specified.
    if (tokens.size() < 2) target = getenv("HOME");
    else                   target = tokens[1].unquote().string();

    // Expand tilde (~) to $HOME.
    if ((target.size() > 0) && (target[0] == '~'))
        target = replace(target, "~", getenv("HOME"));

    // If the target token is not a directory path, then show error message.
    if (not std::filesystem::is_directory(target))
    {
        std::cout << "\033[33mNiShiKi: cd: not directory: " << target << "\033[m\n" << std::endl;
        return EXIT_FAILURE;
    }

    // Move to the target directory.
    std::filesystem::current_path(target);

    return EXIT_SUCCESS;

}   // }}}

int32_t
CommandRunner::command_exec(const std::string& command)
const noexcept
{   // {{{

    return system(command.c_str());

}   // }}}

int32_t
CommandRunner::command_nishiki(const std::string& command)
noexcept
{   // {{{

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // [Abstract]
    //   Print error message and contents of splitted tokens.
    //   This function will be called when parse error occurred.
    //
    // [Args]
    //   tokens (const std::vector<std::string>): [IN ] Tokenized user input.
    //   self   (CommandRunner*)                : [OUT] Pointer to myself.
    //
    // [Returns]
    //   (int32_t): Failure exit code (EXIT_FAILURE).
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto print_error_message = [](const std::vector<std::string>& tokens) noexcept
    {
        // Print error message.
        std::cout << "NiShiKi: parse error on NiShiKi-special command:" << std::endl;

        // Print the number of tokens.
        std::cout << "  - tokens.size() = " << tokens.size() << std::endl;

        // Print tokens for debuf purpose.
        for (uint32_t n = 0; n < tokens.size(); ++n)
            std::cout << "  - tokens[" << n << "] " << tokens[n] << std::endl;

        return EXIT_FAILURE;
    };

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // [Abstract]
    //
    // [Args]
    //   tokens      (const std::vector<std::string>): [IN] Tokenized user input.
    //   ret_cmd_out (const bool)                    : [IN] True if returns command output.
    //
    // [Returns]
    //   (int32_t): Failure exit code (EXIT_FAILURE).
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto run_external_command = [](const std::vector<std::string>& tokens, const bool ret_cmd_out = true) noexcept
    {
        // Create command string.
        std::string cmd = tokens[3];
        cmd = replace(cmd, "{lhs}", tokens[1]);
        cmd = replace(cmd, "{rhs}", tokens[2]);

        // If ret_cmd_out is True, then returns command output.
        if (ret_cmd_out)
            return run_command(strip(cmd));

        // Otherwise, run the command but returns an empty string.
        int32_t res = system(strip(cmd).c_str());
        if (res != 0)
            std::cout << "NiShiKi: command returned non-zero value: '" << strip(cmd) << "'" << std::endl;

        return std::string("");
    };

    // Split NiShiKi-style command to tokens.
    std::vector<std::string> tokens = split(command, NISHIKI_CMD_DELIM);

    // Ignore empty tokens on the top.
    while ((tokens.size() > 0) and (tokens[0].size() == 0))
        tokens.erase(tokens.begin());

    // The number of splitted token should be four.
    if (tokens.size() != 4)
        return print_error_message(tokens);

    // Each token means:
    //   - tokens[0]: Command type, ext (external) or int (internal).
    //   - tokens[1]: Left hand side string.
    //   - tokens[2]: Right hand side string.
    //   - tokens[3]: Command contents.

    // Set next right hand side editing buffer.
    this->rhs_next = StringX(tokens[2]);

    // Command 1-1: external command: insertion with left and right hand side string.
    if (tokens[0] == "ext")
        this->lhs_next = StringX(tokens[1] + run_external_command(tokens, true));

    // Command 1-2: external command:
    else if (startswith(tokens[0], "ext["))
    {
        // Process 'i' specifier: insert command results.
        this->lhs_next = StringX(run_external_command(tokens, contains(tokens[0], 'i')));

        // Process 'l' specifier: append left hand side string.
        if (contains(tokens[0], 'l'))
            this->lhs_next = StringX(tokens[1]) + this->lhs_next;

        // Process 'r' specifier: append right hand side string.
        if (!contains(tokens[0], 'r')) this->rhs_next.clear();
    }

    // Command 1: filechooser
    else if ((tokens[0] == "int") and (tokens[3] == "filechooser"))
        this->lhs_next = StringX(tokens[1]) + StringX(" ").join(choose_files(), true);

    // Command 2: histchooser
    else if ((tokens[0] == "int") and (tokens[3] == "histchooser"))
        this->lhs_next = StringX(tokens[1]) + StringX(" ").join(choose_hists(), true);

    // Command 3: procchooser
    else if ((tokens[0] == "int") and (tokens[3] == "procchooser"))
        this->lhs_next = StringX(tokens[1]) + StringX(" ").join(choose_procs(), true);

    // Command 4: parse error
    else
        return print_error_message(tokens);

    // Delete extra whitespaces from the bottom of lhs_next if rhs_next starts with white space,
    // in order to avoid the double whitespaces after filename completion.
    if ((tokens[2].size() > 0) and (tokens[2][0] == ' '))
        this->lhs_next = this->lhs_next.strip(false, true);

    return EXIT_SUCCESS;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
