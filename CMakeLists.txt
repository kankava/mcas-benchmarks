cmake_minimum_required(VERSION 3.10)
project(mcas-benchmarks)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_FLAGS "-Wall -fstack-protector -fstack-protector-all")
set(CMAKE_CXX_FLAGS "-Wall -fstack-protector -fstack-protector-all")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "If enabled, compile the tests." OFF)
if (BUILD_TESTS)
    find_package(GMock MODULE REQUIRED)
    find_package(GTest MODULE REQUIRED)
    include(GoogleTest OPTIONAL RESULT_VARIABLE HAVE_CMAKE_GTEST)
    enable_testing()
endif ()

#add_subdirectory(lockfree-mcas)

find_package(Threads REQUIRED)

file(GLOB LB_HEADER_FILES "lockbased/*.h")
file(GLOB LB_SOURCE_FILES "lockbased/*.cpp")

file(GLOB LFMCAS_HEADER_FILES "lockfree-mcas/*.h")
file(GLOB LFMCAS_SOURCE_FILES "lockfree-mcas/*.cpp")

add_executable(mcas_benchmarks main.cpp benchmarks.cpp benchmarks.h benchmark.h mcas/mcas.h
               ${LB_HEADER_FILES} ${LB_SOURCE_FILES}
               ${LFMCAS_HEADER_FILES} ${LFMCAS_SOURCE_FILES}
              )
target_link_libraries(mcas_benchmarks ${CMAKE_THREAD_LIBS_INIT})
