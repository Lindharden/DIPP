#!/bin/bash

# Number of times to run the program
num_runs=100

# Command to run your program
command_to_run="./bin/binary 100000"

total_time=0

for ((i = 1; i <= num_runs; i++)); do
    # Measure the time and capture it
    result=$( { time $command_to_run; } 2>&1 )
    
    # Extract the real time (in seconds) and convert to milliseconds
    real_time=$(echo "$result" | grep real | awk '{print $2}')
    min=$(echo $real_time | cut -d'm' -f1)
    sec=$(echo $real_time | cut -d'm' -f2 | sed 's/s//')
    # Convert the minutes to seconds and then add the seconds, then convert to milliseconds
    total_milliseconds=$(echo "($min * 60 + $sec) * 1000" | bc -l)
    
    # Add the real time to the total
    total_time=$(echo "$total_time + $total_milliseconds" | bc -l)
done

# Calculate the average time
average_time=$(echo "$total_time / $num_runs" | bc -l)

printf "Average time over %d runs: %.2f milliseconds\n" $num_runs $average_time