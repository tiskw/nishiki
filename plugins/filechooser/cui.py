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

# A function that draws a rectangular border.
def rectbox(win, rect):

    # Decompose the rect into (x, y, w, h) for making it easier to use later.
    x, y, w, h = rect

    # Calculate the upper left coordinates (x0, y0) and the lower right coordinates (x1, y1).
    x0, x1 = x, x + w - 1
    y0, y1 = y, y + h - 1

    # Place the four corner characters.
    # Note that the function win.insch moves the character to the right.
    win.insch(y0, x0, curses.ACS_ULCORNER)
    win.insch(y0, x1, curses.ACS_URCORNER)
    win.insch(y1, x0, curses.ACS_LLCORNER)
    win.insch(y1, x1, curses.ACS_LRCORNER)

    # Place horizontal line characters, being careful not to fill the corners.
    win.hline(y0, x0 + 1, curses.ACS_HLINE, w - 2)
    win.hline(y1, x0 + 1, curses.ACS_HLINE, w - 2)

    # Arrange vertical line characters while being careful not to fill the corners.
    win.vline(y0 + 1, x0, curses.ACS_VLINE, h - 2)
    win.vline(y0 + 1, x1, curses.ACS_VLINE, h - 2)

# A function that draws a rectangular border.
# The inside of the rectangle can be separated by a vertical line.
def rectbox_with_vlines(win, rect, cols_vline):

    # Decompose the rect into (x, y, w, h) for making it easier to use later.
    x, y, w, h = rect

    # Calculate the lower right coordinates (x1, y1).
    y0, y1 = y, y + h - 1

    # First, draw the outer frame.
    rectbox(win, rect)

    # Then, draw a vertical lines.
    for col in cols_vline:

        # Place the TEEs.
        win.addch(y0, col, curses.ACS_TTEE)
        win.addch(y1, col, curses.ACS_BTEE)

        # Place vertical line characters, being careful not to fill the TEEs.
        win.vline(y0 + 1, col, curses.ACS_VLINE, h - 2)

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

        for color, value in tokens:

            # Limit the number of characters to be drawn so that the drawing area does not exceed the rect.
            if col + utils.strlen(value) >= w:
                value = utils.clipstr(value, w - col, append="~")

            # Interpret escape sequences.
            # Enclose it in try-except, since many errors tend to be caused in here.
            try   : color = int(color.split(";")[-1])
            except: color = 0

            # Convert the color code to the curses attribute.
            if   30 <= color <= 37: attr = curses.color_pair(color - 30)
            elif 90 <= color <= 97: attr = curses.color_pair(color - 82)
            else                  : attr = curses.color_pair(0)

            # Draw the string on the screen.
            win.addstr(y + row, x + col, value, attr)

            # Update the number of characters drawn in this line.
            col += utils.strlen(value)

# A function that draws a window screen in filer mode.
# The variable contents1 is the contents of the left window,
# contents2 is the list containing the contents of the center window.
# The contents3 must be a function which can be called by "contents3(win, rect)".
def draw_filer_window(win, width_ratios, headers, contents1, contents2, contents3, focus1, focus2, show_preview, cmap):

    # Clear the screen (actually it is cleared when refresh() is called).
    win.erase()

    # Get the maximum width of the screen.
    h_max, w_max = win.getmaxyx()

    # Get the number of lines in the header and footer.
    # The height of the frame is reduced by the number of lines.
    n_headers = len(headers)

    # Just to be on the safe side, limit the length of the input width ratio list to 3 and also normalize it.
    width_ratios = [r / sum(width_ratios[:3]) for r in width_ratios[:3]]

    # Calculate the width of the list box (note that it is the width of its contents, not the width of the frame).
    # Note that each window size is different whether showing preview window or not.
    w1 = round(w_max * width_ratios[0]) - 1
    w2 = round(w_max * width_ratios[1]) - 1 if show_preview else w_max - w1 - 3
    w3 = w_max - (w1 + w2) - 4              if show_preview else 0

    # Draw the header line.
    textbox(win, (0, 0, w_max, h_max), "\n".join(headers))

    # Draw the border lines.
    rectbox_with_vlines(win, (0, n_headers, w_max, h_max - n_headers), [w1 + 1, w1 + w2 + 2 if show_preview else w1 + 1])

    # Calculate position/size of the drawing area (not including the border) of each window.
    rect1 = (1,           n_headers + 1, w1, h_max - n_headers - 2)
    rect2 = (w1 + 2,      n_headers + 1, w2, h_max - n_headers - 2)
    rect3 = (w1 + w2 + 3, n_headers + 1, w3, h_max - n_headers - 2)

    # Draw the contents of the left window and the center window.
    listbox(win, rect1, contents1, focus1, cmap)
    listbox(win, rect2, contents2, focus2, cmap)

    # The drawing of the right window is performed below, but the drawing process is a little complicated.
    # Because the contents3 allows not only strings but also callable function like preview function.
    # First, if contents3 is a simple string, the string is drawn in the specified rectangular area.
    if show_preview and isinstance(contents3, str): textbox(win, rect3, contents3)

    # Next, run the contents3 assuming that the contents3 is a callable function.
    # If a runtime error occurs, do nothing.
    elif show_preview and contents3:
        try   : contents3(win, rect3)
        except: pass

    # The screen refresh command "win.refresh" is not called in here.
    # Someone will call the refresh command outside of this function.

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
