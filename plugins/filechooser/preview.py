#!/usr/bin/env python3
# coding: utf-8
#
# This script provides function to preview a file.

import os
import sys

import utils
import cui

# The main function of this module.
# The other functions other than this function should not be exposed to the outside.
def preview(win, rect, filepath, max_preview_bytes = 4096):

    # Decompose the rect into (x, y, w, h) for making it easier to use later.
    x, y, w, h = rect

    # Use the "file" command to get the MIME type.
    command = f"file --mime '{filepath}'"
    output  = utils.runcmd(command)
    mimes   = output.split(":")[-1].split(";")[0].strip().split("/")

    # Determine the behavior of this function by the first half of the MIME type.
    if   mimes[0] == "text" : preview_txtfile(win, rect, filepath, max_preview_bytes)
    elif mimes[0] == "image": preview_command(win, rect, "mediainfo '%s'" % filepath, max_preview_bytes)
    elif mimes[0] == "video": preview_command(win, rect, "mediainfo '%s'" % filepath, max_preview_bytes)

    # Determine the behavior of this function by the second half of the MIME type.
    elif mimes[1] == "zip"            : preview_command(win, rect, f"zipinfo '{filepath}'", max_preview_bytes)
    elif mimes[1] == "x-7z-compressed": preview_command(win, rect, f"7z l '{filepath}'", max_preview_bytes)
    elif mimes[1] == "directory"      : preview_dirpath(win, rect, filepath)

    # No preview is provided for those that could not be determined.
    else: pass

# Execute the command and display the result. This function supports ANSI color code.
def preview_command(win, rect, command, max_preview_bytes):

    # Execute the given command and get the result as a string.
    text = utils.runcmd(f"{command} | head -c {max_preview_bytes}")

    # Draw the command execution result on the screen.
    cui.textbox(win, rect, text)

# Execute the ls command and display the result (with color).
def preview_dirpath(win, rect, dirpath):

    # Decompose the rect into (x, y, w, h) for making it easier to use later.
    x, y, w, h = rect

    # Execute the ls command and format the result in the column.
    output = utils.runcmd(f"ls -F --group-directories-first --color '{dirpath}'")
    output = utils.column(output.strip().split(), w)

    # Draw the result on the screen.
    cui.textbox(win, rect, output)

# Highlight the contents of the text file with the highlight command.
def preview_txtfile(win, rect, filepath, max_preview_bytes):

    # Execute the command highlight to get the result as a character string.
    *_, suffix    = os.path.splitext(filepath)
    option_suffix = ("-S " + suffix[1:]) if suffix else ""
    command_shell = f"head -c {max_preview_bytes} '{filepath}' | expand -t4 | highlight --force -O ansi -q {option_suffix}"
    text_ansi_esc = utils.runcmd(command_shell)

    # Draw the result of ls command on the screen.
    cui.textbox(win, rect, text_ansi_esc)

sys.modules[__name__] = preview

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
