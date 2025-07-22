cmake_minimum_required(VERSION 3.12)
project(MatMulTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Füge deine Quell- und Header-Dateien hinzu
add_executable(MatMulTest
    MatMulTest.cpp
)

# SFML-Module einbinden
target_link_libraries(MatMulTest
    OpenCL
)