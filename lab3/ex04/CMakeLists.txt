cmake_minimum_required(VERSION 3.10)

project(ex04 C)

include_directories(include)

set(SOURCE_FILES
  src/ex04.c
)

add_executable(ex04 ${SOURCE_FILES})
