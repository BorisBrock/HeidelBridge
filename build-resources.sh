#!/bin/bash

# Hardcoded input directory
INPUT_DIR="./data"
OUTPUT_DIR="$INPUT_DIR/headers"

# Ensure the directory exists
if [ ! -d "$INPUT_DIR" ]; then
    echo "Error: Directory '$INPUT_DIR' not found."
    exit 1
fi

# Create the output directory for header files
mkdir -p "$OUTPUT_DIR"

# Process each file in the directory (excluding already converted .h files)
find "$INPUT_DIR" -type f ! -name "*.h" | while read -r file; do
    filename=$(basename -- "$file")
    varname=$(echo "$filename" | tr '.-' '_')  # Convert dots and hyphens to underscores
    headerfile="$OUTPUT_DIR/${filename}.h"

    echo "Converting $file -> $headerfile"
    
    echo "#pragma once" > "$headerfile"
    echo "" >> "$headerfile"
    echo "const unsigned char ${varname}[] PROGMEM = {" >> "$headerfile"
    # Generate only the hex values without variable declaration
    xxd -i "$file" | sed -E '/unsigned char|unsigned int/d' >> "$headerfile"
    echo "" >> "$headerfile"
    echo "const unsigned int ${varname}_len = sizeof(${varname});" >> "$headerfile"
done

echo "Conversion complete. Header files saved in '$OUTPUT_DIR'."
