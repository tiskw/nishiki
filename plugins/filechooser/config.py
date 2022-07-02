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

    # Filer command correspondence table.
    commands: dict = {
        "G" : "cmd.move_to_bottom",
        "d" : "cmd.cursor_move_down_more",
        "h" : "cmd.change_directory_backward",
        "j" : "cmd.cursor_move_down",
        "k" : "cmd.cursor_move_up",
        "l" : "cmd.change_directory_forward",
        "q" : "cmd.quit",
        "r" : "cmd.refresh",
        "u" : "cmd.cursor_move_up_more",
        "^B": "cmd.cursor_move_up_more",
        "^F": "cmd.cursor_move_down_more",
        "^J": "cmd.execute",
        "^K": "cmd.execute_previous",
        "^P": "cmd.toggle_preview",
        "-" : "cmd.change_directory_backward",
        "." : "cmd.toggle_showdot",
        " " : "cmd.toggle_selection",
        "$" : "cmd.move_to_bottom",
        "0" : "cmd.move_to_top",
        "/" : "cmd.toggle_grep",
    }

    # The amount of movement when moving the cursor "a lot".
    curs_move_more: int = 25

    # The format of file information displayed in the list in the center of the screen.
    fileinfo: str = "{date} {time}  {size:>4}"

    # The prompt format of the first line of the header which displayed at the top
    # of the filer screen. The value of the environment variable can be used as it is.
    header_dir = "\x1b[97m[\x1b[32m{USER}\x1b[97m::\x1b[35mclara\x1b[97m]\x1b[34m "

    # The prompt format of the second line of the header which displayed at the top
    # of the filer screen. The value of the environment variable can be used as it is.
    header_file = "\x1b[97m>\x1b[0m "

    # The maximum number of lines of source code to display in the preview.
    max_preview_bytes: int = 3072

    # The format of file information displayed in the list in the center of the screen.
    psinfo: str = "{pid:>7}  {user:>7}  {%cpu:>5}  {%mem:>5}  {cmdname:<30}  {cmd}"

    # Filer screen division ratio.
    width_ratios: tuple = (0.15, 0.45, 0.40)

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
