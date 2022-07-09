"""
This `utils` module provides utility functions used in Nishiki.
"""

# Import standard libraries.
import os, re, stat, subprocess, unicodedata


def strwidth(text):
    """
    Compute width of the given string (not bytes).

    Args:
        text (str): Input string.

    Returns:
        int: Total width of `text`.
    """
    # Define a function to remove ANSI escape sequence.
    strip_ansi = lambda s: re.sub("\x1b\\[[^m]*m", "", s)

    # Define a function to compute width of each character.
    get_width = lambda c: 2 if unicodedata.east_asian_width(c) in "FWA" else 1

    # Apply `get_width` for each character and sum up them.
    return sum(map(get_width, strip_ansi(text)))


def runcmd(command, split=False, cache=None):
    """
    Call command and returns an output of the command.

    Args:
        command (str) : Command to be called.
        split   (bool): Split to lines if True.
        cache   (dict): Cache of this the outputs of this command.

    Returns:
        str: A command output.
    """
    # Do nothing is the command already exists in the cache.
    if cache is not None and command.strip() in cache: return cache[command.strip()]

    # Call command and get output.
    try   : output = subprocess.check_output(command.strip(), shell=True, stderr=subprocess.STDOUT).decode().strip()
    except: output = ""

    # Split to lines if `split` is `True`.
    if split: output = [s.strip() for s in output.split("\n") if s.strip()]

    # Memorize the output.
    if cache is not None: cache[command.strip()] = output

    return output


def get_git_status(color1 = "\x1b[32m", color2 = "\x1b[33m"):
    """
    Returns git status string which is decolated by icon and ANSI color sequence.

    Args:
        color1 (str): ANSI color escape sequence used if git branch is clean.
        color2 (str): ANSI color escape sequence used if git branch has an update.

    Returns:
        str: Current Git status string with ANSI color and icon.
    """
    # Get git branch and status information.
    branch = runcmd("git rev-parse --abbrev-ref HEAD 2> /dev/null")
    status = runcmd("git status --porcelain 2> /dev/null")

    if   branch and status: return (color2 + branch + "!\x1b[0m")
    elif branch           : return (color1 + branch + " \x1b[0m")
    else                  : return (         branch + " \x1b[0m")


def get_color(path, ls_colors={}):
    """
    File path colorization.

    Args:
        path (pathlib.Path): A `pathlib.Path` instance to be colorized.

    Returns:
        str: ANSI escape sequence of the file colorization.
    """
    # Get file stat.
    mode = path.stat(follow_symlinks=False).st_mode

    # Returns color string (ANSI escape sequence).
    if   stat.S_ISBLK(mode)  != 0: return "\x1b["  + ls_colors.get("bd", "0") + "m"
    elif stat.S_ISCHR(mode)  != 0: return "\x1b["  + ls_colors.get("cd", "0") + "m"
    elif stat.S_ISFIFO(mode) != 0: return "\x1b["  + ls_colors.get("pi", "0") + "m"
    elif stat.S_ISSOCK(mode) != 0: return "\x1b["  + ls_colors.get("so", "0") + "m"
    elif stat.S_ISLNK(mode)  != 0: return "\x1b["  + ls_colors.get("ln", "0") + "m"
    elif stat.S_ISDIR(mode)  != 0: return "\x1b["  + ls_colors.get("di", "0") + "m"
    elif mode & stat.S_IXUSR != 0: return "\x1b["  + ls_colors.get("ex", "0") + "m"
    elif stat.S_ISREG(mode)  != 0: return "\x1b["  + ls_colors.get("fi", "0") + "m"
    else                         : return ""


def colorize(text, syntax="sh", enc="utf-8"):
    """
    Colorize the given text.

    Args:
        text   (str) : Input text.
        syntax (str) : Colorization syntax.
        enc    (str) : Text encoding.

    Returns:
        str: Command output.
    """
    # Text colorization.
    return subprocess.check_output(["highlight", "-q", "-S", syntax, "-O", "ansi"],
                                   input=str(text).encode(enc)).decode(enc).strip("\n")


def get_common_substr(strings):
    """
    Returns common sub-string of the given strings.

    Args:
        strings (list): A list of strings.

    Returns:
        str: Common sub-string of the string list.
    """
    if len(strings) == 0: return ""

    min_length = min(len(s) for s in strings)

    for n in range(min_length):
        if len(set(s[n] for s in strings)) != 1:
            return strings[0][:n]

    return strings[0][:min_length]


def skip_ansi_escape_sequence(ctext, start=0):
    """
    Skip ANSI escape sequence.

    Args:
        ctext (str): Colored text which contains ANSI escape sequence.
        start (int): Start index of `ctext`.

    Returns:
        int: Index of the next o the start index while skipping ANSI escape sequence.
    """
    while start < len(ctext) and ctext[start] == "\x1b":
        while not ctext[start].isalpha():
            start += 1
        start += 1
    return min(start, len(ctext))


def split_colorized_text(ctext, len_left, start=0):
    """
    Split colorize input by the given length.

    Args:
        ctext    (str): Colored text to be split.
        len_left (int): Length of the splitting.
        start    (int): Start index.

    Returns:
        (str, str): Splitted strings.
    """
    # Find actual starting position.
    start = skip_ansi_escape_sequence(ctext, start)

    # Move the current position to right while `len_left` is positive.
    while len_left > 0 and start < len(ctext):
        len_left, start = (len_left - 1, start + 1)
        start = skip_ansi_escape_sequence(ctext, start)

    return (ctext[:start], ctext[start:])


