#!/bin/sh

# Help and version.
./run_nishiki --help
./run_nishiki --version

# Run a normal command.
./run_nishiki --input "ls\n#ls\nexit\n"

# Try empty input.
./run_nishiki --input "\nexit\n"

# Try alias command.
./run_nishiki --input "alias\nexit\n"

# Try cd command.
./run_nishiki --input "cd\ncd ..\ncd ~\ncd hoge\nexit\n"

# Try set command.
./run_nishiki --input "set\nset -x HOGE hoge\nset -e HOGE\nexit\n"

# Try file chooser.
./run_nishiki --input "!chooser --lhs 'echo ' --rhs '' --mode file\n\nexit\n"

# Erroneous command line option.
./run_nishiki --erroneous_option

# Try plugins.
./run_nishiki --input "!welcome\nexit\n"

# Manual user input.
./run_nishiki --input "exit"

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
