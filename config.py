####################################################################################################
# Config file for NiShiKi                                                                          #
#                                                                                                  #
# NOTE: This config file is written in Python, but it's PocketPy that is a portable implementation #
#       of Python 3.x, not CPython that is the most common Python implementation. The available    #
#       standard library in PocketPy is very limited. See <https://pocketpy.dev> for more details. #
####################################################################################################

# Import standard libraries.
import datetime
import os
import re

# Import NiShiKi's custom module.
import nishiki


####################################################################################################
# General settings
####################################################################################################

# Maximum size of histories.
max_hist_size: int = 5000

# Height of drawing area.
area_height: int = 8

# Length of mergin in the column display.
column_margin: int = 3

# Prefix and postfix of the time stamp.
# For example, you can set ANSI color code to colorize the time stamp.
datetime_pre: str  = "\x1B[90m["
datetime_post: str = "]\x1b[m"

# Prefix and postfix of the history hint.
# For example, you can set ANSI color code to colorize history hint.
histhint_pre: str  = "\x1B[90m"
histhint_post: str = "\x1B[m"


####################################################################################################
# Alias settings
####################################################################################################

aliases: list = [

    # "ls" command families.
    ["ls", "ls -h --color=auto --group-directories-first"],
    ["ll", "ls -hl --color=auto --group-directories-first"],
    ["la", "ls -ahl --color=auto --group-directories-first"],
    ["sl", "ls -h --color=auto --group-directories-first"],

    # "cp", "mv", and "rm".
    ["cp", "cp -i"],
    ["mv", "mv -i"],
    ["rm", "rm -i"],

    # Others.
    ["diff", "diff --color=auto"],
    ["grep", "grep --color=auto"],
]


####################################################################################################
# Colorization settings
####################################################################################################

# Comma seperated values of shell command to be colorized.
# Default value: "cat,cd,chmod,chown,cp,echo,env,export,grep,let,ln,ls,make,..."
colorize_commands: str = None

# Comma seperated values of shell keywords to be colorized.
# Default value: "case,do,done,elif,else,esac,exit,fi,for,function,if,in,..."
colorize_keywords: str = None

# Comma seperated values of shell symbols to be colorized.
# Default value: "&,|,>,<,&&,||,>>,<<"
colorize_symbols: str = None


####################################################################################################
# Propmt settings
####################################################################################################

def get_ps0() -> str:
    """
    Returns the line before the prompt.
    """
    # Get the date and time string.
    date, time = str(datetime.datetime.now()).split()

    # Get the current directory.
    try   : cwd = os.getcwd().replace(os.getenv("HOME"), "~")
    except: cwd = "???"

    # Get the width of the terminal.
    width = nishiki.get("width")

    # Get the git branch info.
    git_info = get_git_branch_info()

    # Create the left side of the prompt string.
    prompt  = "\x1b[31m{user} \x1b[0m: ".format(user=os.getenv("USER"))
    prompt += "\x1b[32m{name} \x1b[0m: ".format(name=os.getenv("NAME"))
    prompt += "\x1b[33m{date} \x1b[0m: ".format(date=date)
    prompt += "\x1b[34m{time} \x1b[0m: ".format(time=time)
    prompt += "\x1b[35m{cwd}\x1b[0m".format(cwd=cwd)

    # Create the right side of the prompt string.
    prompt += " " * (width - strwidth(prompt) - strwidth(git_info) - 1)
    prompt += git_info

    return prompt


def get_ps1() -> str:
    """
    Returns the first prompt.
    """
    return "=>> "


def get_ps2() -> str:
    """
    Returns the second prompt.
    """
    return ".. "


####################################################################################################
# Keybind settings
#
# The format of the key binding string is:
#     "!COMMAND_TYPE COMMAND_CONTENTS"
#
# The COMMAND_TYPE must be either "int" (internal) or "ext" (external). The
# details of each command type are as follows.
#
# [command_type: int]
#   The internal command calls NiShiKi-internal functions (i.e. functions
#   embedded in NiShiKi). It has three command contents:
#     - filechooser: Choose file path graphically.
#     - histchooser: Choose command history graphically.
#     - procchooser: Choose process number graphically.
#   This sample config file sets keybinds of them to Ctrl-F, Ctrl-H, and
#   Ctrl-P respectively. See the following [KEYBIND] section.
#
# [command_type: ext]
#   The external command calls an external command, for example "xclip", and
#   inserts the command results into your editing line if you want. The external
#   command types have several specifiers, 'i', 'l' and 'r' etc. The specifiers
#   are declared like "ext[il]" which means the external command with 'i' and
#   'l' specifiers. The following is the meaning of each specifier.
#     - i: insert command results to the editing buffer.
#     - l: append left-hand side string.
#     - r: append right-hand side string.
#   Note that the simple command type "ext" is equivalent to "ext[ilr]".
#   Whitespace is necessary after the internal/external specifier.
#
# The following variables are available for external commands:
#   - {lhs}: Left-hand side string.
#   - {rhs}: Right-hand side string.
####################################################################################################

