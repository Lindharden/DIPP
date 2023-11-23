#!/bin/bash

BINARY="./bin/binary"
CSV_FILE="perf-benchmark.csv"

# Write CSV header
echo "Iteration,cpu-clock,context-switches,cache-misses,cache-references,cache-miss-rate,peak-memory" > $CSV_FILE

for i in $(seq 1 100)
do
    # Run perf stat and capture output
    perf_output=$(perf stat -e cpu-clock,context-switches,cache-misses,cache-references -a $BINARY $i 2>&1)

    # Extract measurements from perf output
    cpu_clock=$(echo "$perf_output" | grep 'cpu-clock' | awk '{print $1}')
    context_switches=$(echo "$perf_output" | grep 'context-switches' | awk '{print $1}')
    cache_misses=$(echo "$perf_output" | grep 'cache-misses' | awk '{print $1}')
    cache_references=$(echo "$perf_output" | grep 'cache-references' | awk '{print $1}')

    # Calculate cache miss rate
    cache_miss_rate=$(echo "scale=2; $cache_misses / $cache_references * 100" | bc)

    # Run binary with time command and capture peak memory usage
    peak_memory=$(/usr/bin/time -f "%M" $BINARY $i 2>&1 | tail -n 1)

    # Write measurements to CSV file
    echo "$i,$cpu_clock,$context_switches,$cache_misses,$cache_references,$cache_miss_rate,$peak_memory" >> $CSV_FILE
done
