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
        std::cout << "\033[33mNiShiKi: cd: no such file or directory: " << target << "\033[m\n" << std::endl;
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
    // Print error message and contents of splitted tokens.
    // This function will be called when parse error occurred.
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
    constexpr auto print_error_message = [](const std::vector<std::string>& tokens)
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

    // Split NiShiKi-style command to tokens.
    std::vector<std::string> tokens = split(command, NISHIKI_CMD_DELIM);

    // Erase the first token if the first token is empty.
    if ((tokens.size() > 0) and (tokens[0].size() == 0))
        tokens.erase(tokens.begin());

    // The number of splitted token should be four.
    if (tokens.size() != 4)
        return print_error_message(tokens);

    // Command 0: external command.
    if (tokens[0] == "ext")
        this->lhs_next = StringX(tokens[1]) + StringX(run_command(strip(tokens[3])));

    // Command 1: filechooser
    else if ((tokens[0] == "int") && (tokens[3] == "filechooser"))
        this->lhs_next = StringX(tokens[1]) + StringX(" ").join(choose_files(StringX(".")), true);

    // Command 2: histchooser
    else if ((tokens[0] == "int") && (tokens[3] == "histchooser"))
        this->lhs_next = StringX(tokens[1]) + StringX(" ").join(choose_hists(), true);

    // Command 3: procchooser
    else if ((tokens[0] == "int") && (tokens[3] == "procchooser"))
        this->lhs_next = StringX(tokens[1]) + StringX(" ").join(choose_procs(), true);

    // Command 4: parse error
    else
        return print_error_message(tokens);

    // Set next right hand side editing buffer.
    this->rhs_next = StringX(tokens[2]);

    return EXIT_SUCCESS;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
