#!/usr/bin/env python3
# coding: utf-8
#
# This script provides state classes that represents the state of the filer/pgrep.

import curses
import os
import shlex

import cui
import cmd
import preview
import utils

# Key binding during the grep mode.
def keybind_grep_mode(st, char):
    if   char == "^J"           : return f"st.grepstr  = st.grepstr[1:]; st.refresh = True"
    elif char == "^H"           : return f"st.grepstr  = st.grepstr[:-1] if len(st.grepstr) > 1 else '/'"
    elif char == "KEY_BACKSPACE": return f"st.grepstr  = st.grepstr[:-1] if len(st.grepstr) > 1 else '/'"
    else                        : return f"st.grepstr += '{char}'"

# A class that manages keystrokes in filer mode.
# The key binding itself is written in config.py, not described in this class.
class FilerKeybind:

    def __init__(self, config):
        self.config = config  # A config instance.

    # A function that gets only one character string from the Curses window win and returns the corresponding command.
    # Note that the return value of this function is the string of function.
    # In other words, the function is not executed in here.
    def input(self, win, st):

        # Get one keystroke from the window.
        char = curses.keyname(win.getch()).decode()

        # Return a string which cortrespond to the keystroke.
        if st.grepstr.startswith("/"): return keybind_grep_mode(st, char)
        else                         : return self.config.commands.get(char, "cmd.dummy") + "(st)"

# A class that represents the state of filer mode.
class FilerState:

    def __init__(self, args, config):
        self.args    = args         # A variable that stores the command line arguments (parsed by argparse).
        self.config  = config       # A config instance.
        self.dirpath = os.getcwd()  # A path of the current directory.
        self.items   = None         # A variable to store the result of "ls" in the current directory.
        self.prevs   = None         # A variable to store the result of "ls" in the previous directory.
        self.focus   = 0            # A non-negative integer variable that represents a cursor position.
        self.result  = None         # A variable that stores the final result. This software ends when this is not None.
        self.select  = list()       # A dictionary variable that stores the selected file.
        self.showdot = False        # A boolean flag which indicates whether to show dotfiles.
        self.preview = True         # A boolean flag which indicates whether to show preview window.
        self.grepstr = ""           # A string for grep mode.
        self.refresh = False        # Reload directory if True.

    # A function that returns a summary of the filer's state (mainly used for drawing).
    def contents(self):
        contents1 = [(item["name"], "",                                  item["path"] in self.select, item["color"]) for item in self.prevs]
        contents2 = [(item["name"], self.config.fileinfo.format(**item), item["path"] in self.select, item["color"]) for item in self.items]
        contents3 = lambda win, rect: preview(win, rect, self.target(), self.config.max_preview_bytes)
        focus1    = utils.findindex(self.prevs, os.path.basename(self.dirpath))
        focus2    = self.focus
        return (contents1, contents2, contents3, focus1, focus2)

    # A function that returns the file information (dictionary type) or the value corresponding
    # to the given key of the currently selected file.
    def item(self, key = None):
        return self.items[self.focus][key] if key else self.items[self.focus]

    # A function that returns header information. The content of the header is described in "config.py".
    def headers(self, width = None):
        usr = self.config.header_dir.format(**os.environ)
        grp = "\x1b[31m/" + self.grepstr if self.grepstr else ""
        return [usr + self.dirpath + grp, self.config.header_file + self.item("name")]

    # A function that returns the absolute file path of the currently selected file.
    def target(self):
        return os.path.join(self.dirpath, self.item("name"))

    # A function that updates the results of ls for the current and previous directory.
    def update(self):
        self.focus = 0
        self.files = utils.dictls(self.dirpath, ls_opt = "A" if self.showdot else "", previous = False, cmap = self.config.cmap)
        self.prevs = utils.dictls(self.dirpath, ls_opt = "A" if self.showdot else "", previous = True,  cmap = self.config.cmap)
        self.grepstr = ""
        self.grep()

    # Apply grep filter, create `self.items` instance, and move cursor if necessary.
    def grep(self):
        # Apply grep filter and create `self.items` instance.
        grepstr = self.grepstr.lstrip("/")
        if grepstr: self.items = [item for item in self.files if self.grepstr.lstrip("/") in item["name"]]
        else      : self.items = self.files
        # Add dummy data which represent an empty directory if `self.items` is empty.
        if len(self.items) == 0:
            self.items = [{"perm": "", "nlinks": 0, "user": "", "group": "", "size": 0, "date": "",
                           "time": "", "name": "EMPTY DIREDTORY", "type": "", "color": 1, "path": ""}]
        # Move cursor if necessaty.
        self.focus = max(0, min(self.focus, len(self.items) - 1))

# A function that starts the curses session of filer mode.
def start(stdscr, args, conf):

    # Initialization of state variables.
    st, key = FilerState(args, conf), FilerKeybind(conf)

    # Initialization of curses.
    cui.initialize()

    # Read the directory.
    st.update()

    while st.result is None:

        # Get the screen size.
        h, w = stdscr.getmaxyx()

        # Draw to the screen.
        stdscr.erase()
        cui.draw_filer_window(stdscr, conf.width_ratios, st.headers(w), *st.contents(), st.preview, conf.cmap)
        stdscr.refresh()

        # Accepts keystrokes and executes the corresponding command.
        # Note that the return value of the function "keybind.input()" is just a string.
        exec(key.input(stdscr, st))

        # Apply grep filter.
        st.grep()

    # Convert the result to the shortest file path, then join with a blank and return.
    return " ".join([shlex.quote(utils.shorten_path(path)) for path in st.result]).strip()

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
