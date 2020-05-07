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

add_subdirectory(lockfree-mcas)

find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

file(GLOB LB_HEADER_FILES "lockbased/*.h")
file(GLOB LB_SOURCE_FILES "lockbased/*.cpp")

file(GLOB LFMCAS_HEADER_FILES "lockfree-mcas/*.h")
file(GLOB LFMCAS_SOURCE_FILES "lockfree-mcas/*.cpp")

add_executable(mcas_benchmarks main.cpp benchmarks.cpp benchmarks.h mcas/mcas.h
               ${LB_HEADER_FILES} ${LB_SOURCE_FILES}
               ${LFMCAS_HEADER_FILES} ${LFMCAS_SOURCE_FILES}
              )
target_link_libraries(mcas_benchmarks ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_casn test_casn.cpp catch.hpp)
target_link_libraries(test_casn ${CMAKE_THREAD_LIBS_INIT})

#find_program(STATIC_ANALYZER pvs-studio)
#
#if (STATIC_ANALYZER)
#    include(cmake/PVS-Studio.cmake)
#    pvs_studio_add_target(TARGET analyze
#            ANALYZE mcas #subtarget:path/to/subtarget
#            FORMAT tasklist
#            PREPROCESSOR gcc
#            LOG "${CMAKE_HOME_DIRECTORY}/pvs.log")
#    #LICENSE "/path/to/PVS-Studio.lic"
#    #CXX_FLAGS ${PREPROCESSOR_ADDITIONAL_FLAGS}
#    #C_FLAGS ${PREPROCESSOR_ADDITIONAL_FLAGS}
#    #CONFIG "${CMAKE_HOME_DIRECTORY}/PVS-Studio.cfg"
#endif ()

