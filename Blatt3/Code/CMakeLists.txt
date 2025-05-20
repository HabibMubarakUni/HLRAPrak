cmake_minimum_required(VERSION 3.20)
project(HPCA_PC_MLP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include_directories(include)
add_executable(HPCA_PC_MLP main.cpp
        src/MLPHandler.cpp
        src/Utils.cpp
        )
