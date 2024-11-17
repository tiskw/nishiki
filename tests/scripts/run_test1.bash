#!/bin/sh

# This is a test runner for NiShiKi, that is, call NiShiKi and type command
# some commands automatically. This script assumed to be called as a background
# process, and NiShiKi is called just after calling this script.

####################################################################################################
# Wait while NiShiKi test command found.
####################################################################################################

# Timeout of waiting NiShiKi test command.
TIMEOUT=30

# Count of waiting loop.
count=0

echo "Waiting for the nishiki-test..."

while [ "$count" -lt "$TIMEOUT" ]
do

    # Find NiShiKi test command.
    num_ps=`ps aux | grep nishiki-test | grep -v grep | wc -l`

    # If process found then exit the loop.
    if [ ${num_ps} -eq 1 ]
    then
        echo "process none"
        break
    fi

    sleep 1s
    ((count++))

done

# Find NiShiKi test command.
num_ps=`ps aux | grep nishiki-test | grep -v grep | wc -l`
if [ ${num_ps} -eq 0 ]
then
    exit 0
fi

####################################################################################################
# Run test commands.
####################################################################################################

sleep 1s

mkdir -p dummy
sleep 0.3s

# Basic commands.
xdotool type 'echo "NiShiKi"'
xdotool key Return
sleep 0.3s
xdotool type 'ls /dev'
xdotool key Return
sleep 0.3s

# Commented out.
xdotool type '# this is a comment'
xdotool key Return
sleep 0.3s

# Delete characters.
xdotool type 'echo "NiShiKi"'
xdotool key ctrl+h
xdotool key BackSpace
sleep 0.3s

# Cursor move and histries.
xdotool type 'echo "NiShiKi"'
xdotool key Up
xdotool key Down
xdotool key Left
xdotool key Left
xdotool key Right
xdotool key Right
sleep 0.3s

# Normal mode.
xdotool type 'echo "NiShiKi"'
xdotool key ctrl+bracketleft
xdotool type '0$hhlljkxS'
xdotool key ctrl+bracketleft
xdotool type '0a'
xdotool key ctrl+bracketleft
xdotool type 'A'
xdotool key ctrl+bracketleft
xdotool type 'I'
xdotool key Return
sleep 0.3s

# Filename completion.
xdotool type 'ls RE'
xdotool key Tab
xdotool key Return
xdotool type 'ls src/c'
xdotool key ctrl+i
xdotool key Return
sleep 0.3s

# Directory path completion.
xdotool type 'ls ext'
xdotool key Tab
xdotool key Return
sleep 0.3s

# Command completion.
xdotool type 'lsl'
xdotool key Tab
xdotool key Return
sleep 0.3s

# History completion.
xdotool type 'ls '
xdotool key ctrl+n 
xdotool key Return
sleep 0.3s

# Option completion.
xdotool type 'find --help '
xdotool key Return
sleep 0.3s

# Subcommand completion.
xdotool type 'git status '
xdotool key Return
sleep 0.3s

# Shell completions.
xdotool type 'git branch '
xdotool key Return
sleep 0.3s

# Preview completion.
xdotool type 'ls src/main.cxx '
xdotool key Return
sleep 0.3s
xdotool type 'ls LICENSE '
xdotool key Return
sleep 0.3s
xdotool type 'ls DUMMY '
xdotool key Return
sleep 0.3s

# Alias command.
xdotool type 'alias'
xdotool key Return
sleep 0.3s

# Test file chooser 1/3.
xdotool type 'ls '
xdotool key ctrl+f
sleep 0.3s
xdotool type 'jjjjjjjjkkkkkkkkhl-l'
xdotool key ctrl+f
xdotool key ctrl+b
xdotool key Up
xdotool key Down
xdotool type 'Gl0  '
xdotool type '/LICENSE'
xdotool key BackSpace
xdotool key ctrl+h
xdotool key Return
xdotool key Return
sleep 0.3s
xdotool key Return
sleep 0.3s

# Test file chooser 2/3.
xdotool type 'ls '
xdotool key ctrl+f
sleep 0.3s
xdotool type ctrl+f
xdotool type ctrl+b
xdotool type 'hhhhhhhhhhhhhhhhhhhh0'
xdotool type '/lib'
xdotool key Return
xdotool type 'lG'
xdotool type 'q'
sleep 0.3s
xdotool key Return

# Test file chooser 3/3.
xdotool type 'cd tests/dummy'
xdotool key Return
sleep 0.3s
xdotool key ctrl+f
sleep 0.3s
xdotool key Return
sleep 0.3s
xdotool key Return
sleep 0.3s
xdotool type 'cd ../..'
xdotool key Return
sleep 0.3s

# Test hist chooser.
xdotool type 'echo '
xdotool key ctrl+u
sleep 0.3s
xdotool type ctrl+f
xdotool type ctrl+b
xdotool type 'jjjjjjjjkkkkkkkkhl'
xdotool key Up
xdotool key Down
xdotool type '/README.md'
xdotool key BackSpace
xdotool key ctrl+h
xdotool key Return
xdotool key Return
sleep 0.3s
xdotool key Return
sleep 0.3s

# Test process chooser 1/2.
xdotool type 'ps '
xdotool key ctrl+p
sleep 0.3s
xdotool type 'jjjjjjjjkkkkkkkkhl'
xdotool key Up
xdotool key Down
xdotool type '/chrome'
xdotool key ctrl+h
xdotool key BackSpace
xdotool type 'me'
xdotool key Return
xdotool key ctrl+f
xdotool key ctrl+b
xdotool type 'q'
sleep 0.3s
xdotool key Return
sleep 0.3s

# Test process chooser 2/2.
xdotool type 'ps '
xdotool key ctrl+p
sleep 0.3s
xdotool type '   '
xdotool key Return
sleep 0.3s
xdotool key Return
sleep 0.3s

# Test external command.
xdotool type '# '
xdotool key ctrl+y
xdotool key Return
sleep 0.3s
xdotool key ctrl+w
xdotool key Return
sleep 0.3s
xdotool key ctrl+g
xdotool key Return
sleep 0.3s

# Incorrect NiShiKi-special command 1/2.
xdotool type ':NISHIKI:unknown:NISHIKI:lhs:NISHIKI:rhs:NISHIKI:command'
xdotool key Return
sleep 0.3s

# Incorrect NiShiKi-special command 2/2.
xdotool type ':NISHIKI::NISHIKI:'
xdotool key Return
sleep 0.3s

# Change directory command.
xdotool type 'cd external'
xdotool key Return
xdotool type 'cd ~'
xdotool key Return
xdotool type 'cd not_existing_directory'
xdotool key Return
sleep 0.3s

# Set command.
xdotool type 'set'
xdotool key Return
sleep 0.3s
xdotool type 'set -x hoge HOGE'
xdotool key Return
sleep 0.3s
xdotool type 'set -e hoge'
xdotool key Return
sleep 0.3s

# Exit NiSHiKi.
sleep 1s
xdotool key ctrl+d

rmdir dummy

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
