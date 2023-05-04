#!/usr/bin/env python3

"""
Script file to build C/C++ project. This script

1. search all C/C++ source file under the source directory (specified by --src option)
2. compile all detected C/C++ source file to object file and dump them
   under the output directory (specified by --out option)
3. link all object files and generate the target file (specified by --target option)
"""

import argparse
import pathlib
import re
import subprocess


SUFFIXES_C = [".c", ".cxx", ".cc", ".cpp"]
SUFFIXES_H = [".h", ".hxx", ".hh", ".hpp"]


def parse_args():
    """
    Parse command line arguments.
    """
    parser = argparse.ArgumentParser(description="Build C/C++ project")
    parser.add_argument("--cc", choices=["clang", "gcc"], default="clang", help="compiler selection")
    parser.add_argument("--gcov", action="store_true", default=False, help="compile with gcov options")
    parser.add_argument("--link", type=str, default="", help="linked libraries (comma separated)")
    parser.add_argument("--out", type=str, default="", help="output directory")
    parser.add_argument("--src", type=str, default="", help="source directory")
    parser.add_argument("--target", type=str, default="", help="target file name")
    return parser.parse_args()


def c_to_obj_file(path_c, path_src, path_out):
    """
    Create file path of object files that correspond to the given C file paths.

    Args:
        list_path_c (pathlib.Path): List of C/C++ source paths.
        path_src    (pathlib.Path): Path to source directory.
        path_out    (pathlib.Path): Path to outout directory.

    Returns:
        (pathlib.Path): Path to object file.
    """
    path_r = path_c.relative_to(path_src)
    path_o = path_out / path_r.with_suffix(".o")
    return path_o


def mtime(path):
    """
    Returns modiry time of the given file.

    Args:
        path (pathlib.Path): Target file path.

    Returns:
        (float): "mtime" of the target file.
    """
    if path.exists(): return path.stat().st_mtime
    else            : return 0.0


def build(source_files, object_files, args):
    """
    Build source files.

    Args:
        source_files (list)              : List of source files.
        object_files (list)              : List of object files.
        args         (argparse.Namespace): Parsed command line arguments.
    """
    # Define path to the output directory.
    path_out = pathlib.Path(args.out)

    # Number of C/C++ source files.
    n_files = len(source_files)

    # Select compiler.
    if   args.cc == "clang": CC = "clang++ -stdlib=libc++"
    elif args.cc == "gcc"  : CC = "g++"
    else                   : CC = "g++"

    # Add extra options.
    CC += " -std=c++17 -Wall -Wextra -O3 -I./src -I./external -I."

    # Add gcov related options if --gcov is specified.
    if args.gcov:
        CC += " -fprofile-arcs -ftest-coverage"

    for index, (path_c, path_o) in enumerate(zip(source_files, object_files)):

        needs_build = False

        if mtime(path_c) > mtime(path_o):
            needs_build = True

        for suffix_h in SUFFIXES_H:
            if mtime(path_c.with_suffix(suffix_h)) > mtime(path_o):
                needs_build = True

        if needs_build:
            cmd = f"{CC} -c -o {path_o} {path_c}"
            print(f"\033[32mRUN[{index:02d}/{n_files:02d}]:\033[34m {cmd}\033[m")
            ret = subprocess.run(cmd, shell=True)

            # Stop build process if some error occurred.
            if ret.returncode != 0:
                exit()

        else:
            print(f"\033[32mRUN[{index:02d}/{n_files:02d}]:\033[34m Object file is latest ({path_o})\033[m")

    lnk = ["-l" + link for link in args.link.split(",") if link]
    cmd = f"{CC} -o {path_out / args.target} " + str(path_out / "*.o") + " " + " ".join(lnk)
    print(f"\033[32mRUN[{n_files}/{n_files}]:\033[34m {cmd}\033[m")
    subprocess.run(cmd, shell=True)


def main(args):
    """
    Build project.

    Args:
        args (argparse.Namespace): Parsed command line arguments
    """
    path_src = pathlib.Path(args.src)
    path_out = pathlib.Path(args.out)

    source_files = [path for path in sorted(path_src.glob("**/*.*")) if path.suffix in SUFFIXES_C]
    object_files = [c_to_obj_file(path_c, path_src, path_out) for path_c in source_files]

    path_out.mkdir(parents=True, exist_ok=True)

    build(source_files, object_files, args)


if __name__ == "__main__":
    main(parse_args())


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
