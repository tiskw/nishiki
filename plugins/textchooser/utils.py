#!/usr/bin/env python3
# coding: utf-8

import getpass
import itertools
import os
import re
import subprocess
import unicodedata

# A function that clips the value x in the interval [x_min, x_max].
def clip(x_min, x, x_max):

    return max(x_min, min(x, x_max))

# A function that limits the given string to a specified length. This function supports multi-byte characters.
# If the specified string is longer than the specified length, fill the right side with the character "fill".
# This function uses strwidths() to calculate the character width.
def clipstr(text, width, fill = None, append = None):

    # If the argument "fill" is specified and the string is shorter than the specified length, white spaces are added.
    # In some cases, this processing may generate a string which slightly exceed the specified length,
    # but it's not a problem because the following procedure will cut the extra part.
    if fill and len(text) < width: text_filled = text + fill * ((width - strlen(text)) // strlen(fill) + 1)
    else                         : text_filled = text

    # Calculate the list of widths of the input characters and the cumulative list.
    accu_widths = itertools.accumulate(strwidths(text_filled))

    # Calculate the cutting out position.
    idx_allowable = [i for i, w in enumerate(accu_widths) if w <= width]
    idx_cut_pos   = max(idx_allowable) + 1 if len(idx_allowable) > 0 else 0

    # Get the substring after cutting out position.
    result = text_filled[:idx_cut_pos]

    # If the argument "append" is given, replace the rightmost character with append.
    if (append) and (len(text) > len(result)) and (len(result) == width):
        result = result[:-len(append)] + append

    return result

# A function that returns the process information of the current user.
# The return value is a list of dictionary which has the following keys, where "cmdname" is a base name of command.
#     ["pid", "user", "start", "%cpu", "%mem", "command"]
def dictps(my_process_only = True):

    # Decomposes the result of the "ps" command into a dictionary.
    def tokenize(header, line):
        data = dict(zip(map(lambda s: s.lower(), header), line))
        data.update(cmdname = os.path.basename(data["cmd"].split()[0]))
        return data

    # Execute the "ps" command and get the result.
    ps_command_out = runcmd(f"ps -e -o pid,user,start,pcpu,pmem,cmd").strip()

    # Decompose the command result in the first line (header part) and other parts.
    header, *lines = (line.strip().split(maxsplit = 5) for line in ps_command_out.split("\n"))

    # Converts the result to dictionary and returns it.
    items = [tokenize(header, line) for line in lines]
    if my_process_only: return [item for item in items if re.match(item["user"].replace("+", ".*"), getpass.getuser())]
    else              : return items

# A function to execute a command.
# STDERR is disabled so as not to destroy the curses interface.
def runcmd(command):

    try   : return subprocess.check_output(command + " 2> /dev/null", shell = True).decode(errors = "backslashreplace").strip()
    except: return None

# A function that calculates the length of a string. This function supports multi-byte.
# This function uses "strwidths()" to calculate the character width.
def strlen(text):

    return sum(strwidths(text))

# A function that returns a list of character widths. This function supports multi-byte.
# For example, if you enter an English word, a list is returned in which all elements are 1
# and the length of the list is the string length.
# This function uses "unicodedata.east_asian_width()" to calculate the character width.
def strwidths(text):

    # The function "unicodedata.east_asian_width()" only returns the distinguishing characters
    # used to determine the character width, it does not return the actual width.
    # For the time being, F, W, A should be 2, and the others should be 1.
    # You can realize this mapping by the following dictionary with the default value of 1.
    CHAR_WIDTH = {"F": 2, "W": 2, "A": 2}

    # Create a list of character widths. It is still a list of strings at the stage of the first line,
    # and be a list of character width at the second line.
    char_widths = [unicodedata.east_asian_width(c) for c in text]
    char_widths = [CHAR_WIDTH.get(c, 1) for c in char_widths]

    return char_widths

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
