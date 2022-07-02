#!/usr/bin/env python3
# coding: utf-8


import cmd
import readline
import pathlib
import socket
import subprocess
import time


def daemon_exists(host, port):
    """
    Returns true if the daemon is running.
    """
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        try   : sock.connect((host, port))
        except: return False
    return True


def start_daemon(verbose=True):
    """
    Start daemon. Make sure the daemon is not running before calling this function.
    """
    def get_main_script_filepath():
        """
        Returns the file name of the main script.
        This function works if the main script is zipped as zipapp.
        """
        path = pathlib.Path(__file__)
    
        if path.exists(): return str((path.parent / "pyconsole.py").resolve())
        else            : return str((path.parent).resolve())

    # Get path to the main script, and start daemon.
    filepath_main_script = get_main_script_filepath()
    subprocess.run(f"{filepath_main_script} d &", shell=True)

    # Sleep a while to make sure the daemon started.
    time.sleep(0.1)

    # Dump info message.
    if verbose:
        print("INFO: daemon started")


class PyConsoleInteractiveSession(cmd.Cmd):
    """
    Interactive session using cmd.Cmd.
    """
    def __init__(self, host, port):
        """
        Constructor.
        """
        super().__init__()
        self.address  = (host, port)
        self.prompt   = ">>> "
        self.received = None

    def do_EOF(self, args):
        """
        Exit command loop.
        """
        return True

    def do_exit(self, args):
        """
        Exit command loop.
        """
        return True

    def default(self, line):
        """
        Send the line to the daemon, and receive response from the daemon.
        """
        # Send message to the daemon and receive response from the daemon.
        self.received = self.send(line)

        # Print response to STDOUT if not empty.
        if self.received:
            print(self.received)

        # Exit command loop if the user input is some specific commands.
        return line in ["daemon:kill"]

    def send(self, command):
        """
        Send message to client and receive response.
        """
        # Create a socket.
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:

            # Connect to server.
            sock.connect(self.address)

            # Send data to the daemon.
            sock.sendall(bytes(command.strip() + "\n", "utf-8"))

            # Receive data from the daemon and shutdown.
            received = str(sock.recv(4096), "utf-8").strip()

        return received


def create_output_file_path(filepath_output):
    """
    Create output file and returns it's pathlib.Path instance.
    """
    # Convert filepath from string to pathlib.Path instance.
    path = pathlib.Path(filepath_output)

    # Create parent directory if not exists.
    if not path.parent.exists():
        path.parent.mkdir(exist_ok=True)

    # Raise error if the parent directory is not a directory (e.g. regular file).
    if not path.parent.is_dir():
        raise RuntimeError("failed to create output directory")

    return path


def main_client(args):
    """
    Main function in client mode.
    """
    host = str(args.host)
    port = int(args.port)

    # Create output directory if specified.
    if args.output: path_output = create_output_file_path(args.output)
    else          : path_output = None

    # Start daemon if not exists.
    if not daemon_exists(host, port): 
        start_daemon()

    # Create interactive session instance.
    session = PyConsoleInteractiveSession(host, port)

    # Initialize anchor variable is specified.
    if args.anchor:
        session.send(args.anchor + " = str()")

    session.cmdloop()

    # Select dump text. If anchor variable is specified, use the contents of it,
    # otherwise, use the last output of the interactive session.
    if args.anchor: dump_text = session.send(args.anchor)
    else          : dump_text = session.received

    # Dump received string if arg.output was specified.
    # Otherwise, dump to STDOUT.
    if path_output: path_output.write_text(dump_text.strip())
    else          : print(dump_text.strip())


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
