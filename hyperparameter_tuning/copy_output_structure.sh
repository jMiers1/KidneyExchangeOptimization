#!/bin/bash

# Define the source and destination directories
src="out"
dest="json"

# Create the destination directory if it doesn't exist
mkdir -p "$dest"

# Copy the folder structure
find "$src" -type d -exec bash -c 'mkdir -p -- "$0/${1#*/}"' "$dest" {} \;

rmdir "$dest"/"$src"

echo "Folder structure copied from '$src' to '$dest'."
