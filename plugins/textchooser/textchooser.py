#!/usr/bin/env python3
# coding: utf-8

"""textchooser - multi purpose text selection plugin"""

import argparse
import curses
import locale
import os

import config
import session

__version__ = "0.0"

# A function that parses command line arguments.
def parse_commandline_arguments():

    fmtcls = lambda prog: argparse.HelpFormatter(prog, max_help_position = 30)
    parser = argparse.ArgumentParser(description = __doc__.strip(), formatter_class = fmtcls, add_help = False)

    group1 = parser.add_argument_group("input value arguments")
    group1.add_argument("-c", "--cmd",     default = "",   type = str, help = "command for generating target text")
    group1.add_argument("-d", "--delim",   default = "\n", type = str, help = "delimiter of text or command output")
    group1.add_argument("-i", "--input",   default = "",   type = str, help = "path to input text file")
    group1.add_argument("-o", "--output",  default = "",   type = str, help = "output file")
    group1.add_argument("-r", "--reverse", action="store_true",        help = "reverse input file")
    group1.add_argument("-t", "--text",    default = "",   type = str, help = "target text")

    group2 = parser.add_argument_group("output value arguments")
    group2.add_argument("-f", "--field", default = -1, type = int, help = "number of field to be returnd")

    group3 = parser.add_argument_group("other optional arguments")
    group3.add_argument("-h", "--help",    action = "help", help = "show help message and exit")
    group3.add_argument("-v", "--version", action = "version", version = get_version())

    return parser.parse_args()

# A function that returns version information as a string.
def get_version():

    return f"textchooser {__version__}"

# Dump the given string to file.
def print_to_file(text, filepath):

    os.makedirs(os.path.dirname(filepath), exist_ok = True)

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

    # Start the filer/pgrep session, get the output, and print it to STDOUT.
    output = curses.wrapper(lambda win: session.start(win, args, conf))
    if   output and args.output: print_to_file(output, args.output)
    elif output                : print(output)

if __name__ == "__main__": main()

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
