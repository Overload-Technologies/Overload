#!/bin/bash

# Get the action parameter (default to gmake)
ACTION="${1:-gmake}"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PREMAKE_PATH="$SCRIPT_DIR/../../Dependencies/premake5/bin/premake5"

pushd "$SCRIPT_DIR/../.." > /dev/null
"$PREMAKE_PATH" "$ACTION"
popd > /dev/null
