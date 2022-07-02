"""
This `config` module provides `NishikiConfig` which is a collection of immutable constant
values as a named tuple that are used as configuration values of Nishiki.
"""

# Import standard libraries.
import os, typing


class NishikiConfig(typing.NamedTuple):
    """
    This class provides a collection of immutable constant values as a named tuple for
    customizing the configuration of Nishiki.
    """

    ################################################################################################
    # Basic settings.
    ################################################################################################

    # Maximum window heights (prompts, user inputs and completion lines).
    height: int = 8

    # Path to a history file.
    history_path: str = os.path.expanduser("~/.config/nishiki/history.txt")

    # Maximum size of the history.
    history_size: int = 10000

    # Path to a temporary file which is used for storing plugin's output.
    plugin_dump_path: str = "/tmp/nishiki/plugin_tmp_dump.txt"

    # Hint (history completion) color.
    history_hint_color: str = "\x1b[90m"

    # Welcome message.
    welcome_message: str = "Welcome to {C1}N {C5}I {C2}S {C3}H {C5}I {C4}K {C5}I {CX}!!\n"

    ################################################################################################
    # Prompt settings.
    #
    # In this section, the following variables are available:
    #   * {mode}: Editing mode ("[N]" or "[I]").
    #   * {user}: Login user name.
    #   * {host}: Host name.
    #   * {cwd} : Current directory.
    #   * {git} : Git branch and status with icon.
    ################################################################################################

    # Short prompt shown before the user input.
    P1: str = "\x1b[37m>>\x1b[0m "

    # Not used now.
    P2: str = ""

    # Long prompt shown above the user input line.
    H1: str = "\x1b[31m[{mode}] \x1b[37m: \x1b[32m{user} \x1b[37m: \x1b[33m{host} \x1b[37m: \x1b[34m{cwd} \x1b[0m"

    # Short info tooltip placed at the right end of the long prompt.
    H2: str = "{git}"

    ################################################################################################
    # Alias settings.
    ################################################################################################

    # A dictionary of the command aliases table.
    # The keys of the table are user input commands and the values are corresponding aliases.
    aliases: dict = {
        # Special key aliases.
        "^D": "exit",
        "^L": "clear",

        # Command aliases.
        "cp"  : "cp -i",
        "grep": "grep --color=auto",
        "la"  : "ls -ahl --color=auto --group-directories-first",
        "ls"  : "ls -h --color=auto --group-directories-first",
        "ll"  : "ls -lh --color=auto --group-directories-first",
        "mv"  : "mv -i",
        "rm"  : "rm -i",
    }

    ################################################################################################
    # Completion settings.
    ################################################################################################

    # An ordered dictionary of the command completions table.
    # The key of the table is a list of regular expressions with the following extra special commands:
    #   * "FILE": existing file path
    #   * ">>"  : skip tokens (available only 1 time in one patterns)
    #
    # Examples:
    #   * example 1: ["git", "push", ">>"] matches with "git push hoge fuga"
    #   * example 2: [">> "-.*"] matches with "anycommand -l"
    #   * example 3: [">> "FILE", ""] matches with "ls /some/existing/file/path "
    completions: list = [
        # Docker command completions.
        (("docker", "exec", ".*"), "shell",  r""),
        (("docker", "run",  ".*"), "shell",  r"docker image list --format '{{.Repository}}:{{.Tag}}'"),
        (("docker", ".*"),         "subcmd", r"docker --help | grep -E '^  [^ -]'"),

        # Git command completions.
        (("git", "branch",   ".*"), "shell",  r"git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"),
        (("git", "checkout", ".*"), "shell",  r"git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"),
        (("git", "merge",    ".*"), "shell",  r"git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"),
        (("git", "push",     ".*"), "shell",  r"git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"),
        (("git", ".*"),             "subcmd", r"git --help | grep -E '^   [^ ]'"),

        # Ssh command completions.
        (("ssh", ".*"), "shell", "cat ~/.ssh/config | grep '^Host ' | cut -b 6-"),

        # Low priority completions.
        (("[./~].*",),       "path",    r"./"),
        ((".+",),            "command", None),
        ((">>", "-.*"),      "option",  None),
        ((">>", "FILE", ""), "preview", None),
        ((">>", ".*"),       "path",    r"./"),
    ]

    # Ratio of the preview window.
    preview_window_ratio: float = 0.45

    ################################################################################################
    # Keybind settings.
    ################################################################################################

    # A list of keybinds.
    keybinds: dict = {
        "^E": f"nvim {plugin_dump_path}",
        "^F": f"~/.config/nishiki/plugins/filechooser -o {plugin_dump_path}",
        "^P": f"~/.config/nishiki/plugins/textchooser -o {plugin_dump_path} -c 'ps aux --no-header' -f 1",
        "^U": f"~/.config/nishiki/plugins/textchooser -o {plugin_dump_path} -i ~/.config/nishiki/history.txt -r",
        "^Y": f"~/.config/nishiki/plugins/pyconsole c -o {plugin_dump_path} -a output",
        "^V": f"xclip -out | tr '\\n' ' ' > {plugin_dump_path}",
    }

    ################################################################################################
    # Preview settings.
    ################################################################################################

    # An ordered dictionary of preview table, where the format of the key is MIME type splitted
    # by "/". You are able to check the file MIME type by the command `file --mime <filepath>`.
    previews: list = [
        # Text file: colorize file.
        ("text/.*",          r"head -c 1024 {path} | highlight --force -O ansi"),
        ("application/x-sh", r"head -c 1024 {path} | highlight --force -O ansi"),

        # Media file: display media infomation.
        ("image/.*", r"file {path} | sed 's/, /\n/g'"),
        ("video/.*", r"file {path} | sed 's/, /\n/g'"),

        # Archive file: list archive contents.
        (".*/x-7z-compressed", r"7z l    {path} | head -c 1024"),
        (".*/zip",             r"zipinfo {path} | head -c 1024"),
    ]

    # Delimiter of the preview window.
    preview_delim: str = " \u2502 "

    ################################################################################################
    # Color settings.
    ################################################################################################

    # Definition of colors used in Nishiki.
    colors: dict = {"C0": "\x1b[30m", "C1": "\x1b[31m", "C2": "\x1b[32m", "C3": "\x1b[33m",
                    "C4": "\x1b[34m", "C5": "\x1b[35m", "C6": "\x1b[36m", "C7": "\x1b[37m",
                    "C8": "\x1b[90m", "C9": "\x1b[91m", "CA": "\x1b[92m", "CB": "\x1b[93m",
                    "CC": "\x1b[94m", "CD": "\x1b[95m", "CE": "\x1b[96m", "CF": "\x1b[97m",
                    "CX": "\x1b[0m"}

    ################################################################################################
    # File path completion color settings.
    ################################################################################################

    # Definition of file path colors used in Nishiki.
    ls_colors: dict = {"fi": "00",        # Regular file
                       "di": "01;34",     # Directory
                       "ln": "01;36",     # Symbolic link
                       "pi": "40;33",     # Pipe
                       "so": "01;35",     # Socket
                       "bd": "40;33;01",  # Block device driver
                       "cd": "40;33;01",  # Character device driver
                       "or": "40;31;01",  # Orphaned syminks
                       "ex": "01;32",     # Executable file

                       # Archive files
                       ".7z" : "01;31", ".bz2": "01;31", ".gz" : "01;31", ".tar": "01;31",
                       ".tgz": "01;31", ".taz": "01;31", ".xz" : "01;31", ".zip": "01;31",

                       # Media files.
                       ".bmp" : "01;35", ".eps" : "01;35", ".gif" : "01;35", ".jpg" : "01;35",
                       ".jpeg": "01;35", ".png" : "01;35", ".ps"  : "01;35", ".psd" : "01;35",
                       ".svg" : "01;35", ".tif" : "01;35", ".tiff": "01;35", ".xpm" : "01;35",
                       ".avi" : "01;35", ".mp4" : "01;35", ".mpg" : "01;35", ".wmv" : "01;35"}


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
