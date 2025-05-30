#!/usr/bin/env python3
"""
Usage:
    pycon [-w PATH] [-r PATH] [-v] [-h]

Connector to Python interpreter.

Input/output arguments:
  -r, --path_r PATH   Right-hand-side string.   [default: ~/.local/share/nishiki/pycon_r]
  -w, --path_w PATH   Left-hand-side string.    [default: ~/.local/share/nishiki/pycon_w]

Other options:
  -v, --version       Show version info and exit.
  -h, --help          Show help message and exit.
"""

# Define version.
__version__ = "2025.04.19"

# Import standard libraries.
import argparse
import code
import os
import pathlib

# For type hints.
from typing import Any


#---------------------------------------------------------------------------------------------------
# Connector to Python interpreter
#---------------------------------------------------------------------------------------------------

def pycon(args: argparse.Namespace) -> None:
    """
    Connector to Python interpreter.

    Args:
        (argparse.Namespace): Parsed command line arguments.
    """
    def dump(*pargs: Any):
        """
        Dump results to the pipe for writing.
        """
        with path_w.open("wt") as ofp:
            ofp.write(" ".join(map(str, pargs)) + "\n")

    #
    path_r = pathlib.Path(args.path_r).expanduser().resolve()
    path_w = pathlib.Path(args.path_w).expanduser().resolve()

    #
    if path_r.exists():
        raise FileExistsError(f"FIFO file for reading already exists: {path_r}")
    if path_w.exists():
        raise FileExistsError(f"FIFO file for writing already exists: {path_w}")

    #
    os.mkfifo(path_r)
    os.mkfifo(path_w)

    #
    interpreter = code.InteractiveInterpreter(locals() | {"dump": dump})

    while True:

        #
        with path_r.open("rt") as ifp:
            text = ifp.read()

        #
        if text.strip() == "exit":
            break

        #
        interpreter.runsource(text, symbol="exec")

    #
    os.remove(path_r)
    os.remove(path_w)


#---------------------------------------------------------------------------------------------------
# Main function
#---------------------------------------------------------------------------------------------------

def parse_args() -> argparse.Namespace:
    """
    Parse command line arguments.

    Returns:
        (argparse.Namespace): Parsed command line arguments.
    """
    parser = argparse.ArgumentParser(prog="callcmd", add_help=False)
    parser.add_argument("-r", "--path_r", type=str, default="~/.local/share/nishiki/pycon_r")
    parser.add_argument("-w", "--path_w", type=str, default="~/.local/share/nishiki/pycon_w")
    parser.add_argument("-v", "--version", action="store_true")
    parser.add_argument("-h", "--help", action="store_true")
    return parser.parse_args()


def main(args: argparse.Namespace) -> None:
    """
    Entrypoint of this script.

    Args:
        args (argparse.Namespace): Parsed command line arguments.
    """
    if args.version:
        print("pycon", __version__)
        print("Copyright (C) Tetsuya Ishikawa")
        print("This software is released under the MIT License")

    elif args.help:
        print(__doc__.strip())

    else:
        pycon(args)


if __name__ == "__main__":
    main(parse_args())


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
