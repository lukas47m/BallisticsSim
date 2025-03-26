#!/bin/bash

# Exit on error
set -e

# Create build directory if it doesn't exist
mkdir -p output

# Navigate to build directory
cd output

# Configure with CMake
cmake ..

# Build the project
make

# Run the executable
./SimulationTest

echo "Program executed successfully!" 