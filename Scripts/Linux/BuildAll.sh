#!/bin/bash

# Get the configuration parameter (default to debug)
CONFIGURATION="${1:-debug}"

# Convert to lowercase for make
CONFIG_LOWER=$(echo "$CONFIGURATION" | tr '[:upper:]' '[:lower:]')

# Generate the projects
pushd "$(dirname "$0")/.." > /dev/null
./Linux/GenerateProjects.sh gmake
popd > /dev/null

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
