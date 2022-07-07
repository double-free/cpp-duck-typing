cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project(node LANGUAGES CXX)

Option(BUILD_TEST "Build unittest" OFF)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE 
    src/main.cpp
    src/node/Node.cpp
    src/mock/MockNodes.cpp
)

# unittests
if (BUILD_TEST)
    add_subdirectory("test")
endif()
