#!/bin/bash

BINARY="./bin/binary"
CSV_FILE="perf-benchmark.csv"
NUM_ITERATIONS=50

# Write CSV header
echo "Iteration,avg-cpu-clock,avg-context-switches,avg-cache-misses,avg-cache-references,avg-cache-miss-rate,avg-peak-memory" > $CSV_FILE

for i in $(seq 1 100)
do
    echo "iteration" $i
    # Initialize sum variables
    sum_cpu_clock=0
    sum_context_switches=0
    sum_cache_misses=0
    sum_cache_references=0
    sum_peak_memory=0

    for j in $(seq 1 $NUM_ITERATIONS)
    do
        # Run perf stat and capture output
        perf_output=$(perf stat -e cpu-clock,context-switches,cache-misses,cache-references -a $BINARY $i 2>&1)

        # Extract measurements from perf output
        cpu_clock=$(echo "$perf_output" | grep 'cpu-clock' | awk '{print $1}')
        context_switches=$(echo "$perf_output" | grep 'context-switches' | awk '{print $1}')
        cache_misses=$(echo "$perf_output" | grep 'cache-misses' | awk '{print $1}' | tr -d ',')
        cache_references=$(echo "$perf_output" | grep 'cache-references' | awk '{print $1}' | tr -d ',')

        # Run binary with time command and capture peak memory usage
        peak_memory=$(/usr/bin/time -f "%M" $BINARY $i 2>&1 | tail -n 1)

        # Add measurements to sum variables
        sum_cpu_clock=$(echo "$sum_cpu_clock + $cpu_clock" | bc)
        sum_context_switches=$(echo "$sum_context_switches + $context_switches" | bc)
        sum_cache_misses=$(echo "$sum_cache_misses + $cache_misses" | bc)
        sum_cache_references=$(echo "$sum_cache_references + $cache_references" | bc)
        sum_peak_memory=$(echo "$sum_peak_memory + $peak_memory" | bc)
    done

    # Calculate averages
    avg_cpu_clock=$(echo "scale=2; $sum_cpu_clock / $NUM_ITERATIONS" | bc)
    avg_context_switches=$(echo "scale=2; $sum_context_switches / $NUM_ITERATIONS" | bc)
    avg_cache_misses=$(echo "scale=2; $sum_cache_misses / $NUM_ITERATIONS" | bc)
    avg_cache_references=$(echo "scale=2; $sum_cache_references / $NUM_ITERATIONS" | bc)
    avg_peak_memory=$(echo "scale=2; $sum_peak_memory / $NUM_ITERATIONS" | bc)

    # Calculate average cache miss rate
    avg_cache_miss_rate=$(echo "scale=10; $avg_cache_misses / $avg_cache_references * 100" | bc)

    # Write averages to CSV file
    echo "$i,$avg_cpu_clock,$avg_context_switches,$avg_cache_misses,$avg_cache_references,$avg_cache_miss_rate,$avg_peak_memory"
    echo " "
    echo "$i,$avg_cpu_clock,$avg_context_switches,$avg_cache_misses,$avg_cache_references,$avg_cache_miss_rate,$avg_peak_memory" >> $CSV_FILE
done
