#!/usr/bin/env python3

"""
Usage: coreutils [--help] [--version] command [options...]

Python implementation of GNU Coreutils.
Not all coreutils commands are implemented (see --help for details).
"""

__VERSION__ = "0.0"

import argparse
import importlib
import pathlib
import sys
import zipfile


def get_function_names_from_zipfile(filepath):
    """
    """
    # Open the zip file as read mode.
    with zipfile.ZipFile(filepath, "r") as ifp:

        # Iterate all files in the zip file.
        for info in ifp.infolist():

            # Returns only command files.
            if info.filename.startswith("command_") and info.filename.endswith(".py"):

                # Returns command name (strip ".py" from the file path).
                yield info.filename[:-3]


def get_command_name(modules=None):
    """
    """
    def print_help(modules):
        """
        Create help string.
        """
        # Print base help string.
        print(__doc__.strip())

        # Print [available commands] section in the help message.
        if modules is not None:

            # Print header.
            print("\navailable commands:")

            # Add items.
            for name, module in modules.items():
                print("  %s: %s" % (name, module.__doc__.strip().split("\n")[0]))

    # Prints help message and returns None if the input arguments is invalid.
    if (len(sys.argv) < 2) or (sys.argv[1] == "--help") or (sys.argv[1] not in modules):
        return print_help(modules)

    elif sys.argv[1] == "--version":
        return print(__VERSION__)

    # Otherwise, returns the given command name.
    else: return sys.argv[1]


def main():

    # Get existing command module names.
    exefile  = pathlib.Path(__file__).parent
    modnames = list(get_function_names_from_zipfile(exefile))

    # Import existing command modules.
    # Note: The prefix "command_" is stripped in the key name.
    modules = {mname[8:]:importlib.import_module(mname) for mname in modnames}

    # Parse command line arguments and get command name to be called.
    command_name = get_command_name(modules)

    # Call corresponding functions if the given command is valid.
    if command_name is not None: modules[command_name].main()


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
