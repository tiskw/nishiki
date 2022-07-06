#!/usr/bin/env python3
# coding: utf-8
#
# A script that collects referentially transparent functions related to drawing.

import curses

import utils

# A function that initializes Curses.
def initialize():

    # Make cursor invisible.
    curses.curs_set(0)

    # Color initialization. The background color is assumed to be transparent.
    curses.start_color()
    curses.use_default_colors()
    for n in range(16): curses.init_pair(n, n, -1)

# A function that draws a list box. The color of the focused line is inverted.
# Where the variable `contents` is a list of tuples with the following format:
#    (left character string, right character string, attributes)
def listbox(win, rect, contents, focus, cmap):

    # Decompose the rect into (x, y, w, h) for making it easier to use later.
    x, y, w, h = rect

    # Modify to ensure that the variable focus is not outside the range of lines, and
    # Copy an instance of the variable focus for later.
    focus = int(utils.clip(0, focus, len(contents)))

    # The length of `contents` will be used a lot in the following steps, so define an alias.
    n = len(contents)

    # Calculate the standard cursor position at which focused line is placed
    # if the given contents is enough long.
    c0 = int(2 * h / 3 + 1)

    # The displayed part is calculated from the standard cursor position and focus position.
    idx_bgn = utils.clip(0, focus - c0, n - h)
    idx_end = utils.clip(h, focus - c0 + h, n)

    # Based on the calculated display position, lists, attr, and focus are limited to the display part only.
    # The variable focus can be overwritten because the instance was copied earlier.
    contents = contents[idx_bgn:idx_end]
    focus   -= idx_bgn

    # The actual drawing work is performed below.
    for row, (left, right, selected, attr) in enumerate(contents):

        # Format the string based on the width information.
        # The drawing area does not extend beyond the rect, and the inversion affects the right edge of the screen.
        # Make sure that the string length matches the width.
        line = utils.clipstr(left, w - utils.strlen(right) - 4, fill=" ", append="~")
        line = "  " + line + " " + right + " "

        # If attr is an integer, convert it to the corresponding color.
        if isinstance(attr, int) and 0 <= attr <= 7:
            attr = curses.color_pair(attr)

        # If a file is selected, add "*" at the beginning to force the highlight color to change.
        if selected:
            line = "*" + line[1:]
            attr = curses.color_pair(cmap.get("*", 0))

        # Add invert attribute if the line matches the cursor position.
        if row == focus: attr += curses.A_REVERSE

        # Draw a character string on the screen.
        win.addstr(y + row, x, line, attr)

# A function that display the given string. This function supports ANSI color code.
def textbox(win, rect, text):

    # Decompose the rect into (x, y, w, h) for making it easier to use later.
    x, y, w, h = rect

    # For each line of the string with an escape sequence, the escape sequence is interpreted and draw to screen.
    # Note that this can be achieved because the output of the highlight command is independent for each line.
    for row, line in enumerate(text.split("\n")):

        # Limitation on the number of lines so that the drawing area does not exceed rect.
        if row >= h: break

        # Convert a string with an escape sequence into a tuple (escaped content, followed by the string).
        # This can be achieved by (1) divide each line with "\x1b" as the delimiter, and (2) divide them with m as the delimiter only once.
        # However, if the escape sequence is not included at the beginning or end of the line,
        # a tuple like (following string, ) is generated. Therefore, the process for manage this issue is added to the end of lines.
        tokens = [token.split("m", 1) for token in line.split("\x1b[")]
        tokens = [["", "m".join(tokens[0])]] + tokens[1:]

        # Line breaks and unnecessary white spaces at the end of lines are annoying and should be deleted.
        tokens[-1][-1].rstrip()

        # Prepare a variable to count the number of drawn lines.
        col = 0

        # Move the cursor to the upper left of the drawing area.
        win.move(y + row, x)

        for color, text in tokens:

            # Limit the number of characters to be drawn so that the drawing area does not exceed the rect.
            if col + utils.strlen(text) > w:
                text = utils.clipstr(text, w - col)

            # Interpret escape sequences.
            # Enclose it in try-except, since many errors tend to be caused in here.
            try   : color = int(color.split(";")[-1])
            except: color = 0

            # Convert the color code to the curses attribute.
            if   30 <= color <= 37: attr = curses.color_pair(color - 30)
            elif 90 <= color <= 97: attr = curses.color_pair(color - 82)
            else                  : attr = curses.color_pair(0)

            # Draw the string on the screen.
            win.addstr(y + row, x + col, text, attr)

            # Update the number of characters drawn in this line.
            col += len(text)

# A function that draws a window screen in process number selection mode.
# The variable content is a list that stores the drawing contents.
def draw_textchooser_window(win, headers, contents, focus, cmap):

    # Get the screen size.
    h_max, w_max = win.getmaxyx()

    # Draw the header (only the 1st line).
    textbox(win, (0, 0, w_max - 1, 1), headers[0])

    # Draw a horizontal line.
    win.hline(1, 0, curses.ACS_HLINE, w_max - 2)

    # Draw a process list.
    listbox(win, (0, 2, w_max - 1, h_max - 3), contents, focus, cmap)

    # The screen refresh command "win.refresh" is not called in here.
    # Someone will call the refresh command outside of this function.

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
