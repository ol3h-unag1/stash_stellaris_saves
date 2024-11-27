#!/bin/bash

# Echo the decorative line
echo "Easing in...\n† ═╬═ 7 ═╬═ 7 ═╬═ †"

# Compile the C++ program
g++ -std=c++23 -o stash stash.cpp -O3

# Create auto-save files
for i in {1..5}; do
    filename=$(printf "autosave%03d.sav" $i)
    touch "$filename"
    game_state=$((i * i - 1))
    time_stampt=$(date '+%Y-%m-%d %H:%M:%S')
    echo "game_state=$game_state - time_stampt=$time_stampt" > "$filename"
done

# Ask user how many user saves to create
read -p "How many user saves would you like to create? " num_user_saves

# Create user save files
for ((i = 1; i <= num_user_saves; i++)); do
    filename=$(printf "usersave%03d.sav" $i)
    touch "$filename"
    game_state=$((i * i - 1))
    time_stampt=$(date '+%Y-%m-%d %H:%M:%S')
    echo "game_state=$game_state - time_stampt=$time_stampt" > "$filename"
done

# Report on file creations
echo "Files created:"
ls

# Run the compiled program
if [[ -f "./stash" ]]; then
    ./stash
else
    echo "Error: Compiled program './stash' not found."
fi

echo "Stashed files origin directory:"
ls

echo "Folder added to: ~/Stellaris/Saves/test_lab/:"
ls ~/Stellaris/Saves/test_lab/

echo "End of things...\n ⸸ ═╬═ 7 ═ ⸹⸹⸹ ╬ ⸹⸹⸹ ═ 7 ═╬═ ⸸"