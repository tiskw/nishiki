"""
This `helper` module provides `Helper` class which has a function to complete user input.
"""

# Import standard libraries.
import mimetypes, re, os, pathlib

# Import original modules.
import utils


class Helper:
    """
    A class for supporting user input.

    Example:
    >> helper = Helper()
    >>
    >> # Compute completion.
    >> helper.preproc("ls", " main")
    >>
    >> # Print completion result and completion lines (used in the completion display area).
    >> print(helper.result)
    >> print(helper.lines)
    """
    # A list of preprocessing function types.
    FUNCTYPES = ["command", "option", "path", "preview", "shell", "subcmd"]

    def __init__(self, cfg):
        self.cfg   = cfg
        self.cache = {ft:dict() for ft in self.FUNCTYPES + [None]}

    def __enter__(self):
        """
        This function will be called when entering `with` sentence.
        """
        # Update window size.
        self.size = (os.get_terminal_size()[0], self.cfg.height)

        # Erase caches.
        self.cache = {ft:dict() for ft in self.FUNCTYPES + [None]}

    def __exit__(self ,type, value, traceback):
        """
        This function will be called when exiting `with` sentence.
        """
        pass

    def preproc(self, lhs, rhs):
        """
        Preprocessing for completion. Preprocessing results are stored as a member variables.

        Args:
            lhs (str): Left hand side of the user input.
            rhs (str): Right hand side of the user input.

        Returns:
            This function returns no value, but newly creates the following member variables:

            * self.result (str) : User input after completion (i.e. completion result).
            * self.lines  (list): Description lines shown at the bottom of editing line.

        Note:
            This function does not update `lhs` and `rhs`, because the actual completion
            will be conducted only when the member function `self.complete` will be called
            (which is usually triggered by the TAB key).
        """
        def get_matched_preproc_func(tokens):
            for patterns, ftype, option in self.cfg.completions:
                if match(patterns, tokens) and ftype in self.FUNCTYPES:
                    return (ftype, option, eval("preproc_%s" % ftype))
            return (None, None, preproc_dummy)

        # Parse user input and get the target argument.
        tokens = [token for token in lhs.split() if token] + ([""] if lhs.endswith(" ") else [])
        target = tokens[-1] if len(tokens) > 0 else ""

        # Find matched preproc function and it's attributes (i.e. function type and option).
        # If the `tokens` is empty list, then `token` will be replaced to `[""]`.
        ftype, option, cfunc = get_matched_preproc_func(tokens if tokens else [""])

        # Run the preproc function.
        self.result, self.lines = cfunc(target, tokens, lhs, option, *self.size, self.cache[ftype], self.cfg)


def match(patterns, tokens, index_token=0):
    """
    Return True if the given tokens matched with the given patterns. The arguments `tokens`
    is a list of strings, and the argument `patterns` are list of regular expression
    strings with the following extra special commands:

    * "FILE": existing file path
    * ">>"  : skip tokens (available only 1 time in one patterns)

    Args:
        patterns (list): List of regular expression strings.
        tokens   (list): List of strings to be matched.

    Returns:
        matched (bool): True is the given tokens matched with the given patterns.
    """
    for index_pattern, pattern in enumerate(patterns):

        # Select target argument.
        if index_token < len(tokens): token = tokens[index_token]
        else                        : return False

        # Case 1: pattern ">>"
        if pattern == ">>":
            index_token = len(tokens) - len(patterns) + index_pattern

        # Case 2: pattern "FILE"
        elif pattern == "FILE":
            if not os.path.exists(token):
                return False

        # Case 3: others
        elif not re.match(pattern, token):
            return False

        index_token += 1

    # No unprocessed tokens remains if the token matches with the pattern.
    return (index_token == len(tokens))


def preproc_command(target, tokens, lhs, option, width, height, cache, cfg):
    """
    Preprocessing for the command names.

    Args:
        target (str) : Target user input token.
        tokens (list): Tokens of user input.
        lhs    (str) : Left hand side of the user input.
        option (str) : Optional argument. This variable is not used in this function.
        width  (int) : Width of screen.
        height (int) : Height of screen.
        cache  (dict): Cache of shell commands.

    Returns:
        result (str) : Completion result.
        lines  (list): List of completion lines.
    """
    cmds  = utils.runcmd(r"echo $PATH | sed 's/:/\n/g' | sed '/^$/d' | xargs ls -1 2>/dev/null | sort | uniq", True, cache)
    cands = {cmd:cmd for cmd in cmds if cmd.startswith(target)}
    return preproc_from_candidates(cands, target, lhs, width, height)


