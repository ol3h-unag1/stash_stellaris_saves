# Get the current date in YYYY-MM-DD format
current_date=$(date +%F)

# Get the number of lines added and removed to the repository today
git_log_output=$(git log --since="$current_date 00:00" --until="$current_date 23:59" --pretty=tformat: --numstat)
lines_added=$(echo "$git_log_output" | awk '{added += $1} END {print added}')
lines_removed=$(echo "$git_log_output" | awk '{removed += $2} END {print removed}')
lines_changed=$(echo "$git_log_output" | awk '{changed += $1 + $2} END {print changed}')

clear
echo "Number of lines added to the repository today: $lines_added"
echo "Number of lines removed from the repository today: $lines_removed"
echo "Total number of lines changed in the repository today: $lines_changed"
current_date=$(date +%F)