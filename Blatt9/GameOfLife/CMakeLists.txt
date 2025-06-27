cmake_minimum_required(VERSION 3.27)
project(GameOfLife LANGUAGES CXX CUDA) # Für Blatt 9 hier geändert

enable_language(CUDA) #vllt raus

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17) # Für Blatt 9 hinzugefügt
set(CMAKE_CUDA_STANDARD_REQUIRED ON) # Für Blatt 9 hinzugefügt
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp")

include_directories("includes")

add_executable(GameOfLife
  GameOfLife.cpp
  World.cpp
  cli.cpp
  World.cu # Für Blatt 9 hinzugefügt
)

# Diese Zeile fügt den save/ Ordner in den build-Ordner ein
file(COPY ${CMAKE_SOURCE_DIR}/save DESTINATION ${CMAKE_BINARY_DIR})
