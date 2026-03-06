#!/bin/bash

WORKSPACE_ROOT="$(dirname "$0")/../.."

# If no argument is provided, default to gmake
if [ $# -eq 0 ]; then
    echo "No action specified. Defaulting to 'gmake'."
    set -- gmake
fi

pushd "$WORKSPACE_ROOT" > /dev/null
"./Dependencies/premake5/bin/premake5" "$@"
popd > /dev/null
