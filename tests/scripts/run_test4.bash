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
    if [ ${num_ps} -ge 1 ]
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

# Exit filechooser/textchooser.
xdotool type 'q'
sleep 0.3s

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
