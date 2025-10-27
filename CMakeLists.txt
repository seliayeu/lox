cmake_minimum_required(VERSION 3.31)
project(lox)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()
set(CMAKE_CXX_FLAGS "-Werror -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion")

add_executable(lox src/main.cpp 
    src/interpreter/Environment.cpp
    src/interpreter/Interpreter.cpp
    src/interpreter/LoxFunction.cpp
    src/interpreter/error.cpp
    src/interpreter/Scanner.cpp
    src/interpreter/Token.cpp)
