#!/bin/bash

# Exit immediately if any command fails
set -e

# Compile index.cpp
echo "Building index.cpp..."
g++ -std=c++23 -o index_service index.cpp -O3

# Compile index_service_test.cpp
echo "Building index_service_test.cpp..."
g++ -std=c++23 -o index_service_test index_service_test.cpp -O3

# Generate a temporary directory
TMP_DIR=$(mktemp -d ./tmp_index_test_XXXXXX)
echo "Temporary directory created: $TMP_DIR"

# Execute index_service
echo "Executing index_service..."
./index_service "$TMP_DIR" "$(realpath ./index_service_test)" &

# Process ID of index_service
INDEX_SERVICE_PID=$!

# Function to generate random files in the temporary directory
generate_random_files() {
    while true; do
        RANDOM_FILENAME=$(mktemp "$TMP_DIR/random_file_XXXXXX")
        echo "Created file: $RANDOM_FILENAME"
        sleep 1
    done
}

# Trap to clean up and terminate the script on interrupt or termination
cleanup() {
    echo "Cleaning up..."
    kill "$INDEX_SERVICE_PID" 2>/dev/null || true
    rm -rf "$TMP_DIR"
    echo "Temporary directory removed: $TMP_DIR"
    exit 0
}

trap cleanup SIGINT SIGTERM

# Start generating random files
generate_random_files
