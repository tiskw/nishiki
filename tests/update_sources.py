#!/usr/bin/env python3

"""
Script file to copy C/C++ source and header files for test.
"""

import argparse
import pathlib
import shutil


SUFFIXES_C = [".c", ".cxx", ".cc", ".cpp"]
SUFFIXES_H = [".h", ".hxx", ".hh", ".hpp"]


def parse_args():
    """
    Parse command line arguments.
    """
    parser = argparse.ArgumentParser(description="Copy C/C++ sources and headers")
    parser.add_argument("--out", type=str, default=None, help="output directory")
    parser.add_argument("--src", type=str, default=None, help="source directory")
    return parser.parse_args()


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


def main(args):
    """
    Build project.

    Args:
        args (argparse.Namespace): Parsed command line arguments
    """
    if (args.out is None) or (args.src is None):
        exit("Please specify --out and --src.")

    path_src = pathlib.Path(args.src)
    path_out = pathlib.Path(args.out)

    # Detect all files to be copied.
    source_files  = [path for path in sorted(path_src.glob("**/*.*")) if path.suffix in SUFFIXES_C]
    source_files += [path for path in sorted(path_src.glob("**/*.*")) if path.suffix in SUFFIXES_H]
    source_files  = sorted(source_files)

    # Create output directory if not exists.
    path_out.mkdir(parents=True, exist_ok=True)

    for path_from in source_files:

        # Compute output file path.
        path_to = path_out / path_from.name

        if mtime(path_from) > mtime(path_to):
            shutil.copyfile(path_from, path_to)

    # Copy C source/header files for unittest.
    shutil.copyfile("unittest/unittest.cxx", path_out / "unittest.cxx")
    shutil.copyfile("unittest/unittest.hxx", path_out / "unittest.hxx")

    # Copy main file.
    shutil.copyfile("unittest/main_with_unittest.cxx", path_out / "main.cxx")


if __name__ == "__main__":
    main(parse_args())


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
