#!/bin/bash

# Get the current timestamp
timestamp=$(date +"%Y-%m-%d_%H-%M-%S")

# Find all subdirectories (skip the root directory)
find . -mindepth 1 -type d | while read -r dir; do
    dirname=$(basename "$dir")  # Extract the name of the directory
    touch "$dir/${dirname}_${timestamp}.txt"  # Create the file
    echo "Created file: $dir/${dirname}_${timestamp}.txt"
done

echo "Timestamped files created in all subdirectories!"
