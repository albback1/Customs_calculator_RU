cmake_minimum_required(VERSION 3.10)

project(Calculator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES calc.cpp calc.h)

add_library(calc SHARED ${SOURCES})