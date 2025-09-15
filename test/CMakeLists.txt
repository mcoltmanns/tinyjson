cmake_minimum_required(VERSION 4.0)

project(tinyjson_test VERSION 0.0 DESCRIPTION "Tests for tinyjson library")

add_executable(tests primitives.c)

target_include_directories(tests PRIVATE ../src)
target_link_libraries(tests tinyjson)
