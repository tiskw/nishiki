"""
This `readcmd` module provides the class `ReadCmd` which can start user input session
and returns user input as a string, like the GNU Readline Library [1].

Reference:
    [1] GNU Readline Library <https://tiswww.case.edu/php/chet/readline/rltop.html>
"""

# Import standard libraries.
import getpass, os, pathlib, re, subprocess, sys, tty, termios

# Import original modules.
import editor, helper, writer, utils


class HistoryManager:
    """
    This class manage command history.
    """
    def __init__(self, cfg):
        self.cfg  = cfg
        self.path = pathlib.Path(self.cfg.history_path)
        self.read()

    def read(self):
        """
        Read history file.
        """
        self.data = self.path.read_text().strip().split("\n") if self.path.exists() else []
        self.data = utils.deduplicate(reversed(self.data[-self.cfg.history_size:]))

    def append(self, text):
        """
        Append history.

        Args:
            text (str): A text to be appended.
        """
        # Do nothing if the text starts with whitespace, or the text matches with the deny list.
        if len(text) == 0 or text.startswith(" ") or any(re.match(pattern, text) for pattern in self.cfg.history_deny_list):
            return

        # Append to history list.
        self.data = utils.deduplicate([text.strip()] + self.data)

        # Create output directory if not exists.
        self.path.parent.mkdir(exist_ok=True)

        # Append to the history file.
        with self.path.open("at") as ofp:
            ofp.write("\n" + text.strip())

    def strip(self):
        """
        Read history file, strip it, and save it again.
        """
        # Read history file.
        self.read()

        # Create history file contents.
        text = "\n".join(reversed(self.data))

        # Write to the history file.
        self.path.write_text(text)

    def get_histhint(self, lhs, rhs):
        """
        Returns history hint string.

        Args:
            lhs (str): Left-hand-side of the user input.
            rhs (str): Right-hand-side of the user input.
        """
        # Do nothing if `rhs` is not empty.
        if lhs and not rhs:
            for hist in self.data:
                if hist.startswith(lhs):
                    return self.cfg.history_hint_color + hist[len(lhs):] + "\x1b[0m"
        return ""


class ReadCmd:
    """
    This class provides a functions like GNU Readline Library [1],
    but easy to customize by the config file and external software.

    Reference:
        [1] GNU Readline Library <https://tiswww.case.edu/php/chet/readline/rltop.html>
    """

    # A string used as a separator of keybind string.
    KEYBIND_SEPARATOR: str = "::KB::"

    def __init__(self, cfg):
        self.config = cfg
        self.editor = editor.ViEditor(cfg)           # Editor instance.
        self.writer = writer.Writer(cfg)             # Writer instance.
        self.helper = helper.Helper(cfg)             # Helper instance.
        self.fileno = sys.stdin.fileno()             # File number of output stream.
        self.tacopy = termios.tcgetattr(self.fileno) # Copy of terminal attribute.
        self.histmn = HistoryManager(cfg)            # HistoryManager instance.

    def __enter__(self):
        """
        This function will be called when entering `with` sentence.
        """
        # Start cbreak mode.
        tty.setcbreak(self.fileno)

    def __exit__(self ,type, value, traceback):
        """
        This function will be called when exiting `with` sentence.
        """
        # Resotre terminal attribute (i.e. exiting cbreak mode).
        termios.tcsetattr(self.fileno, termios.TCSADRAIN, self.tacopy)

    def __input_mainloop__(self, ret, cache):
        """
        Main loop of the interactive session.

        Args:
            ret   (str) : User input.
            cache (dict): Cache variables used in the prompts.

        Notes:
            The variable `ret` is `None` if user is still typing something.
        """
        # Complete user input.
        self.helper.preproc(self.editor.lhs, self.editor.rhs)

        # Format prompt strings.
        H1 = self.config.H1.format(**cache, mode=self.editor.mode)
        H2 = self.config.H2.format(**cache, mode=self.editor.mode)
        P1 = self.config.P1.format(**cache, mode=self.editor.mode)
        P2 = self.config.P2.format(**cache, mode=self.editor.mode)

        # Colorize user input.
        clhs, crhs = utils.split_colorized_text(utils.colorize(self.editor.lhs + self.editor.rhs), len(self.editor.lhs))

        # Get colorized history hint.
        chint = self.histmn.get_histhint(self.editor.lhs, self.editor.rhs)

        # Write prompts.
        self.writer.write(H1, H2, P1, P2, clhs, crhs, self.helper.lines, chint)

        # Get user input.
        self.editor.input()

        # Handle used inputs.

        # If the last character is Ctrl-D, or Enter key, then set the user input text to `ret`.
        if   self.editor.char == "^D": ret = "^D"
        elif self.editor.char == "^J": ret = (self.editor.lhs + self.editor.rhs).rstrip()

        # If the last character is Ctrl-C, then clear user input.
        elif self.editor.char == "^C": self.editor.lhs, self.editor.rhs = ("", "")

        # If the last character is TAB, then execute completion.
        elif self.editor.char == "^I": self.editor.lhs = self.helper.result

        # If the last character is Ctrl-N, then execute history hint completion.
        elif self.editor.char == "^N": self.editor.lhs += re.sub("\x1b\\[[^m]*m", "", chint)

        # If the last character is registered as keybind, 
        elif self.editor.char in self.config.keybinds:
            ret = self.KEYBIND_SEPARATOR.join([self.editor.char, self.editor.lhs, self.editor.rhs])

        return ret

    def input(self, lhs="", rhs=""):
        """
        Starts user interactive session and returns user input text.

        Args:
            lhs (str): Left-hand-side of the user input.
            rhs (str): Right-hand-side of the user input.
        """
        with self, self.editor, self.writer, self.helper:

            # Initialize left and right hand side of the user input.
            self.editor.lhs, self.editor.rhs = lhs, rhs

            # Synchronize editor history data.
            self.editor.set_history(self.histmn.data)

            # Forcibly changed to insert mode.
            self.editor.mode = "I"

            # Initialize returned value.
            ret, cache = None, {"cwd" : utils.getcwd(), "git" : utils.get_git_status(),
                                "host": os.uname()[1],  "user": getpass.getuser()}

            # Main loop of user input.
            while ret is None: ret = self.__input_mainloop__(ret, cache)

        # Manage keybinds.
        if self.KEYBIND_SEPARATOR in ret:

            # Restore input character, left-hand-side, and right-hand-side.
            ch, lhs, rhs = ret.split(self.KEYBIND_SEPARATOR, maxsplit=2)

            # Create a directory for the temporary file if not exists.
            os.makedirs(os.path.dirname(self.config.plugin_dump_path), exist_ok=True)

            # Execute external command.
            subprocess.run(self.config.keybinds[ch], shell=True)

            # Read command result.
            if os.path.exists(self.config.plugin_dump_path):
                lhs += pathlib.Path(self.config.plugin_dump_path).read_text().strip("\n")

            return self.input(lhs, rhs)

        # Append to the history.
        self.histmn.append(ret)

        # Apply aliases.
        tokens = ret.split(maxsplit=1)
        if len(tokens) > 0 and tokens[0] in self.config.aliases:
            ret = self.config.aliases[tokens[0]] + ret[len(tokens[0]):]

        return ret


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