keybinds: list = [
    ["^F", "!int filechooser"],
    ["^U", "!int histchooser"],
    ["^P", "!int procchooser"],
    ["^V", "!ext xclip -o"],
    ["^L", "!ext[] clear"],
]


####################################################################################################
# Completion settings
#
# The format of the completion is:
#     [[PATTERN], COMPLETION_TYPE, EXTRA_STRING]
#
# The PATTERN is a list of strings. If user input matches to the PATTERN,
# the corresponding completion will be applied. There are several types of
# completion, "command", "filepath", "option", "preview", "shell", and "subcmd".
#
#   * "command" : Commands in $PATH will be completed.
#   * "filepath": File paths in a directory willbe completed.
#   * "option"  : Command option will be completed.
#   * "preview" : File paths command with preview window.
#   * "shell"   : Run a shell command and use it as a completion candidate.
#   * "subcmd"  : Complete sub command of current command.
####################################################################################################

completions: list = [
    # Docker command completions.
    [["docker", "exec", ">>", ".*"], "shell",  "docker container ls -a --format '{{.Names}}'"],
    [["docker", "run",  ">>", ".*"], "shell",  "docker image ls --format '{{.Repository}}:{{.Tag}}'"],
    [["docker", ".*"],               "subcmd", "docker --help | grep -E '^  [^ -]'"],

    # Git command completions.
    [["git", "branch",   ">>", ".*"], "shell",  "git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"],
    [["git", "checkout", ">>", ".*"], "shell",  "git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"],
    [["git", "merge",    ">>", ".*"], "shell",  "git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"],
    [["git", "push",     ">>", ".*"], "shell",  "git branch -a --no-color | cut -b 3- | cut -d ' ' -f 1"],
    [["git", ".*"],                   "subcmd", "git --help | grep -E '^   [^ ]'"],

    # Ssh command completions.
    [["ssh", ".*"], "shell", "cat ~/.ssh/config 2>/dev/null | grep '^Host ' 2>/dev/null | cut -b 6-"],

    # Low priority completions.
    [["[./~].*",],       "path",    ""],
    [[".+",],            "command", ""],
    [[">>", "-.*"],      "option",  ""],
    [[">>", "FILE", ""], "preview", ""],
    [[">>", ".*"],       "path",    ""],
]


####################################################################################################
# Preview settings
####################################################################################################

# An ordered list of preview table, where the format of the key is MIME type splitted
# by "/". You are able to check the file MIME type by the command `file --mime <filepath>`.
previews: list = [
    # Directory.
    ["inode/directory", "timeout 0.1s ls -hl --color=always --group-directories-first '{path}'"],

    # Text file: colorize file.
    ["text/.*",          "timeout 0.1s highlight --force -O ansi '{path}'"],
    ["application/x-sh", "timeout 0.1s highlight --force -O ansi '{path}'"],

    # Media file: display media infomation.
    ["image/.*", "file '{path}' | sed 's/, /\\n/g'"],
    ["video/.*", "file '{path}' | sed 's/, /\\n/g'"],

    # Archive file: list archive contents.
    [".*/x-7z-compressed", "timeout 0.1s 7z l    '{path}'"],
    [".*/zip",             "timeout 0.1s zipinfo '{path}'"],
]

# Delimiter of the preview window.
preview_delim: str = " │ "

# Ratio of the preview window.
preview_ratio: float = 0.45


####################################################################################################
# Utility functions (if you don't use, you can erase)
####################################################################################################

def get_git_branch_info() -> str:
    """
    Get git branch and status information and return as a colored string.
    """
    # Get git branch and status information.
    branch = nishiki.check_output("git rev-parse --abbrev-ref HEAD").strip()
    status = nishiki.check_output("git status --porcelain").strip()

    if ((len(branch) > 0) and (len(status) > 0)):
        return ("\x1b[33m" + branch + "!\x1b[m");
    elif len(branch) > 0:
        return ("\x1b[32m" + branch +  "\x1b[m");
    return branch;


def strwidth(text: str) -> int:
    """
    Compute the width of the given string.

    Args:
        text (str): Target string.

    Returns:
        (int): Width of the string.
    """
    def get_width_char(c: str) -> int:
        """
        Get the width of a unicode character.
        """
        if               ord(c) <= 0x0000FF: return 1
        elif 0xEFBDB1 <= ord(c) <= 0xEFBE9F: return 1
        else                               : return 2

    # Ignroe ANSI SGR.
    text = re.sub("\x1b\\[[0-9]*m", "", text)

    return sum(map(get_width_char, text))


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
