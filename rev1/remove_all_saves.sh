#!/bin/bash

# Traverse all subdirectories and delete files (excluding the root directory)
find . -mindepth 2 -type f -exec rm -f {} \;

echo "All files in subdirectories have been deleted!"