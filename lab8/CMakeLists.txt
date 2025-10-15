
cmake_minimum_required(VERSION 3.10)

project(ex01 C)


set(SOURCE_FILES
  src/ex01.c
)

add_executable(ex01 ${SOURCE_FILES})

target_link_libraries(ex01 m)
