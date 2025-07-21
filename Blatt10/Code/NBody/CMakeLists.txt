cmake_minimum_required(VERSION 3.12)
project(NBodySimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Suche nach SFML
# find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

set(SFML_ROOT "$ENV{HOME}/SFML")
include_directories(${SFML_ROOT}/include)
include_directories(include)
link_directories(${SFML_ROOT}/lib64)

# Füge deine Quell- und Header-Dateien hinzu
add_executable(NBodySimulation
    NBody.cpp
    src/Body.cpp
)

# SFML-Module einbinden
target_link_libraries(NBodySimulation
    OpenCL
    sfml-graphics
    sfml-window
    sfml-system
)

set_target_properties(NBodySimulation PROPERTIES BUILD_RPATH ${SFML_ROOT}/lib64)