////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: command_runner.cxx                                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "cmd_runner.hxx"

// Include the headers of STL.
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

// Include the headers of custom modules.
#include "config.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// CommandRunner: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

CommandRunner::CommandRunner(void)
{ /* Do nothing */ }

////////////////////////////////////////////////////////////////////////////////////////////////////
// CommandRunner: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Tuple<StringX, StringX> CommandRunner::run(const StringX& command) const noexcept
{   // {{{

    // Clear next editing buffer.
    StringX lhs_next, rhs_next;

    // Do nothing if the given command is just a comment.
    if (command.strip().startswith(StringX("#")))
        return {lhs_next, rhs_next};

    // Split user input to tokens.
    std::vector<StringX> tokens = command.strip().tokenize();

    // Apply alias.
    if (tokens.size() > 0 and config.aliases.find(tokens[0].string()) != config.aliases.end())
        tokens[0] = StringX(config.aliases[tokens[0].string()]);

    // Drop white-space tokens.
    drop_whitespace_tokens(tokens);

    // Do nothing if the given command has no token.
    if (tokens.size() == 0)
        return {lhs_next, rhs_next};

    // Process command "alias": list up all aliases.
    else if (tokens[0] == StringX("alias"))
        return this->command_alias();

    // Process command "cd": change directory.
    else if (tokens[0] == StringX("cd"))
        return this->command_cd(tokens);

    // Process command "set": set environment.
    else if (tokens[0] == StringX("set"))
        return this->command_set(tokens);

    // Process plugin command: call plugin command and modify buffer strings.
    else if (tokens[0].startswith(StringX("!")))
        return this->command_plugin(tokens);

    // Process other commands: call external command.
    else return this->command_exec(tokens);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CommandRunner: Private member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Tuple<StringX, StringX> CommandRunner::command_alias(void) const noexcept
{   // {{{

    // Print all aliases.
    for (const auto& pair : config.aliases)
        std::cout << pair.first << '\t' << pair.second << std::endl;

    return {StringX(""), StringX("")};

}   // }}}

Tuple<StringX, StringX> CommandRunner::command_cd(const Vector<StringX>& tokens) const noexcept
{   // {{{

    // A variable to store a path to move.
    String target;

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
        return {StringX(""), StringX("")};
    }

    // Move to the target directory.
    std::filesystem::current_path(target);

    return {StringX(""), StringX("")};

}   // }}}

Tuple<StringX, StringX> CommandRunner::command_exec(const Vector<StringX>& tokens) const noexcept
{   // {{{

    // Generate command string by concatenating all tokens and run it.
    int32_t res = std::system(StringX(" ").join(tokens).string().c_str());

    // TODO: Append the result value to NiShiKi internal variables.

    return {StringX(""), StringX("")};

}   // }}}

Tuple<StringX, StringX> CommandRunner::command_plugin(const Vector<StringX>& tokens) const noexcept
{   // {{{

    // Compute the path of the specified plugin.
    // Note that the "!" at the beginning of the first token should be removed.
    Path path = Path(replace(config.path_plugins, "~", getenv("HOME"))) / tokens[0].string().substr(1);

    // Compute the temporary file path.
    Path path_tmp = Path("/tmp") / (get_random_string(16) + ".txt");

    // Get the tokens except the first token.
    Vector<StringX> sub_tokens = {tokens.begin() + 1, tokens.end()};

    // Concat all tokens and create command string.
    const String command = path.string() + " " + StringX(" ").join(sub_tokens).string() + " --out " + path_tmp.string();

    // Generate command string by concatenating all tokens and run it.
    int32_t res = std::system(command.c_str());
    if (res != 0)
        std::cout << "NiShiKi: Warning: Plugin command returns non-zero code: " << command << std::endl;

    // Print error message and returns empty strings if no output file exists.
    if (not std::filesystem::exists(path_tmp))
    {
        std::cout << "NiShiKi: Warning: The output file not found: " << path_tmp << std::endl;
        return {StringX(""), StringX("")};
    }

    // Read the output file.
    Vector<String> lines = read_lines(path_tmp.string());

    // Set the first line of the output file to "rhs", and second line to "rhs".
    StringX lhs, rhs;
    if (lines.size() > 0) { lhs = StringX(lines[0]); }
    if (lines.size() > 1) { rhs = StringX(lines[1]); }

    return {lhs, rhs};

}   // }}}

Tuple<StringX, StringX> CommandRunner::command_set(const std::vector<StringX>& tokens) const noexcept
{   // {{{

    // Declare the use of global variable.
    extern char **environ;

    // Case 1: "set" => print all environment variables.
    if (tokens.size() == 1)
        for (char** env_ptr = environ; *env_ptr != nullptr; ++env_ptr)
            std::cout << *env_ptr << std::endl;

    // Case 2: "set -x NAME VALUE" => set as an environment variable.
    else if ((tokens.size() == 4) and ((tokens[1] == StringX("-x")) or (tokens[1] == StringX("--export"))))
        setenv(tokens[2].string().c_str(), tokens[3].string().c_str(), 1);

    // Case 3: "set -e NAME" => delete environment variable.
    else if ((tokens.size() == 3) and ((tokens[1] == StringX("-e")) or (tokens[1] == StringX("--erase"))))
        unsetenv(tokens[2].string().c_str());

    return {StringX(""), StringX("")};

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
