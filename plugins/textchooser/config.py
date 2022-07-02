#!/usr/bin/env python3
# coding: utf-8
#
# This script provides constants used through the software.
# Since the instance of Config class is immutable,
# please be careful not to overwrite the contents of the constants.
# (the class Config implemented with NamedTuple for that purpose!)

import typing

# Immutable set of constants used in this software.
class Config(typing.NamedTuple):

    # Set the color of each file type. The color numbers follow ANSI standards.
    #     (0) Black  (1) Red      (2) Green  (3) Yellow
    #     (4) Blue   (5) Magenta  (6) Cyan   (7) White
    # The correspondence between each file type and the symbol is as follows.
    #     (-) regular file,          (l) symbolic link,
    #     (b) block device file,     (p) named pipe,
    #     (c) character device file, (x) executable file,
    #     (d) directory,             (*) selected file,
    cmap: dict = {"-" : 0, "b" : 1, "c" : 5, "d" : 4,
                  "l" : 6, "p" : 5, "x" : 2, "*" : 3}

    # Command correspondence table of process number selection mode.
    commands: dict = {
        "G" : "cmd.move_to_bottom",
        "d" : "cmd.cursor_move_down_more",
        "j" : "cmd.cursor_move_down",
        "k" : "cmd.cursor_move_up",
        "q" : "cmd.quit",
        "r" : "cmd.refresh",
        "u" : "cmd.cursor_move_up_more",
        "^B": "cmd.cursor_move_up_more",
        "^F": "cmd.cursor_move_down_more",
        "^J": "cmd.execute",
        " " : "cmd.toggle_selection",
        "$" : "cmd.move_to_bottom",
        "/" : "cmd.toggle_grep",
        "0" : "cmd.move_to_top",
    }

    # The amount of movement when moving the cursor "a lot".
    curs_move_more: int = 25

    # The prompt format of the first line of the header which displayed at the top
    # of the filer screen. The value of the environment variable can be used as it is.
    header_dir = "\x1b[97m[\x1b[32m{USER}\x1b[97m::\x1b[35mclara\x1b[97m]\x1b[34m "

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
