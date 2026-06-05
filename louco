#!/bin/bash

# Louro Code Generator (louco)
# Transpiles Louro scripts directly to C.

BUILD_TRANSPILER_ONLY=0
ENV_FILE=""
INPUT_FILE=""
OUTPUT_FILE=""

usage() {
    echo "Usage: $0 [-t] -e <env_file.h> [-o output] [input.txt]"
    echo "  -t : Build the transpiler executable only (do not run it)"
    echo "  -e : Environment header file defining LOURO_CUSTOM_ENV"
    echo "  -o : Output file (default: out.c, or louco_transpiler if -t is used)"
    exit 1
}

while getopts "te:o:h" opt; do
    case ${opt} in
        t ) BUILD_TRANSPILER_ONLY=1 ;;
        e ) ENV_FILE=$OPTARG ;;
        o ) OUTPUT_FILE=$OPTARG ;;
        h ) usage ;;
        \? ) usage ;;
    esac
done

shift $((OPTIND -1))
INPUT_FILE=$1

if [ -z "$ENV_FILE" ]; then
    usage
fi

if [ "$BUILD_TRANSPILER_ONLY" -eq 0 ] && [ -z "$INPUT_FILE" ]; then
    usage
fi

# Ensure paths are relative or absolute properly. We will just pass the raw ENV_FILE to GCC.
# It's better if we make ENV_FILE absolute so it's found when compiling from tools directory.
ENV_ABS=$(realpath "$ENV_FILE")

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TEMPLATE_C="$DIR/examples/template_aot.c"

if [ "$BUILD_TRANSPILER_ONLY" -eq 1 ]; then
    if [ -z "$OUTPUT_FILE" ]; then OUTPUT_FILE="louco_transpiler"; fi
    
    echo "[louco] Compiling Transpiler Executable..."
    gcc "$TEMPLATE_C" -DENV_HEADER="\"$ENV_ABS\"" -o "$OUTPUT_FILE" -lm
    
    if [ $? -ne 0 ]; then
        echo "Error building transpiler."
        exit 1
    fi
    echo "[louco] Success! Transpiler executable generated at: $OUTPUT_FILE"
    echo "[louco] You can now run it: ./$OUTPUT_FILE <input.txt>"
    exit 0
fi

if [ -z "$OUTPUT_FILE" ]; then OUTPUT_FILE="out.c"; fi

echo "[louco] Compiling AOT transpiler..."
gcc "$TEMPLATE_C" -DENV_HEADER="\"$ENV_ABS\"" -o "$DIR/tmp_aot_compiler" -lm

if [ $? -ne 0 ]; then
    echo "Error building AOT compiler."
    exit 1
fi

echo "[louco] Transpiling $INPUT_FILE..."
"$DIR/tmp_aot_compiler" "$INPUT_FILE" > "$OUTPUT_FILE"

if [ $? -ne 0 ]; then
    echo "Error transpiling."
    exit 1
fi

rm "$DIR/tmp_aot_compiler"
echo "[louco] Success! C code generated at: $OUTPUT_FILE"
