cmake_minimum_required(VERSION 3.10)
project(LightShell)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra")

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(lsh ${SOURCES})
