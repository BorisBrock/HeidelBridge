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
    
    echo "#ifndef ${varname^^}_H" > "$headerfile"
    echo "#define ${varname^^}_H" >> "$headerfile"
    echo "" >> "$headerfile"
    echo "const unsigned char ${varname}[] PROGMEM = {" >> "$headerfile"
    xxd -i "$file" | sed "s/unsigned char/${varname}/g" >> "$headerfile"
    echo "};" >> "$headerfile"
    echo "" >> "$headerfile"
    echo "const unsigned int ${varname}_len = sizeof(${varname});" >> "$headerfile"
    echo "" >> "$headerfile"
    echo "#endif // ${varname^^}_H" >> "$headerfile"
done

echo "Conversion complete. Header files saved in '$OUTPUT_DIR'."
