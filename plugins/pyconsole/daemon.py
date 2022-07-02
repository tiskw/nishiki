#!/usr/bin/env python3
# coding: utf-8


import socket
import socketserver
import traceback
import subprocess


def sh(arg):
    """
    Execute command in a background shell.

    Args:
        arg (str or list): shell command, or a list of shell commands.
    """
    if isinstance(arg, list):
        return [sh(a) for a in arg]
    else:
        return subprocess.check_output(arg, shell=True).decode("utf-8").strip()


def execute(code):
    """
    Execute the given Python code.
    """
    try   : return eval(code, globals())
    except: return exec(code, globals())


class PyConsoleTCPServer(socketserver.TCPServer):
    """
    Custom class for the TCPServer.
    """
    def server_bind(self):
        """
        Override the superclass's method for avoiding the error "Address already in use".
        """
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.bind(self.server_address)


class PyConsoleTCPHandler(socketserver.StreamRequestHandler):
    """
    Custom class for the StreamRequestHandler.
    """
    def handle(self):
        """
        Handle received data.
        """
        self.data_in = self.rfile.readline().decode().strip()

        if self.data_in == "daemon:kill":
            exit()

        try   : self.data_out = execute(self.data_in)
        except: self.data_out = traceback.format_exc()

        output = str(self.data_out) if self.data_out else ""
        self.wfile.write((output.strip() + "\n").encode())


def main_daemon(args):
    """
    Main function in daemon mode.
    """
    host = str(args.host)
    port = int(args.port)

    # Create the server, binding to localhost on port 9999
    with PyConsoleTCPServer((host, port), PyConsoleTCPHandler) as server:
        server.serve_forever()


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