def preproc_dummy(target, tokens, lhs, option, width, height, cache, cfg):
    """
    Dummy preprocessing function. This function does nothing and returns empty results.

    Args:
        target (str) : Target user input token.
        tokens (list): Tokens of user input.
        lhs    (str) : Left hand side of the user input.
        option (str) : Optional argument. This variable is not used in this function.
        width  (int) : Width of screen.
        height (int) : Height of screen.
        cache  (dict): Cache of shell commands.

    Returns:
        result (str) : Completion result.
        lines  (list): List of completion lines.
    """
    # Returns dummy `result` and `lines`.
    return (lhs, [""] * height)


def preproc_option(target, tokens, lhs, option, width, height, cache, cfg):
    """
    Preprocessing for the command options.

    Args:
        target (str) : Target user input token.
        tokens (list): Tokens of user input.
        lhs    (str) : Left hand side of the user input.
        option (str) : Optional argument. This variable is not used in this function.
        width  (int) : Width of screen.
        height (int) : Height of screen.
        cache  (dict): Cache of shell commands.

    Returns:
        result (str) : Completion result.
        lines  (list): List of completion lines.
    """
    opts  = utils.parse_command_options(tokens[0], cache)
    cands = {opt:dsc for opt, dsc in opts.items() if opt.startswith(target)}
    return preproc_from_candidates(cands, target, lhs, width, height)


def preproc_path(target, tokens, lhs, option, width, height, cache, cfg):
    """
    Preprocessing for the file path completion.

    Args:
        target (str) : Target user input token.
        tokens (list): Tokens of user input.
        lhs    (str) : Left hand side of the user input.
        option (str) : Optional argument. This variable is not used in this function.
        width  (int) : Width of screen.
        height (int) : Height of screen.
        cache  (dict): Cache of shell commands.

    Returns:
        result (str) : Completion result.
        lines  (list): List of completion lines.
    """
    def colorize_name(name, path):
        """
        Colorize file name.

        Args:
            name (str): File name.
            path (str): File path which is used for getting file attributes.

        Returns:
            colorized_name (str): File name clorized by ANSI escape sequence.
        """
        return utils.get_color(path, cfg.ls_colors) + str(name) + "\x1b[0m"

    # Split the target token to directory path and file name while expandnig tilde `~` to `$HOME`.
    # NOTE: The `target_dir` will be an empty strnig if the target directory is the current directory.
    if target.endswith("/"): target_dir, target_name = (os.path.expanduser(target), "")
    else                   : target_dir, target_name = os.path.split(os.path.expanduser(target))

    # Returns empty candidates if the `dirpath` does not exists.
    if not os.path.exists(target_dir if target_dir else "."):
        return preproc_from_candidates({}, target, lhs, width, height)

    # If the target filename is a hidden file, then show all files including hidden files.
    show_dot = target_name.startswith(".")

    # Get a list of files in the directory and create candidates dictionary.
    names = sorted([name for name in os.listdir(target_dir if target_dir else ".") if name.startswith(target_name)])
    paths = [name if target_dir == "./" else os.path.join(target_dir, name) for name in names[:cfg.max_comp_items]]
    cands = {path:colorize_name(name, path) for name, path in zip(names, paths) if show_dot or not name.startswith(".")}

    # Computes `result` and `lines` from candidate dictionary and returns them.
    return preproc_from_candidates(cands, target, lhs, width, height)


def preproc_preview(target, tokens, lhs, option, width, height, cache, cfg):
    """
    Completion preprocessing for the file preview.

    Args:
        target (str) : Target user input token.
        tokens (list): Tokens of user input.
        lhs    (str) : Left hand side of the user input.
        option (str) : Optional argument. This variable is not used in this function.
        width  (int) : Width of screen.
        height (int) : Height of screen.
        cache  (dict): Cache of shell commands.

    Returns:
        result (str) : Completion result.
        lines  (list): List of completion lines.
    """
    def get_preview_contents(path, previews):
        """
        Returns preview contents.

        Args:
            path     (pathlib.Path): Path to the preview target file.
            previews (list)        : List of tuples (mimetype_pattern, command).

        Returns:
            str: Contents of preview.
        """
        # Estimate mimetype of the target file.
        mimetype, _ = mimetypes.guess_type(str(path), strict=False)

        # Set the `mimetype` to "text/plain" if None.
        if mimetype is None: mimetype = "text/plain"

        # Search the mathed mimetype and run the command.
        for mimetype_pattern, command in cfg.previews:
            if re.match(mimetype_pattern, mimetype):
                return utils.runcmd(command.format(path=str(path))).split("\n")

    # Run the file path completion preprocessing.
    result, lines = preproc_path(target, tokens, lhs, option, width, height, cache, cfg)

    # Create path instance of the target file.
    path = pathlib.Path(tokens[-2] if len(tokens) > 1 else "").resolve()

    # Do nothing if the target file not exists.
    if not path.is_file():
        return (result, lines)

    # Compute width of the left-side of the preview window.
    w = width - int(width * cfg.preview_window_ratio) - len(cfg.preview_delim)

    # Get the contents of the target file.
    if not str(path) in cache:
        cache[str(path)] = get_preview_contents(path, cfg.previews)

    # Do nothing if preview contents is None (i.e. preview command is not registered).
    if cache[str(path)] is None:
        return (result, lines)

    # Compute preview lines.
    preview_lines = cache[str(path)] + [""] * height

    # Compute completion lines.
    lines = [utils.strtrim(utils.strtrim(line + " " * w, w) + cfg.preview_delim + preview_line, width) + "\x1b[0m"
             for line, preview_line in zip(lines, preview_lines)]

    return (result, lines)


