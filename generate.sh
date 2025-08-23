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

for SRC in "${SOURCES[@]}"; do 
    SRC="$(realpath "$SRC")"

    # Generate Meta data
    GENERATE_META_DATA="./build/my-annotation-matcher"
    META_DATA_PATH="${SRC%.*}.meta.cpp"

    echo "${GENERATE_META_DATA} ${SRC} -out ${META_DATA_PATH} -- ${INCLUDE_FLAGS}"

    ${GENERATE_META_DATA} ${SRC} -out ${META_DATA_PATH} -- ${INCLUDE_FLAGS}
done