"""
This module provides a class for command execution which able to handle signals crrectly.
"""

# Import standard libraries.
import datetime, os, signal, subprocess, sys

# Import original modules.
import utils


class CmdRunner:

    def __init__(self):
        self.proc = None

    def chdir(self ,cmd, args):
        """
        Change current directory.
    
        Args:
            cmd  (str) : Raw user input.
            args (list): Parsed used input.
        """
        if len(args) == 1: os.chdir(os.path.expanduser("~"))
        else             : os.chdir(os.path.expanduser(args[1]))

    def run(self, cmd, args, cfg):
        """
        Run the given command.
    
        Args:
            cmd  (str)          : Raw user input.
            args (list)         : Parsed user input.
            cfg  (NishikiConfig): Config instance.
        """
        # Print user input.
        datetimestr = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        print("{C7}[{s}]{CX} {c}".format(c=utils.colorize(cmd), s=datetimestr, **cfg.colors))

        # Run command.
        if   args[0] == "exit": sys.exit()
        elif args[0] == "cd"  : self.chdir(cmd, args)
        else                  : self.run_with_signal_care(cmd)

        # Print blank line for readability.
        print("")

    def run_with_signal_care(self, cmd):
        """
        """
        # Override SIGINT signal.
        signal.signal(signal.SIGINT, self.signal_handler)

        # Open subprocess and execute the command.
        self.proc = subprocess.Popen(cmd, shell=True)

        # Wait while the command finish.
        self.proc.wait()
 
    def signal_handler(self, signum, frame):
        """
        Signal handler for command execution.

        Args:
            signum (int)  : Signal number:
            frame  (frame): Frame object.
        """
        self.proc.send_signal(signum)
 

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
