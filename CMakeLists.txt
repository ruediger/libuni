project(libuni)

cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
  set(
    CMAKE_BUILD_TYPE
    Debug
    CACHE
    STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif()

message(STATUS "Source: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary: ${CMAKE_BINARY_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message(FATAL_ERROR "In-source builds are not permitted. Make a separate folder for building:\nmkdir build; cd build; cmake ..\nBefore that, remove the files that cmake just created:\nrm -rf CMakeCache.txt CMakeFiles")
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(INSTALL_EXECUTABLES_PATH ${CMAKE_INSTALL_PREFIX}/bin)
set(INSTALL_LIBRARIES_PATH ${CMAKE_INSTALL_PREFIX}/lib)
set(INSTALL_HEADERS_PATH ${CMAKE_INSTALL_PREFIX}/include)
set(INSTALL_ETC_PATH ${CMAKE_INSTALL_PREFIX}/etc)
set(INSTALL_PC_PATH ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)

add_definitions("-DINSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

include(CheckIncludeFile)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSourceRuns)

set(CMAKE_CXX_FLAGS "-std=c++0x -pedantic-errors -Wall -Wextra")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mtune=native -march=native -DNDEBUG -fomit-frame-pointer -ffast-math") # TODO -Ofast GCC 4.6
set(CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
# TODO RelWithDebInfo MinSizeRel? maybe move -march=native -ffast-math etc. to a different build type

set(BUILD_TESTS True)

find_package(Boost 1.40.0 COMPONENTS unit_test_framework)
if(NOT Boost_FOUND)
  message(STATUS "Boost not found! (No Unit Tests)")
  set(BUILD_TESTS False)
else()
  link_directories(${Boost_LIBRARY_DIRS})
endif()

include_directories(${libuni_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)

if(NOT LIBUNI_VERSION)
  if(EXISTS "${libuni_SOURCE_DIR}/version")
    file(READ "${libuni_SOURCE_DIR}/version" LIBUNI_VERSION)
    string(REGEX REPLACE "\n" "" LIBUNI_VERSION "${LIBUNI_VERSION}")
  elseif(EXISTS "${libuni_SOURCE_DIR}/.git")
    if(NOT GIT_FOUND)
      find_package(Git QUIET)
    endif()
    if(GIT_FOUND)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --tags --dirty --always
        WORKING_DIRECTORY "${libuni_SOURCE_DIR}"
        OUTPUT_VARIABLE LIBUNI_VERSION
        RESULT_VARIABLE EXECUTE_GIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
    endif()
  endif()
endif()

if(NOT LIBUNI_VERSION)
  set(LIBUNI_VERSION "unknown-dirty")
endif()

message(STATUS "libuni version: ${VOBSUB2SRT_VERSION}")
