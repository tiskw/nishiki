"""
nishiki - a simple shell wrapper
"""

__VERSION__ = "0.0"

# Import standard libraries.
import argparse, datetime, importlib, os, signal, traceback, shlex, subprocess, sys

# Import original modules.
import readcmd, runner, utils


def parse_args():
    """
    Parse command line arguments.

    Returns:
        argparse.Namespace: Parsed command line arguments.
    """
    fmtcls = lambda prog: argparse.HelpFormatter(prog, max_help_position=30)
    parser = argparse.ArgumentParser(prog="nishiki", description=__doc__.strip(),
                                     formatter_class=fmtcls, add_help=False)

    # Options for shell interface.
    group1 = parser.add_argument_group("Shell wrapper options")
    group1.add_argument("-c", "--config", metavar="PATH", type=str, default=None, help="path to config file")

    # Other options.
    group2 = parser.add_argument_group("Other options")
    group2.add_argument("-h", "--help", action="help", help="show this message and exit")
    group2.add_argument("-v", "--version", action="version", version=__VERSION__, help="show version info and exit")

    return parser.parse_args()


def main(args):
    """
    Entry point of Nishiki.

    Args:
        args (argparse.Namespace): Parsed used input.
    """
    # Load config module.
    if args.config: config = importlib.machinery.SourceFileLoader("config", args.config).load_module()
    else          : config = importlib.import_module("config")

    # Instanciate config class.
    cfg = config.NishikiConfig()

    # Create ReadCmd instance.
    rc = readcmd.ReadCmd(cfg)

    # Create CommandRunner instance.
    cr = runner.CmdRunner()

    # Print welcome message.
    print(cfg.welcome_message.format(**cfg.colors))

    while True:

        # Get user input.
        ret = rc.input()

        # Run command if the user input is not None and not empty string.
        if ret is not None and len(ret) > 0:
            try              : cr.run(ret, shlex.split(ret), cfg)
            except SystemExit: break
            except           : print(traceback.format_exc())

    # Strip history file.
    rc.histmn.strip()


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
