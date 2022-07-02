"""
This `editor` module provides editor classes which support user command editing.
"""

# Import standard libraries.
import curses.ascii, sys


class EditorBase:
    """
    A base class for command line editor.
    This class is assumed to be inherited.
    """
    def __init__(self, cfg):
        self.char = None  # Last input character.

    def __enter__(self):
        """
        This function will be called when entering `with` sentence.
        """
        sys.stdout.write("\x1b[?25l")
        self.lhs = ""
        self.rhs = ""
        self.idx = 0

    def __exit__(self ,type, value, traceback):
        """
        This function will be called when exiting `with` sentence.
        """
        sys.stdout.write("\x1b[?25h")

    def input(self):
        """
        Get user input and edit text.

        Updates:
            self.char: User input character.
        """
        # Initialize user input string.
        self.char = ""

        # Read valid UTF-8 and returns it, but returns "^C" if Ctrl-C typed by user.
        try:

            # Read valid UTF-8 character.
            while True:

                # Read user input only 1 byte.
                self.char += sys.stdin.read(1)

                # Exit loop if the read string is a valid UTF-8.
                try    : self.char.encode("utf-8")
                except : continue
                finally: break

            # Convert read string to a printable form.
            if ord(self.char) < 256:
                self.char = curses.ascii.unctrl(self.char)

        except KeyboardInterrupt: self.char = "^C"


class ViEditor(EditorBase):
    """
    A class for command line editor with vi keybinds.
    """
    def __init__(self, cfg):
        super().__init__(cfg)
        self.mode = "I"

    def input(self):
        """
        Get user input and edit text.

        Updates:
            self.char: User input character.
            self.mode: Updates editing mode.
        """
        # Run `input` function of the base class that creates `self.char`.
        super().input()

        # Manipulate user input.
        if   self.char not in {"^C", "^J"} and self.mode == "I": self.edit_insert(self.char)
        elif self.char not in {"^C", "^J"} and self.mode == "N": self.edit_normal(self.char)

    def edit_insert(self, c):
        """
        Insert mode.

        Args:
            c (str): User input character.

        Updates:
            self.lhs : Left hand side of the user input text.
            self.rhs : Right hand side of the user input text.
            self.mode: Updates editing mode.
        """
        # Mode transition.
        if c == "^[": self.mode = "N"

        # Text editing.
        if   c == "^H"  : self.lhs, self.rhs = (self.lhs[:-1], self.rhs)
        elif c == "^?"  : self.lhs, self.rhs = (self.lhs[:-1], self.rhs)
        elif c[0] != "^": self.lhs, self.rhs = (self.lhs + c,  self.rhs)

    def edit_normal(self, c):
        """
        Edit mode.

        Args:
            c (str): User input.

        Updates:
            self.lhs : Left hand side of the user input text.
            self.rhs : Right hand side of the user input text.
            self.mode: Updates editing mode.
        """
        # Mode transition.
        if c in {"a", "i", "A", "I"}: self.mode = "I"

        # Text editing.
        if   c == "0": self.lhs, self.rhs = ("",                       self.lhs + self.rhs)
        elif c == "h": self.lhs, self.rhs = (self.lhs[:-1],            self.lhs[-1:] + self.rhs)
        elif c == "j": self.lhs, self.rhs = self.move(delta=-1)
        elif c == "k": self.lhs, self.rhs = self.move(delta=+1)
        elif c == "l": self.lhs, self.rhs = (self.lhs + self.rhs[0:1], self.rhs[1:])
        elif c == "a": self.lhs, self.rhs = (self.lhs + self.rhs[0:1], self.rhs[1:])
        elif c == "A": self.lhs, self.rhs = (self.lhs + self.rhs,      "")
        elif c == "I": self.lhs, self.rhs = ("",                       self.lhs + self.rhs)
        elif c == "D": self.lhs, self.rhs = (self.lhs,                 "")
        elif c == "S": self.lhs, self.rhs = ("",                       "")

    def set_history(self, hist_data):
        """
        Set history.

        Args:
            hist_data (list): A list of strings that corresponds to history.

        Updates:
            self.hist: A new list of history (pairs of lhs and rhs) with blank history.
        """
        self.hist = [("", "")] + [(h, "") for h in hist_data if h]

    def move(self, delta):
        """
        Move editing text.

        Args:
            delta (int): Moving amount.

        Returns:
            str: `delta`-th previous history which is a pair of (`lhs`, `rhs`).

        Updates:
            self.idx: Updated index of the history list.
        """
        self.hist[self.idx] = (self.lhs, self.rhs)
        self.idx = max(0, min(self.idx + delta, len(self.hist) - 1))
        return self.hist[self.idx]


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
