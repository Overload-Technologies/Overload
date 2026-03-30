#!/bin/bash

WORKSPACE_ROOT="$(dirname "$0")/../.."

BUILD_DIR="$WORKSPACE_ROOT/Build"
mkdir -p "$BUILD_DIR"

# If no argument is provided, configure using Unix Makefiles and Release by default
if [ $# -eq 0 ]; then
    echo "Configuring with Unix Makefiles (Release)"
    cmake -S "$WORKSPACE_ROOT" -B "$BUILD_DIR" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
else
    cmake -S "$WORKSPACE_ROOT" -B "$BUILD_DIR" "$@"
fi

pushd "$WORKSPACE_ROOT" > /dev/null
popd > /dev/null
