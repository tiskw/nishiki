#!/usr/bin/env python3

"""
Concatenate FILE(s) to standard output.
With no FILE, or when FILE is -, read standard input.
"""

import argparse, os, sys


def parse_args():
    """
    Parse command line arguments.

    Returns:
        args (argparse.Namespace): Parsed command line arguments.
    """
    fmtcls = lambda prog: argparse.HelpFormatter(prog, max_help_position=30)
    parser = argparse.ArgumentParser("cat", description=__doc__.strip(), formatter_class=fmtcls, add_help=False)

    # Add positional arguments.
    group1 = parser.add_argument_group("Required arguments")
    group1.add_argument("files", nargs="+", help="Input files")

    # Add optional arguments.
    group2 = parser.add_argument_group("Optional arguments")
    group2.add_argument("-A", "--show-all",         action="store_true", help="Equivalent to -vET")
    group2.add_argument("-b", "--number-nonblank",  action="store_true", help="Number nonempty output lines, overrides -n")
    group2.add_argument("-e",                       action="store_true", help="Equivalent to -vE")
    group2.add_argument("-E", "--show-ends",        action="store_true", help="Display $ at end of each line")
    group2.add_argument("-n", "--number",           action="store_true", help="Number all output lines")
    group2.add_argument("-s", "--squeeze-blank",    action="store_true", help="Suppress repeated empty output lines")
    group2.add_argument("-t",                       action="store_true", help="Equivalent to -vT")
    group2.add_argument("-T", "--show-tabs",        action="store_true", help="Display TAB characters as ^I")
    group2.add_argument("-v", "--show-nonprinting", action="store_true", help="Use ^ and M- notation, except for LFD and TAB")

    # Add general options.
    group3 = parser.add_argument_group("Others")
    group3.add_argument("--help", action="help", help="Show this help message and exit")
    group3.add_argument("--version", action="version", help="Show version information and exit")

    # Parse command line arguments.
    args = parser.parse_args(sys.argv[2:])

    # -A (--show-all) is equivalent to -vET.
    if args.show_all:
        args.show_nonprinting = True
        args.show_ends        = True
        args.show_tabs        = True

    # -e is equivalent to -vE.
    if args.e:
        args.show_nonprinting = True
        args.show_ends        = True

    # -t is equivalent to -vT.
    if args.t:
        args.show_nonprinting = True
        args.show_tabs        = True

    return args


def cat(ifp):
    """
    Shows the contents of the given file pointer.

    Args:
        ifp (FILE): Input file.
    """
    for line in ifp:
        sys.stdout.write(line)
        sys.stdout.flush()


def main():
    """
    Entry point of this script.
    """
    # Parse command line arguments.
    args = parse_args()

    # Check file existance.
    for filepath in args.files:
        if (filepath != "-") and (not os.path.exists(filepath)):
            print("cat: %s: No such file or directory" % filepath)
            exit()

    for filepath in args.files:

        # Print STDIN.
        if filepath == "-":
            cat(sys.stdin)

        # Print the contents of the file if the file exists.
        elif os.path.exists(filepath):
            with open(filepath) as ifp:
                cat(ifp)


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
