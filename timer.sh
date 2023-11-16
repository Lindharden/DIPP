#!/bin/bash

# Make the result file
rm results.csv
touch results.csv
echo "module_num,time_per_module" >> results.csv

# Number of times to run the program
num_runs=100

# Max number of modules to test
max_modules=100

# Digits of Pi to calculate
pi_digits=5000000

for ((i = 1; i <= max_modules; i++)); do
    total_time=0
    for ((j = 1; j <= num_runs; j++)); do
        # Capture start time
        start_time=$(date +%s%N)

        # Run the program
        "./bin/binary" "$pi_digits" "$i" > /dev/null 2>&1

        # Capture end time
        end_time=$(date +%s%N)

        # Calculate the elapsed time in milliseconds
        elapsed_time=$(( (end_time - start_time) / 1000000 ))

        # Add the elapsed time to the total
        total_time=$((total_time + elapsed_time))
    done

    # Calculate the average time
    average_time=$(echo "$total_time / $num_runs / $i" | bc -l)
    printf "Average time per module over %d runs for %d modules: %.2f milliseconds\n" $num_runs $i $average_time

    echo "$i,$average_time" >> results.csv

done

