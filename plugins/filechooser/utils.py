#!/usr/bin/env python3
# coding: utf-8

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
def clipstr(text, width, fill=None, append=None):

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

# A function to arrange strings in a table format.
# This functionality is close to the command "column" commonly used in Linux system,
# but the implementation is completely unique, and there is no particular compatibility with the "column" command.
def column(texts, width, join=True):

    # Divide the given array (character string) by length.
    def chunk(xs, length):
        return [xs[n:n + length] for n in range(0, len(xs), length)]

    # A function that strips the ANSI code from a given string.
    # The function "strlen()" counts the ANSI color code as the number of characters,
    # therefore striping of ANSI code is necessary for calculating
    # the true length of a string that contains ANSI color code.
    def strip_ansi_code(text):
        return re.sub("\x1b\[[^a-zA-Z]*[a-zA-Z]", "", text)

    # If an empty list is given, the function is terminated without doing anything.
    if not texts: return ("" if join else [])

    # Calculate the length of the given string. Note that ANSI code may be included.
    lengths = [strlen(strip_ansi_code(text)) for text in texts]

    # Calculate the maximum value of the length of a given character string.
    # The magic number +2 is provided to add at least two spaces after the string.
    max_length = max(lengths) + 2

    # Calculate the number of columns in the table.
    # The term "max(1, ...)" is necessary to avoid that num_col become zero.
    num_col = max(1, width // (max_length))

    # Create a table.
    table = ["".join(ss) for ss in chunk([text + " " * (max_length - length) for text, length in zip(texts, lengths)], num_col)]

    # Return joined string if the argument "join" is true.
    return "\n".join(table) if join else table

# A function to get the list of files contained in a directory.
# If the argument previous is true, ls the previous directory.
# The return value is a list of dictionary which has the following keys:
#     ["perm", "nlinks", "user", "group", "size", "month", "day", "time", "name", "path"]
def dictls(dirpath, ls_opt, previous=False, cmap=dict()):

    # Variable names and data types for interpreting the results of the ls command.
    KEYS  = ["perm", "nlinks", "user", "group", "size", "date", "time", "name"]
    TYPES = [   str,      int,    str,     str,    str,    str,    str,    str]

    # Dummy data to represent an empty directory.
    EMPTY_ITEM = {"perm": "", "nlinks": 0, "user": "", "group": "", "size": 0, "date": "",
                  "time": "", "name": "EMPTY DIREDTORY", "type": "", "color": 1, "path": ""}

    # Decompose the result of the "ls" command into a dictionary type.
    def tokenize(line):
        vals = line.split(maxsplit=len(KEYS)-1)
        return {key:t(val) for key, t, val in zip(KEYS, TYPES, vals)}

    # After decomposing the result of the "ls" command into a dictionary type,
    # the following post-processing is performed.
    def postproc(token):
        token["path"]  = os.path.join(dirpath, token["name"])
        token["type"]  = "x" if token["perm"][0] == "-" and "x" in token["perm"] else token["perm"][0]
        token["color"] = cmap.get(token["type"], 0)
        return token

    # If the argument "previous" is specified, go back to the previous directory.
    if previous: dirpath = os.path.dirname(dirpath)

    # Execute "ls" in the background, get the result, divide the result into lines, and stripe it.
    # The first line of the "ls" command is ignored because the first line of the ls command is useless one ("total ...").
    output = runcmd(f"ls -hl{ls_opt} --time-style=long-iso --group-directories-first '{dirpath}'")
    lines  = [line.strip() for n, line in enumerate(output.strip().split("\n")) if n > 0]

    # Parse the result of the "ls" command line by line with the tokenize and postproc function.
    return [postproc(tokenize(line)) for line in lines] if lines else [EMPTY_ITEM]

# A function that returns a list of file names that match the argument "name".
# If multiple elements match, it returns the first one.
# If no matching element is found, the default value is returned.
def findindex(items, name, default=0):

    candidates = (n for n, item in enumerate(items) if item["name"] == name)
    return min(candidates, default=default)

# A function that clips a string to a specified length. This function supports multi-byte characters.
# The specifications of this function is the same as "clipstr()" except that this function will cut the left side of the given string.
def rclipstr(text, width, fill=None, append=None):

    return clipstr(text[::-1], width, fill, append)[::-1]

# A function to execute a command.
# STDERR is disabled so as not to destroy the curses interface.
def runcmd(command):

    try   : return subprocess.check_output(command + " 2> /dev/null", shell=True).decode(errors="backslashreplace").strip()
    except: return None

# Make the file path the shortest.
# The current implementation returns the shorter of absolute and relative paths.
def shorten_path(filepath):

    abspath = os.path.abspath(os.path.normpath(filepath))
    relpath = os.path.relpath(os.path.normpath(filepath))
    return abspath if len(abspath) < len(relpath) else relpath

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
