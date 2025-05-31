////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: command_runner.cxx                                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "cmd_runner.hxx"

// Include the headers of STL.
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>

// Include the headers of custom modules.
#include "config.hxx"
#include "path_x.hxx"
#include "utils.hxx"
#include "variables.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Tuple<StringX, StringX> command_alias(void) noexcept
{   // {{{

    // Print all aliases.
    for (const auto& pair : config.aliases)
        std::printf("%s\t%s\n", pair.first.c_str(), pair.second.c_str());

    return {StringX(""), StringX("")};

}   // }}}

Tuple<StringX, StringX> command_cd(const Vector<StringX>& tokens) noexcept
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
        std::printf("\033[33mNiShiKi: cd: not directory: %s\033[m\n", target.c_str());
        return {StringX(""), StringX("")};
    }

    // Move to the target directory.
    std::filesystem::current_path(target);

    return {StringX(""), StringX("")};

}   // }}}

Tuple<StringX, StringX> command_exec(const Vector<StringX>& tokens) noexcept
{   // {{{

    // Generate command string by concatenating all tokens and run it.
    int32_t res = std::system(StringX(" ").join(tokens).string().c_str());

    // Append the result value to NiShiKi internal variables.
    variables["?"] = std::to_string(res);

    return {StringX(""), StringX("")};

}   // }}}

Tuple<StringX, StringX> command_plugin(const Vector<StringX>& tokens) noexcept
{   // {{{

    // Compute the path of the specified plugin.
    // Note that the "!" at the beginning of the first token should be removed.
    PathX path = PathX(config.path_plugins) / tokens[0].string().substr(1);

    // Compute the temporary file path.
    PathX path_tmp = PathX("/tmp") / (get_random_string(16) + ".txt");

    // Get the tokens except the first token.
    Vector<StringX> sub_tokens = {tokens.begin() + 1, tokens.end()};

    // Concat all tokens and create command string.
    const String command = path.string() + " " + StringX(" ").join(sub_tokens).string() + " --out " + path_tmp.string();

    // Generate command string by concatenating all tokens and run it.
    if (std::system(command.c_str()) != 0)
        std::printf("NiShiKi: Warning: Plugin command returns non-zero code: %s\n", command.c_str());

    // Print error message and returns empty strings if no output file exists.
    if (not path_tmp.exists())
    {
        std::printf("NiShiKi: Warning: The output file not found: %s\n", path_tmp.c_str());
        return {StringX(""), StringX("")};
    }

    // Read the output file.
    Vector<String> lines = read_lines(path_tmp.string());

    // Set the first line of the output file to "rhs", and second line to "rhs".
    StringX lhs, rhs;
    if (lines.size() > 0) { lhs = StringX(lines[0].c_str()); }
    if (lines.size() > 1) { rhs = StringX(lines[1].c_str()); }

    return {lhs, rhs};

}   // }}}

Tuple<StringX, StringX> command_set(const Vector<StringX>& tokens) noexcept
{   // {{{

    // Declare the use of global variable.
    extern char **environ;

    // Case 1: "set" => print all environment variables.
    if (tokens.size() == 1)
        for (char** env_ptr = environ; *env_ptr != nullptr; ++env_ptr)
            std::puts(*env_ptr);

    // Case 2: "set -x NAME VALUE" => set as an environment variable.
    else if ((tokens.size() == 4) and ((tokens[1] == StringX("-x")) or (tokens[1] == StringX("--export"))))
        setenv(tokens[2].string().c_str(), tokens[3].string().c_str(), 1);

    // Case 3: "set -e NAME" => delete environment variable.
    else if ((tokens.size() == 3) and ((tokens[1] == StringX("-e")) or (tokens[1] == StringX("--erase"))))
        unsetenv(tokens[2].string().c_str());

    return {StringX(""), StringX("")};

}   // }}}

Tuple<StringX, StringX> command_var(const Vector<StringX>& tokens) noexcept
{   // {{{

    // Case 1: "var" => print all environment variables.
    if (tokens.size() == 1)
    {
        // Get sorted keys of the variable map.
        Vector<String> keys;
        for (const auto& var : variables)
            keys.push_back(var.first);
        std::sort(keys.begin(), keys.end());

        for (const String& key : keys)
            std::printf("%s = %s\n", key.c_str(), variables[key].c_str());
    }

    // Case 2: "set NAME VALUE" => set the variable.
    else if (tokens.size() == 3)
        variables[tokens[1].string()] = tokens[2].string();

    return {StringX(""), StringX("")};

}   // }}}

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

    // Do nothing if the given command is Ctrl-C or just a comment.
    if ((command == StringX("^C")) or command.strip().startswith(StringX("#")))
        return {lhs_next, rhs_next};

    // Split user input to tokens.
    Vector<StringX> tokens = command.strip().tokenize();

    // Replace variable names to their values.
    for (Vector<StringX>::size_type idx = 0; idx < tokens.size(); ++idx)
    {
        // Ignore if the token is not a variable.
        if ((tokens[idx].front().value != '{') or (tokens[idx].back().value != '}'))
            continue;

        // Get the variable name.
        String name = tokens[idx].substr(1, tokens[idx].size() - 2).string();

        // Replace to the variable name to it's value.
        if (variables.contains(name))
            tokens[idx] = StringX(variables[name].c_str());
    }

    // Apply alias.
    if (tokens.size() > 0 and config.aliases.find(tokens[0].string()) != config.aliases.end())
        tokens[0] = StringX(config.aliases[tokens[0].string()].c_str());

    // Drop white-space tokens.
    drop_whitespace_tokens(tokens);

    // Do nothing if the given command has no token.
    if (tokens.size() == 0)
        return {lhs_next, rhs_next};

    // Process command "alias": list up all aliases.
    else if (tokens[0] == StringX("alias"))
        return command_alias();

    // Process command "cd": change directory.
    else if (tokens[0] == StringX("cd"))
        return command_cd(tokens);

    // Process command "set": set environment.
    else if (tokens[0] == StringX("set"))
        return command_set(tokens);

    // Process command "set": set environment.
    else if (tokens[0] == StringX("var"))
        return command_var(tokens);

    // Process plugin command: call plugin command and modify buffer strings.
    else if (tokens[0].startswith(StringX("!")))
        return command_plugin(tokens);

    // Process other commands: call external command.
    else return command_exec(tokens);

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
