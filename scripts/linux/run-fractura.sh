#!/bin/bash
# ============================================================================
# Caelis - Run Fractura (Linux)
# Author: Tim Gatzke <post@tim-gatzke.de>
# ============================================================================

# Detect CI environment
IS_CI=0
if [[ "$CI" == "1" || "$CI" == "true" ]]; then
    IS_CI=1
elif [[ "$GITHUB_ACTIONS" == "true" ]]; then
    IS_CI=1
elif [[ "$TRAVIS" == "true" ]]; then
    IS_CI=1
elif [[ "$CIRCLECI" == "true" ]]; then
    IS_CI=1
elif [[ "$BUILDKITE" == "true" ]]; then
    IS_CI=1
elif [[ -n "$JENKINS_URL" ]]; then
    IS_CI=1
fi

BUILD_TYPE=${1:-Release}
EXECUTABLE_PATH="build/bin/Fractura"

echo ""
echo "============================================================================"
echo "  Running Fractura ($BUILD_TYPE)"
echo "============================================================================"

if [ ! -f "$EXECUTABLE_PATH" ]; then
    echo "ERROR: Fractura executable not found at $EXECUTABLE_PATH"
    echo "Please build the project first using build.sh"
    exit 1
fi

if [[ $IS_CI -eq 1 ]]; then
    echo "Skipping Fractura execution in CI environment"
    echo "Executable exists at: $EXECUTABLE_PATH"
    exit 0
fi

echo "Starting Fractura..."
echo "Working directory: $(pwd)"
echo "Executable: $EXECUTABLE_PATH"
echo ""

cd build/bin
./Fractura
cd ../..

echo ""
echo "Fractura has exited."