#!/usr/bin/env python3
# coding: utf-8
#
# A script that collects callback functions. The argument for each function is always st, instance of state class,
# Also, note that the return value of the function will be ignored, however, no error occurs even if it has a return value.

import os
import utils

# A function that moves to the selected directory.
# If something other than a directory is selected, do nothing.
def change_directory_forward(st):

    if st.item("type") == "d":
        st.dirpath = st.target()
        st.update()

# A function that moves to the previous directory.
# If this function is called in the root directory, nothing happens (as a result).
def change_directory_backward(st):

    prev_dirname  = os.path.basename(st.dirpath)
    st.dirpath = os.path.dirname(st.dirpath)
    st.update()
    st.focus = utils.findindex(st.items, prev_dirname)

# A function that lowers the cursor by one.
# It is called from both Filer and Pgrep, but it can be handled by the same code.
def cursor_move_down(st):

    st.focus = min(st.focus + 1, len(st.items) - 1)

# A function that lowers the cursor a lot.
# It is called from both Filer and Pgrep, but it can be handled by the same code.
def cursor_move_down_more(st):

    st.focus = min(st.focus + st.config.curs_move_more, len(st.items) - 1)

# A function that raises the cursor by one.
# It is called from both Filer and Pgrep, but it can be handled by the same code.
def cursor_move_up(st):

    st.focus = max(st.focus - 1, 0)

# A function that raises the cursor a lot.
# It is called from both Filer and Pgrep, but it can be handled by the same code.
def cursor_move_up_more(st):

    st.focus = max(st.focus - st.config.curs_move_more, 0)

# A dummy function that does nothing. It is used when there is no corresponding key binding.
# I'm not a child I don't need, so don't erase it.
def dummy(st): pass

# A function that returns the selected file as a result.
# However, if the selected file is a dummy file such as an empty directory, nothing is done.
# Whether it is a dummy file or not is judged by whether the file type is properly given.
def execute(st):

    if   len(st.select) > 0: st.result = st.select
    elif st.item("type")   : st.result = [st.target()]

# A function that returns the current directory (not the selected file) as a result.
def execute_previous(st):

    st.result = [st.dirpath]

# A function that moves the cursor to the bottom.
# It is called from both Filer and Pgrep, but it can be handled by the same code.
def move_to_bottom(st):

    st.focus = len(st.items) - 1

# A function that moves the cursor to the top.
# It is called from both Filer and Pgrep, but it can be handled by the same code.
def move_to_top(st):

    st.focus = 0

# A function that terminates the filer.
def quit(st):

    st.result = []

# A function that reloads the specified directory or reloads the process list.
def refresh(st):

    st.update()

# A function that shifts to search mode or returns to normal mode.
def toggle_grep(st):

    st.grepstr = ("/" + st.grepstr) if "/" not in st.grepstr else st.grepstr[1:]

# A function to show/hide preview window.
def toggle_preview(st):

    st.preview = not st.preview

# A function to select/deselect a file.
# It is called from both Filer and Pgrep, but it can be handled by the same code.
def toggle_selection(st):

    if st.target() in st.select: st.select.remove(st.target())
    else                       : st.select.append(st.target())
    cursor_move_down(st)

# A function to show/hide dotfiles.
def toggle_showdot(st):

    st.showdot = not st.showdot
    st.update()

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
