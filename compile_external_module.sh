#!/bin/bash

# sudo apt install gcc make gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu

# Directory containing the .c files
DIR="external_modules"

# Change into the directory
cd "$DIR" || exit

# Loop over all .c files in the directory
for FILE in *.c; do
  # Get the filename without the extension
  BASENAME=$(basename "$FILE" .c)
  
  # Compile the .c file into a normal .so file
  gcc -shared -o "$BASENAME.so" "$FILE"

  # Compile the .c file into an AArch64 .so file
  aarch64-linux-gnu-gcc -shared -o "$BASENAME-aarch64.so" "$FILE"
done

# Change back to the original directory
cd .. || exit
