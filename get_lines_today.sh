#!/bin/bash

echo "Today: "
# Get the current date in the format YYYY-MM-DD
current_date=$(date +"%Y-%m-%d")

# Find all commits from today and calculate added and deleted lines
git log --since="$current_date 00:00" --until="$current_date 23:59" --pretty=tformat: --numstat |
awk '{added+=$1; deleted+=$2} END {print "Lines added: " added "\nLines deleted: " deleted "\nTotal changes: " added + deleted}'

echo "Yesterday: "
# Get yesterday's date in the format YYYY-MM-DD
yesterday_date=$(date -d "yesterday" +"%Y-%m-%d")

# Find all commits from yesterday and calculate added and deleted lines
git log --since="$yesterday_date 00:00" --until="$yesterday_date 23:59" --pretty=tformat: --numstat |
awk '{added+=$1; deleted+=$2} END {print "Lines added: " added "\nLines deleted: " deleted "\nTotal changes: " added + deleted}'