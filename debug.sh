#!/bin/bash
st-util --no-reset &
sleep 1
SUBP=$(echo $!)
arm-none-eabi-gdb -x gdb.cmd
kill $SUBP
