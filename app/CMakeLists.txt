cmake_minimum_required(VERSION 3.2)
project(app)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    build/basalt/src/resource_ids.auto.h
    build/basalt/appinfo.auto.c
    src/main.c)

add_executable(app ${SOURCE_FILES})