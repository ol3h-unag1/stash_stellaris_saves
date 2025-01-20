# Get the current date in YYYY-MM-DD format
current_date=$(date +%F)

# Get the number of lines added to the repository today
lines_added=$(git log --since="$current_date 00:00" --until="$current_date 23:59" --pretty=tformat: --numstat | awk '{added += $1} END {print added}')
clear
echo "Number of lines pushed to the repository today: $lines_added"

# Get the date for yesterday in YYYY-MM-DD format
#yesterday_date=$(date -d "yesterday" +%F)

# Get the number of lines added to the repository yesterday
#lines_added_yesterday=$(git log --since="$yesterday_date 00:00" --until="$yesterday_date 23:59" --pretty=tformat: --numstat | awk '{added += $1} END {print added}')

#echo "Number of lines pushed to the repository yesterday: $lines_added_yesterday"