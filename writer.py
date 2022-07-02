"""
This `wirte` module provides writer class which provides drawing functins to terminal window.
"""

# Import standard libraries.
import os, sys

# Import original modules.
import utils


class Writer:
    """
    A class to write headers, prompts, user inputs and completion lines to a terminal window.
    """
    def __init__(self, cfg):
        self.cfg = cfg

    def __enter__(self):
        """
        This function will be called when entering `with` sentence.
        """
        # Update writing area size.
        self.size = (os.get_terminal_size()[0], self.cfg.height)

        # Fill writing area with "\n".
        # Note that the cursor will be moved to the bottom of the writing area.
        # It is okey because the `write` function starts from moving the cursor
        # to the top of writing area.
        sys.stdout.write("\n" * (self.cfg.height - 1))

    def __exit__(self ,type, value, traceback):
        """
        This function will be called when exiting `with` sentence.
        """
        sys.stdout.write(f"\x1b[{self.cfg.height-1}F\x1b[0J")

    def write(self, H1, H2, P1, P2, clhs, crhs, clines, hint):
        """
        Write prompt, user input, and completions to terminal.

        Args:
            H1     (str) : Header 1.
            H2     (str) : Header 2.
            P1     (str) : Prompt 1.
            P2     (str) : Prompt 2.
            clhs   (str) : Colored left-hand-side of the user input.
            crhs   (str) : Colored right-hand-side of the user input.
            clines (list): List of string that describes completion lines.
            hint   (str) : History hint.
        """
        # Prepare writing strings.
        lines  = self.get_header(H1, H2, *self.size)
        lines += self.get_userinput(P1, P2, clhs, crhs + hint, *self.size)
        lines += self.get_completions(clines, *self.size)

        # Write strings to STDOUT. Note that the cursor will be moved to the top of writing area
        # because the cursor is at the bottom of the writing area.
        sys.stdout.write(f"\x1b[{self.size[1]-1}F" + "\x1b[0K\n".join(lines[:self.size[1]]) + "\x1b[0K")
        sys.stdout.flush()

    @staticmethod
    def get_header(H1, H2, width, height):
        """
        Returns header line of the prompt.

        Args:
            H1     (str): Header 1.
            H2     (str): Header 2.
            width  (int): Width of the writing area.
            height (int): Height of the writing area.

        Returns:
            str: Header line of the prompt.
        """
        whitespaces = " " * (width - utils.strwidth(H1) - utils.strwidth(H2))
        return (H1 + whitespaces + H2).split("\n")[:1]

    @staticmethod
    def get_userinput(P1, P2, clhs, crhs, width, height):
        """
        Returns user input line with prompts.

        Args:
            P1     (str): Prompt 1.
            P2     (str): Prompt 2.
            clhs   (str): Colored left-hand-side of the user input.
            crhs   (str): Colored right-hand-side of the user input.
            width  (int): Width of the writing area.
            height (int): Height of the writing area.

        Returns:
            str: User input line of the prompt.
        """
        # Add ANSI escape sequence for highlighting cursor position.
        index = utils.skip_ansi_escape_sequence(crhs)
        crhs  = crhs[:index] + "\x1b[7m" + (crhs[index:index+1] if len(crhs) > index else " ") + "\x1b[27m" + crhs[index+1:]

        # Returns lines of user input.
        return utils.strchunk(P1 + clhs + crhs, width)[:max(0, height)]

    @staticmethod
    def get_completions(clines, width, height):
        """
        Returns completion lines.

        Args:
            clines (list): Full completion lines.
            width  (int) : Width of the writing area.
            height (int) : Height of the writing area.

        Returns:
            list: List of string for completion lines.
        """
        return [utils.strtrim(line, width) for line in clines[:max(0, height)]]


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
