#!/bin/bash

# Define installation directory
INSTALL_DIR=~/TBB_Lib

# Create installation directory if it doesn't exist
mkdir -p $INSTALL_DIR

# Navigate to installation directory
cd $INSTALL_DIR

# Download TBB library
wget https://github.com/oneapi-src/oneTBB/archive/refs/tags/v2021.5.0.tar.gz -O tbb.tar.gz

# Extract the tarball
tar -xzf tbb.tar.gz

# Remove the tarball after extraction
rm tbb.tar.gz

# Rename the extracted directory
mv oneTBB-2021.5.0 tbb

# Navigate to the TBB directory
cd tbb

# Create a build directory
mkdir build
cd build

# Configure the build with CMake, specify install prefix
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/tbb -DTBB_TEST=OFF ..

# Build the TBB library
cmake --build .

# Install the TBB library
cmake --install .

# Notify the user
echo "TBB library installed successfully in $INSTALL_DIR"

