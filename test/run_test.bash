#!/bin/sh

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

# Basic commands.
xdotool type 'echo "NiShiKi"'
xdotool key Return
sleep 0.1s

# Delete characters.
xdotool type 'echo "NiShiKi"'
xdotool key ctrl+h
xdotool key Backspace
sleep 0.1s

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
sleep 0.1s

# Filename completion.
xdotool type 'ls RE'
xdotool key Tab
xdotool key Return
xdotool type 'ls src/c'
xdotool key ctrl+i
xdotool key Return
sleep 0.1s

# Directory path completion.
xdotool type 'ls ext'
xdotool key Tab
xdotool key Return
sleep 0.1s

# History completion.
xdotool type 'ls '
xdotool key ctrl+n 
xdotool key Return
sleep 0.1s

# Option completion.
xdotool type 'find --help '
xdotool key Return
sleep 0.1s

# Subcommand completion.
xdotool type 'git status '
xdotool key Return
sleep 0.1s

# Shell completions.
xdotool type 'git branch '
xdotool key Return
sleep 0.1s

# Preview completion.
xdotool type 'ls src/main.cxx '
xdotool key Return
xdotool type 'ls LICENSE '
xdotool key Return
sleep 0.1s

# Alias command.
xdotool type 'alias'
xdotool key Return
sleep 0.1s

# Test file chooser 1/2.
xdotool type 'ls '
xdotool key ctrl+f
xdotool type 'jjjjjjjjkkkkkkkkhl'
xdotool type '/LIC'
xdotool key Return
xdotool key Return
sleep 0.2s
xdotool key Return
sleep 0.2s

# Test process chooser 1/2.
xdotool type 'ps '
xdotool key ctrl+p
xdotool type 'jjjjjjjjkkkkkkkkhl'
xdotool type '/chrome'
xdotool key ctrl+h
xdotool key Backspace
xdotool type 'me'
xdotool key Return
xdotool key ctrl+f
xdotool key ctrl+b
xdotool key Return
sleep 0.2s
xdotool key Return
sleep 0.2s

# Test process chooser 2/2.
xdotool type 'ps '
xdotool key ctrl+p
xdotool type '   '
xdotool key Return
sleep 0.2s
xdotool key Return
sleep 0.2s

# Incorrect NiShiKi-special command 1/2.
xdotool type ':NISHIKI:unknown:NISHIKI:lhs:NISHIKI:rhs:NISHIKI:command'
xdotool key Return
sleep 0.2s

# Incorrect NiShiKi-special command 2/2.
xdotool type ':NISHIKI::NISHIKI:'
xdotool key Return
sleep 0.2s

# Change directory command.
xdotool type 'cd external'
xdotool key Return
xdotool type 'cd ~'
xdotool key Return
xdotool type 'cd not_existing_directory'
xdotool key Return
sleep 0.1s

# Exit NiSHiKi.
xdotool key ctrl+d

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
