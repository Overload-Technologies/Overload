#!/bin/bash

SKIP_PROJECT_GENERATION=false

# Parse arguments
for arg in "$@"; do
    case $arg in
        -skip-project-generation)
            SKIP_PROJECT_GENERATION=true
            ;;
        *)
            CONFIGURATION="$arg"
            ;;
    esac
done

CONFIGURATION="${CONFIGURATION:-debug}"

# Convert to lowercase for make
CONFIG_LOWER=$(echo "$CONFIGURATION" | tr '[:upper:]' '[:lower:]')

# Generate project files
if [ "$SKIP_PROJECT_GENERATION" != "true" ]; then
    pushd "$(dirname "$0")" > /dev/null
    ./GenerateProjects.sh
    popd > /dev/null
fi

# Build the solution
echo "Building Overload in $CONFIGURATION mode..."

pushd "$(dirname "$0")/../.." > /dev/null
make config=${CONFIG_LOWER}_x64 -j$(nproc)
BUILD_RESULT=$?
popd > /dev/null

if [ $BUILD_RESULT -ne 0 ]; then
    echo "Build failed."
    exit $BUILD_RESULT
fi

echo "Build completed successfully."
