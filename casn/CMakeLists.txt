cmake_minimum_required(VERSION 3.10)
project(casn)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_FLAGS "-Wall -fstack-protector -fstack-protector-all ") #-fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "-Wall -fstack-protector -fstack-protector-all ") # -fsanitize=address -fno-omit-frame-pointer")
SET(CMAKE_CXX_FLAGS -pthread)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

add_library(casn casn.c casn.h)
#add_executable(test_casn test_casn.cpp casn.h)
#target_link_libraries(test_casn casn ${CMAKE_THREAD_LIBS_INIT})

add_library(mcas mcas.c mcas.h)
target_link_libraries(mcas casn)

add_executable(test_deque test_deque.cpp Deque.h mcas.h)
target_link_libraries(test_deque mcas ${CMAKE_THREAD_LIBS_INIT})

add_executable(benchmark_deque benchmark_deque.cpp Deque.h benchmark.h mcas.h mcas.c casn.h casn.c)
target_link_libraries(benchmark_deque ${CMAKE_THREAD_LIBS_INIT})
