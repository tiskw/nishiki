"""
Compile utility for NiShiKi.
"""

import argparse
import multiprocessing
import pathlib
import subprocess


# Define compile command.
CC: str = "g++ -std=c++23 -Wall -Wextra -Ofast -mtune=native -march=native"
CFLAGS: str = "-I./build/external"


def parse_args() -> argparse.Namespace:
    """
    Parse command line arguments.

    Returns:
        (argparse.Namespace): Parsed command line arguments.
    """
    parser = argparse.ArgumentParser(prog="compile.py", description="Compile NiShiKi", add_help=False)
    parser.add_argument("-i", "--input", metavar="STR", type=str, help="Input directory")
    parser.add_argument("-o", "--output", metavar="STR", type=str, help="Output directory")
    parser.add_argument("-b", "--batch", action="store_true", help="Enables batch compile")
    parser.add_argument("-s", "--separate", action="store_true", help="Enables separate compile")
    parser.add_argument("-h", "--help", action="help", help="Show this message and exit")
    return parser.parse_args()


def worker_compile(path_cxx: pathlib.Path, args: argparse.Namespace):
    """
    Compile one C++ source code.
    """
    path_obj = pathlib.Path(args.output) / path_cxx.with_suffix(".o").name
    message = f"\x1B[32m {path_cxx.name} \x1B[0m=>\x1B[34m {path_obj.name} \x1B[0m"

    if path_obj.exists() and (path_cxx.stat().st_mtime < path_obj.stat().st_mtime):
        print(message, "(already up to date, skipped)")
        return

    print(message)
    command = f"{CC} {CFLAGS} -I{args.input} -c -o {path_obj} {path_cxx}"
    return subprocess.run(command, shell=True).returncode


def main(args: argparse.Namespace):
    """
    Entrypoint of this script.
    """
    # Set default value of --input and --output options.
    if args.input is None:
        args.input = str(pathlib.Path(__file__).parent.parent / "source")
    if args.output is None:
        args.output = str(pathlib.Path(__file__).parent.parent / "build" / "objects")

    print("\x1b[38;5;140m////////////////////////////////////////////////////////////////////////////////\x1b[m")
    print("\x1b[38;5;140m// Start building NiShiKi\x1b[m")
    print("\x1b[38;5;140m////////////////////////////////////////////////////////////////////////////////\x1b[m")

    #
    with multiprocessing.Pool(multiprocessing.cpu_count() - 1) as pool:
        for path_cxx in sorted(pathlib.Path(args.input).glob("*.cxx")):
            pool.apply_async(worker_compile, args=(path_cxx, args))
        pool.close()
        pool.join()

    #
    path_out = pathlib.Path(args.output).parent / "nishiki"
    print("*.o =>", path_out)
    command = f"{CC} -o {path_out} {args.output}/*.o"
    subprocess.run(command, shell=True)


if __name__ == "__main__":
    main(parse_args())


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
