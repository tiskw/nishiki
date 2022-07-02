#!/usr/bin/env python3
# coding: utf-8

import curses
import os
import pathlib
import subprocess

import cmd
import cui

# Key binding during the grep mode.
def keybind_grep_mode(st, char):
    if   char == "^J"           : return f"st.grepstr  = st.grepstr[1:]"
    elif char == "^H"           : return f"st.grepstr  = st.grepstr[:-1]"
    elif char == "KEY_BACKSPACE": return f"st.grepstr  = st.grepstr[:-1]"
    else                        : return f"st.grepstr += '{char}'"

# A class that manages key input in process number selection mode.
class Keybind:

    def __init__(self, config):
        self.config = config  # A config instance.

    # A function that gets only one character string from the Curses window win and returns the corresponding command.
    # Note that the return value of this function is the function itself (function variable). In other words, the function itself is not executed.
    def input(self, win, st):

        # Get one keystroke from the window.
        char = curses.keyname(win.getch()).decode()

        # Return a string which cortrespond to the keystroke.
        if st.grepstr.startswith("/"): return keybind_grep_mode(st, char)
        else                         : return self.config.commands.get(char, "cmd.dummy") + "(st)"

# A class that represents the state of the process number selection mode.
class State:

    def __init__(self, args, config):
        self.args    = args        # A command line arguments.
        self.config  = config      # A config instance.
        self.delim   = args.delim  # A delimiter character.
        self.focus   = 0           # A non-negative integer variable that represents a cursor.
        self.grepstr = ""          # A string for grep mode.
        self.items   = None        # A variable that stores process information.
        self.field   = args.field  # A index of field to be returned.
        self.select  = list()      # A dictionary which stores the selected file.
        self.result  = None        # A variable that stores the final result. This software ends when this is not None.

    # A function that returns a summary of process information (mainly used for drawing).
    def contents(self):
        return [(item, "", item in self.select, 0) for item in self.items]

    # A function that returns header information. The content of the header is described in "config.py".
    def headers(self, width = None):
        return [self.config.header_dir.format(**os.environ) + self.grepstr]

    # A function that returns the file path of the currently selected file.
    def target(self):
        if self.field >= 0: return self.items[self.focus].split()[self.field]
        else              : return self.items[self.focus].strip()

    # Read the process information.
    def update(self):
        self.items = [item.strip() for item in self.text.strip().split(self.delim) if self.grepstr.lstrip("/") in item]
        if self.args.reverse:
            self.items = reversed(self.items)

    def reload(self):
        if   self.args.text : self.text = self.args.text
        elif self.args.input: self.text = pathlib.Path(self.args.input).read_text()
        elif self.args.cmd  : self.text = subprocess.check_output(self.args.cmd, shell = True).decode()
        else                : self.text = ""

# A function that starts the curses session of process number selection mode.
def start(stdscr, args, conf):

    # Initialization of state variables.
    st, key = State(args, conf), Keybind(conf)

    # Initialization of curses.
    cui.initialize()

    # Create list contents.
    st.reload()

    while st.result is None:

        # Update the process information.
        st.update()

        # Draw to the screen.
        stdscr.erase()
        cui.draw_pgrep_window(stdscr, st.headers(), st.contents(), st.focus, conf.cmap)
        stdscr.refresh()

        # Accepts keystrokes and executes the corresponding command.
        # Note that the return value of the function "keybind.input()" is just a string.
        exec(key.input(stdscr, st))

    return " ".join(st.result).strip()

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