def strtrim(text, length):
    """
    Trim string with the specified length.

    Args:
        text   (str): A string to be trimmed.
        length (int): Trim length.

    Returns:
        str: Trimmed string.
    """
    return split_colorized_text(text, length, start=0)[0]


def strchunk(text, length):
    """
    Returns chunked string.

    Args:
        text   (str): A string to be chunked.
        length (int): Length of the chunk.

    Returns:
        list: A list of chunked string.
    """
    result = list()

    while len(text) > 0:

        # Split `text`, and set the rest of string to `text` again.
        substr, text = split_colorized_text(text, length, start=0)

        # Append the splitted text to `result` if valid.
        if substr: result.append(substr)

    return result


def column(strs, width, height, delim="   "):
    """
    Align the given string in a column format like the column command on Linux.

    Args:
        strs   (list): A list of strings to be aligned.
        width  (int) : Maximum width of the alignment.
        height (int) : Maximum height og the alignment.
        delim  (str) : Delimiter of the alignment.

    Returns:
        list: A list of strings where a string corresponds to a line.
    """
    def is_valid_shape(lengths, width, height, len_delim, pos=0):
        """
        Returns `True` if the given height for the alignment is valid
        (i.e. width of the alignment does not exceed the maximum width).

        Args:
            lengths   (list): A list of string lengths.
            width     (int) : Maximum width of the alignment.
            height    (int) : Height of the strings alignment.
            len_delim (int) : Length of the separator of the alignment.
        """
        # Compute the length of each lines.
        for col in range(len(lengths) // height + (0 if len(lengths) % height == 0 else 1)):
            pos += max(lengths[col*height:(col+1)*height]) + (0 if col == 0 else len_delim)

        # Returns True if the length of lines is smaller than screen width.
        return (pos < width)

    def find_optimal_height(strs, width, height, delim):
        """
        Returns optimal height of the alignment where the optimal height is less than
        the maximum height and the corresponding width is less than the maximum width.

        Args:
            strs   (list): A list of strings to be aligned.
            width  (int) : Maximum width of the alignment.
            height (int) : Maximum height of the alignment.
            delim  (int) : Delimiter string of the alignment.
        """
        lengths = list(map(strwidth, strs))

        for h in range(1, height):
            if is_valid_shape(lengths, width, h, len(delim)):
                return h

        return height

    # Update height.
    height = find_optimal_height(strs, width, height, delim)

    # Initialize result (lines, a list of strings).
    lines = [""] * height

    # Initialize column position.
    c_pos = 0

    # Create result lines by adding strings while the length of each lines is smaller than width.
    # Note: The value `width // (len(delim) + 1)` means a enough large colunm number.
    for col in range(width // (len(delim) + 1)):

        # Exit if no string is remained.
        if len(strs) <= height*col: return lines

        # Compute the right position.
        c_pos += (0 if col == 0 else len(delim)) + max(map(strwidth, strs[height*col:height*col+height]))

        # Exit if line length exceeds the screen width.
        if c_pos >= width: return lines

        # Create lines.
        for row in range(min(height, len(strs) - height*col)):
            lines[row] += ("" if col == 0 else delim) + strs[height*col + row]
            lines[row] += " " * max(0, c_pos - strwidth(lines[row]))


def parse_command_options(command, cache=dict()):
    """
    Call command with `--help` option, parse the help messages,
    and returns a list of options and their descriptions.

    Args:
        command (str) : Command to be called with `--help` option.
        cache   (dict): Cache of this the outputs of this command.

    Returns:
        options (dict): A dictionary which maps from option to description.
    """
    # Do nothing if the given command exists in the cache.
    if command in cache: return cache[command]
    else               : cache[command] = dict()

    # Call `command --help` and parse command options.
    for line in runcmd("%s --help" % command.strip(), split=True):
        for opt in line.strip().split():
            if opt.strip(" ,").startswith("-") and line.strip().startswith("-"):
                cache[command][opt.split("=")[0].strip()] = line.strip()

    return cache[command]


def filter_startswith(word, targets):
    """
    Filter strings that matched (in terms of `startswith`) with the given `word`.

    Args:
        word    (str) : Matching word.
        targets (list): A list of strings to be matched.

    Returns:
        (list): A list of strings that start with the given `word`.
    """
    return filter(lambda s: str(s).startswith(word), targets)


def sort_path(list_paths, group_directories_first=True):
    """
    Returns sorted list of file paths.

    Args:
        list_paths              (list): A list of pathlib.Path instances.
        group_directories_first (bool): True if prioritize directory.

    Returns:
        list_paths_sorted (list): Sorted path list.
    """
    def sort_key(path):
        """
        Sorting key for group-directories-first sorting.

        Args:
            path (pathlib.Path): Path instance.

        Returns:
            path_str (str): String instance to be sorted.
        """
        return ("d_" if path.is_dir() else "f_")  + str(path)

    if group_directories_first: return sorted(list_paths, key=sort_key)
    else                      : return sorted(list_paths)


def deduplicate(list_strings):
    """
    Remove duplication from the given list while keeping the order of the list element.

    Args:
        list_strings (list or generator): List to be deduplicated.
    """
    deduplicated = []

    for s in list_strings:
        if s not in deduplicated:
            deduplicated.append(s)

    return deduplicated


def getcwd():
    """
    Returns current working directory with path normalizing.

    Returns:
        str: Normalized current working directory path.
    """
    return os.getcwd().replace(os.path.expanduser("~"), "~")


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
