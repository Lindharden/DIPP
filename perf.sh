#!/bin/bash

# Path to your binary
BINARY="./bin/binary"

# Run the binary in the background and get its PID
$BINARY &
PID=$!

# Use perf to monitor the CPU usage of the binary
perf stat -p $PID -e cpu-clock,cache-misses,cache-references &

# Use top to monitor the memory usage of the binary
top -p $PID -b -n 1 | grep $PID

# Wait for the binary to finish
wait $PID
