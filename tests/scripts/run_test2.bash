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

# Basic commands.
xdotool type 'echo "NiShiKi"'
xdotool key Return
sleep 0.2s
xdotool type 'ls /dev'
xdotool key Return
sleep 0.2s

# Ctrl-C (SIGINT).
ps_id=`ps aux | grep nishiki-test | grep -v grep | tr -s ' ' | cut -d ' ' -f 2`
kill -2 ${ps_id}

# Exit NiSHiKi.
xdotool key ctrl+d

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
