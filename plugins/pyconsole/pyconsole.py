#!/usr/bin/env python3
# coding: utf-8


""" pyconsole - interactive console on Python """


import argparse

from client import main_client
from daemon import main_daemon


__version__ = "0.0"


def parse_commandline_arguments():
    """
    A function that parses command line arguments.
    """
    formatter_class = lambda prog: argparse.ArgumentDefaultsHelpFormatter(prog, max_help_position=30)
    parser = argparse.ArgumentParser(description = __doc__.strip(), formatter_class=formatter_class)
    parser.add_argument("mode", choices = ["c", "d"], help = "running mode (c: client, d: daemon)")
    parser.add_argument("-a", "--anchor", default = None, help = "variable to be dumped")
    parser.add_argument("-o", "--output", default = None, help = "output file")
    parser.add_argument("-p", "--port", default = "9999", help = "socket port")
    parser.add_argument("-t", "--host", default = "localhost", help = "socket host")
    parser.add_argument("-v", "--version", action = "version", version = get_version())
    return parser.parse_args()


def get_version():
    """
    A function that returns version information as a string.
    """
    return f"pyconsole {__version__}"


def main():
    """
    Entry point of this software.
    """
    # Parse command line analysis.
    args = parse_commandline_arguments()

    if   args.mode == "c": main_client(args)
    elif args.mode == "d": main_daemon(args)


if __name__ == "__main__":
    main()


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
