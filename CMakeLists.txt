cmake_minimum_required(VERSION 3.31)
project(lox)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(lox src/main.cpp 
    src/interpreter/Environment.cpp
    src/interpreter/error.cpp
    src/interpreter/Scanner.cpp
    src/interpreter/Token.cpp)
