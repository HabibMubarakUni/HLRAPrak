cmake_minimum_required(VERSION 3.12)
project(NBodySimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Suche nach SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Füge deine Quell- und Header-Dateien hinzu
add_executable(NBodySimulation
    NBody.cpp
    src/Body.cpp
)

# SFML-Module einbinden
target_link_libraries(NBodySimulation
    sfml-graphics
    sfml-window
    sfml-system
)

# Falls du Header in include/ hast
target_include_directories(NBodySimulation PRIVATE include)
