////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: config.hxx                                                                  ///
///                                                                                              ///
/// This file provides a globally accessible variable that describes the config data for NiShiKi.///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_HXX
#define CONFIG_HXX

// Include the headers of custom modules.
#include "dtypes.hxx"
#include "edit_helper.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Data type declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    ////////////////////////////////////////////////////////////////////////////
    // General settings.
    ////////////////////////////////////////////////////////////////////////////

    // Maximum number of history items.
    uint16_t max_hist_size = 5000;

    // Height of the drawing area.
    uint8_t area_height = 6;

    // Margin of column display (in the completion list).
    uint16_t column_margin = 3;

    // Path to history file.
    const char* path_history = "~/.local/share/nishiki/history.txt";

    // Path to plugin directory.
    const char* path_plugins = "~/.config/nishiki/plugins";

    // Prefix and postfix strings of the datetime string.
    const char* datetime_pre = "\x1B[38;2;112;120;128m[";
    const char* datetime_post = "]\x1B[0m";

    // Prefix and postfix strings of the command history completions.
    const char* histhint_pre = "\x1B[38;2;112;120;128m";
    const char* histhint_post = "\x1B[0m";

    // Horizontal line character and it's color.
    const char* horizontal_line_char = "⎯";
    const char* horizontal_line_color = "\x1B[38;2;112;120;128m";

    ////////////////////////////////////////////////////////////////////////////
    // Prompt strings.
    ////////////////////////////////////////////////////////////////////////////

    // Zero-th prompt string.
    const char* ps0 = "\x1B[38;2;204;102;102m{user} \x1B[38;2;197;200;198m: "
                      "\x1B[38;2;181;189;104m{host} \x1B[38;2;197;200;198m: "
                      "\x1B[38;2;240;198;116m{date} \x1B[38;2;197;200;198m: "
                      "\x1B[38;2;129;162;190m{time} \x1B[38;2;197;200;198m: "
                      "\x1B[38;2;178;148;187m{cwd}\x1B[0m";

    // First prompt string when insert mode.
    const char* ps1i = "\x1B[38;2;197;200;198m==>\x1B[0m ";

    // First prompt string when normal mode.
    const char* ps1n = "\x1B[38;2;197;200;198m<<=\x1B[0m ";

    // Second prompt string.
    const char* ps2 = "\x1B[38;2;197;200;198m...\x1B[0m ";

    ////////////////////////////////////////////////////////////////////////////
    // Alias settings.
    ////////////////////////////////////////////////////////////////////////////

    // Collection of command aliases.
    Map<String, String> aliases = {

        // "ls" command shortcuts.
        {"ls", "ls -h --color=auto --group-directories-first"},
        {"ll", "ls -hl --color=auto --group-directories-first"},
        {"la", "ls -ahl --color=auto --group-directories-first"},
        {"sl", "ls -h --color=auto --group-directories-first"},

        // Prevent suddern file overwriting.
        {"cp", "cp -i"},
        {"mv", "mv -i"},
        {"rm", "rm -i"},

        // Colorize commands.
        {"diff", "diff --color=auto"},
        {"grep", "grep --color=auto"},
        {"ip",   "ip --color=auto"}
    };

    ////////////////////////////////////////////////////////////////////////////
    // Keybind settings.
    ////////////////////////////////////////////////////////////////////////////

    // Map of input key and command.
    // Please use integers to represent input keys.
    // For example, Ctrl-A=0x01, Ctrl-B=0x02, ..., Ctrl-Z=0x1A.
    Map<uint64_t, String> keybinds = {
        {0x06, "!chooser --lhs '{lhs}' --rhs '{rhs}' --mode file "},              // Ctrl-F
        {0x15, "!chooser --lhs '{lhs}' --rhs '{rhs}' --mode hist --hist {hist}"}, // Ctrl-U
        {0x10, "!chooser --lhs '{lhs}' --rhs '{rhs}' --mode proc "},              // Ctrl-P
        {0x16, "!ext_cmd --lhs '{lhs}' --rhs '{rhs}' --cmd 'xclip -o'"},          // Ctrl-V
        {0x0C, "clear"},                                                          // Ctrl-L
    };

    ////////////////////////////////////////////////////////////////////////////
    // Colorization settings.
    ////////////////////////////////////////////////////////////////////////////

    // Comma-seperated values of tokens that are colorized on the editing line.
    String colorize_commands = "cat,cd,chmod,chown,cp,echo,env,export,grep,let,ln,ls,make,mkdir,mv,rm,sed,set,tar,touch,umask,unset";
    String colorize_keywords = "case,do,done,elif,else,esac,exit,fi,for,function,if,in,local,read,return,select,shift,then,time,until,while";
    String colorize_symbols  = "&,|,>,<,&&,||,>>,<<";

    ////////////////////////////////////////////////////////////////////////////
    // Completion settings.
    ////////////////////////////////////////////////////////////////////////////

    Vector<Tuple<Vector<String>, EditHelper::CompType, String>> completions = {

        // Docker command completions.
        {{"docker", "exec", ">>", ".*"}, EditHelper::CompType::SHELL,  "docker container ls -a --format '{{.Names}}'"},
        {{"docker", "run",  ">>", ".*"}, EditHelper::CompType::SHELL,  "docker image ls --format '{{.Repository}}:{{.Tag}}'"},
        {{"docker", ".*"},               EditHelper::CompType::SUBCMD, "docker --help | grep -E '^  [^ -]'"},

        // Git command completions.
        {{"git", "branch",   ">>", ".*"}, EditHelper::CompType::SHELL,  "git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"},
        {{"git", "checkout", ">>", ".*"}, EditHelper::CompType::SHELL,  "git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"},
        {{"git", "merge",    ">>", ".*"}, EditHelper::CompType::SHELL,  "git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"},
        {{"git", "push",     ">>", ".*"}, EditHelper::CompType::SHELL,  "git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"},
        {{"git", ".*"},                   EditHelper::CompType::SUBCMD, "git --help | grep -E '^   [^ ]'"},

        // Ssh command completions.
        {{"ssh", ".*"}, EditHelper::CompType::SHELL, "cat ~/.ssh/config 2>/dev/null | grep '^Host ' 2>/dev/null | cut -b 6-"},

        // Low priority completions.
        {{"[./~].*"},        EditHelper::CompType::PATH,    ""},
        {{".+",},            EditHelper::CompType::COMMAND, ""},
        {{">>", "-.*"},      EditHelper::CompType::OPTION,  ""},
        {{">>", "FILE", ""}, EditHelper::CompType::PREVIEW, ""},
        {{">>", ".*"},       EditHelper::CompType::PATH,    ""},
    };

    ////////////////////////////////////////////////////////////////////////////
    // Preview settings.
    ////////////////////////////////////////////////////////////////////////////

    // Collection of MIME type and it's preview command.
    // The {path} in the command strings will be replaced to the path to the file
    // to be previewd.
    //
    // NOTE: DO ONT USE double quote characters (") inside the command. Please
    //       use single quote (') instead.
    Vector<Pair<String, String>> previews = {

        // Directory.
        {"inode/directory", "timeout 0.1s ls -hl --color=always --group-directories-first '{path}'"},

        // Text file: colorize file.
        {"text/.*",          "timeout 0.1s highlight --force -O ansi '{path}'"},
        {"application/x-sh", "timeout 0.1s highlight --force -O ansi '{path}'"},

        // Media file: display media infomation.
        {"image/.*", "file '{path}' | sed 's/, /\\n/g'"},
        {"video/.*", "file '{path}' | sed 's/, /\\n/g'"},

        // Archive file: list archive contents.
        {".*/x-7z-compressed", "timeout 0.1s 7z l    '{path}'"},
        {".*/zip",             "timeout 0.1s zipinfo '{path}'"},
    };

    // Delimiter of the preview window.
    String preview_delim = " │ ";

    // Width of the preview window.
    float preview_ratio = 0.45;
}
NishikiConfig;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototype declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

extern NishikiConfig config;

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
