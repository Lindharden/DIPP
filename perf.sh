#!/bin/bash

# Path to your binary
BINARY="./bin/binary"
echo $BINARY

# Use perf to monitor the CPU usage of the binary
perf stat -e cpu-clock,cache-misses,cache-references -a $BINARY

# Execute the binary file using GNU time and store the output in a variable
output=$(/usr/bin/time -f "%M" $BINARY 2>&1)

# Print the output of the binary file
echo "$output"

# Extract the peak memory usage from the last line of the output
peak_memory=$(echo "$output" | tail -n 1)

# Print the peak memory usage in kilobytes
echo "Peak memory usage: $peak_memory KB"