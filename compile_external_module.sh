#!/bin/bash

# Directory containing the .c files
DIR="external_modules"

# Change into the directory
cd $DIR

# Loop over all .c files in the directory
for FILE in *.c; do
  # Get the filename without the extension
  BASENAME=$(basename "$FILE" .c)
  
  # Compile the .c file into a .so file
  gcc -shared -o "$BASENAME.so" "$FILE"
done

# Change back to the original directory
cd ..