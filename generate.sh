#!/bin/bash

SCRIPT_FILE="$(cd "$(dirname "$BASH_SOURCE[0]")"; pwd)/$(basename "${BASH_SOURCE[0]}")"

SOURCES=(
    main.cpp
    lib.cpp
)

INCLUDE_DIRS=(
    ./
)

INCLUDE_FLAGS=()
for DIR in "${INCLUDE_DIRS[@]}"; do
    DIR="$(realpath "$DIR")"

    INCLUDE_FLAGS=("${INCLUDE_DIRS[@]}" "-I" "$DIR")
done

BINARY_DIR=$(realpath "./build")

GENERATED_SRC_DIR="${BINARY_DIR}/genetated_src"
rm -rf "$GENERATED_SRC_DIR"
mkdir -p "$GENERATED_SRC_DIR"

for SRC in "${SOURCES[@]}"; do 
    SRC="$(realpath "$SRC")"
    SRC_NAME="$(basename "$SRC")"

    # Generate Meta data
    GENERATE_META_DATA="${BINARY_DIR}/my-annotation-matcher"
    META_DATA_PATH="${GENERATED_SRC_DIR}/${SRC_NAME%.*}.meta.cpp"
    ${GENERATE_META_DATA} ${SRC} -out ${META_DATA_PATH} -- ${INCLUDE_FLAGS}
done

