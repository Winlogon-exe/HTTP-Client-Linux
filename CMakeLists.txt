cmake_minimum_required(VERSION 3.10) 
project(HTTP_client)                    

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(inc)

file(GLOB SOURCES "src/*.cpp")

add_executable(HTTP_client ${SOURCES})