def preproc_shell(target, tokens, lhs, option, width, height, cache, cfg):
    """
    Preprocessing for the completion using shell command.

    Args:
        target (str) : Target user input token.
        tokens (list): Tokens of user input.
        lhs    (str) : Left hand side of the user input.
        option (str) : Optional argument. This variable is a shell command to be called.
        width  (int) : Width of screen.
        height (int) : Height of screen.
        cache  (dict): Cache of shell commands.

    Returns:
        result (str) : Completion result.
        lines  (list): List of completion lines.
    """
    # Run the given command, filter matched lines, and create candidate dictionary.
    lines = utils.filter_startswith(target, utils.runcmd(option, split=True, cache=cache))
    cands = {s.split()[0]:s for s in lines[:cfg.max_comp_items] if s}

    # Computes `result` and `lines` from candidate dictionary and returns them.
    return preproc_from_candidates(cands, target, lhs, width, height)


def preproc_subcmd(target, tokens, lhs, option, width, height, cache, cfg):
    """
    Preprocessing for the completion for sub command.

    Args:
        target (str) : Target user input token.
        tokens (list): Tokens of user input.
        lhs    (str) : Left hand side of the user input.
        option (str) : Optional argument. This variable is a shell command to be called.
        width  (int) : Width of screen.
        height (int) : Height of screen.
        cache  (dict): Cache of shell commands.

    Returns:
        result (str) : Completion result.
        lines  (list): List of completion lines.
    """
    def get_display_string(text):
        """
        Returns display string for command substring.

        Args:
            text (str): A sub-command string.
        """
        # Do nothing if no whitespace in the given string.
        if " " not in text:
            return text

        # Split sub-command string to command and description.
        token1, token2 = text.split(maxsplit=1)

        # Create separator.
        separator = " " + "." * (text.find(token2) - len(token1) - 1) + " "

        # Returns modified sub-command string.
        return token1 + separator + token2

    # Run the given command, filter matched lines, and create candidate dictionary.
    lines = utils.filter_startswith(target, utils.runcmd(option, split=True, cache=cache))
    cands = {s.split()[0]:get_display_string(s) for s in lines[:cfg.max_comp_items] if s}

    # Computes `result` and `lines` from candidate dictionary and returns them.
    return preproc_from_candidates(cands, target, lhs, width, height)


def preproc_from_candidates(cands, target, lhs, width, height):
    """
    Returns completion result and completion lines from a dictionary of candidates.

    Args:
        cands  (dict): Dictionary of format {completion:description}.
        target (str) : Target user input token.
        lhs    (str) : Left hand side of the user input.
        width  (int) : Width of screen.
        height (int) : Height of screen.

    Returns:
        result (str) : Completion result.
        lines  (list): List of completion lines.
    """
    # If candidate exists, returns results and lines.
    if len(cands) > 0:

        # Compute completion lines.
        lines = (utils.column(list(cands.values()), width, height) + [""] * height)[:height]

        # Compute completion result.
        result = lhs[:-len(target)] + utils.get_common_substr(list(cands.keys()))

        # Add "/" or " " if the current token is directory or unique.
        token = result.split()[-1] if result else ""
        if os.path.isdir(os.path.expanduser(token)): result = result.rstrip("/") + "/"
        elif len(cands) == 1                       : result = result.rstrip(" ") + " "

        # Repalce `$HOME` to `~`.
        result = result.replace(os.environ["HOME"], "~")

        return (result, lines)

    # Returns empty results if the candidate is empty.
    else: return (lhs, [""] * height)


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
