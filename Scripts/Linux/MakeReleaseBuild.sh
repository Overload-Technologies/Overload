#!/bin/bash

SCRIPT_DIR="$(dirname "$0")"
NO_OPEN=false

# Parse arguments
for arg in "$@"; do
    if [ "$arg" = "--no-open" ]; then
        NO_OPEN=true
    fi
done

# Generate projects
"$SCRIPT_DIR/GenerateProjects.sh"

# Build Debug
"$SCRIPT_DIR/Build.sh" debug -skip-project-generation
if [ $? -ne 0 ]; then
    echo "Debug build failed. Exiting."
    exit $?
fi

# Build Release
"$SCRIPT_DIR/Build.sh" release -skip-project-generation
if [ $? -ne 0 ]; then
    echo "Release build failed. Exiting."
    exit $?
fi

# Build Publish
"$SCRIPT_DIR/Build.sh" publish -skip-project-generation
if [ $? -ne 0 ]; then
    echo "Publish build failed. Exiting."
    exit $?
fi

# Read version from VERSION.txt
VERSION=$(cat "$SCRIPT_DIR/../../VERSION.txt")
PLATFORM="linux_x64"

# Navigate to the build folder
pushd "$SCRIPT_DIR/../../Build/" > /dev/null

# Delete any existing folder with the release name
if [ -d "Overload-$VERSION-$PLATFORM" ]; then
    rm -rf "Overload-$VERSION-$PLATFORM"
fi

# Create Releases folder if it doesn't exist
mkdir -p ../Releases

# Copy the Publish folder to a new folder
cp -r Publish "../Releases/Overload-$VERSION-$PLATFORM"

# Create the archive, delete any existing one first
if [ -f "../Releases/Overload-$VERSION-$PLATFORM.tar.gz" ]; then
    rm "../Releases/Overload-$VERSION-$PLATFORM.tar.gz"
fi

pushd ../Releases > /dev/null
tar -czf "Overload-$VERSION-$PLATFORM.tar.gz" "Overload-$VERSION-$PLATFORM"
echo "Archive created: Releases/Overload-$VERSION-$PLATFORM.tar.gz"
popd > /dev/null

# Delete temporary build
if [ -d "../Releases/Overload-$VERSION-$PLATFORM" ]; then
    rm -rf "../Releases/Overload-$VERSION-$PLATFORM"
    echo "Temporary build deleted."
fi

popd > /dev/null

# Open the output folder in the file manager (if available and not disabled)
if [ "$NO_OPEN" = false ] && command -v xdg-open &> /dev/null; then
    xdg-open "$SCRIPT_DIR/../../Releases"
fi
