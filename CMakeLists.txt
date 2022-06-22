cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project(polymorphism LANGUAGES CXX)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE 
    src/main.cpp
)