cmake_minimum_required(VERSION 3.10)
project(NBodySimulation)

set(CMAKE_CXX_STANDARD 17)

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
