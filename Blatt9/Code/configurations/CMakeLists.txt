cmake_minimum_required(VERSION 3.27)
project(GameOfLife)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include_directories("../includes")

add_executable(GameOfLife
  ../GameOfLife.cpp
  ../World.cpp
  ../cli.cpp
)

# Diese Zeile fügt den save/ Ordner in den build-Ordner ein
file(COPY ${CMAKE_SOURCE_DIR}/../save DESTINATION ${CMAKE_BINARY_DIR})
