#!/usr/bin/env python3
# coding: utf-8

""" filechooser - file/directory path selection plugin """

import argparse
import curses
import locale
import os
import shlex
import stat
import subprocess

import config
import session

__version__ = "0.3"

# A function that parses command line arguments.
def parse_commandline_arguments():

    parser = argparse.ArgumentParser(description=__doc__.strip())
    parser.add_argument("-o", "--output", default="", help="Output file")
    parser.add_argument("-v", "--version", action="version", version=get_version())

    return parser.parse_args()

# A function that returns version information as a string.
def get_version():

    return f"filechooser {__version__}"

# Dump the given string to file.
def print_to_file(text, filepath):

    os.makedirs(os.path.dirname(filepath), exist_ok=True)

    with open(filepath, "wt") as ofp:
        ofp.write(text)

# Entry point of this software.
def main():

    # Get config instance.
    conf = config.Config()

    # Parse command line analysis.
    # (I used "docopt" package before, but ported it to argparse to reduce dependencies)
    args = parse_commandline_arguments()

    # Initialize the locale for curses.
    locale.setlocale(locale.LC_ALL, "")

    # Remove output file if exists.
    if args.output and os.path.exists(args.output):
        os.unlink(args.output)

    # Start the filer/pgrep session, get the output, and print it to STDOUT.
    output = curses.wrapper(lambda win: session.start(win, args, conf))
    if   output and args.output: print_to_file(output, args.output)
    elif output                : print(output)

if __name__ == "__main__": main()

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
