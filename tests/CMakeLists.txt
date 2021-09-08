# encoding: UTF-8
# cmake file for the unit tests of knlite.
#------------------------------------------------------------------------------
#
# © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
#
# This software is licensed under the terms of the 3-Clause BSD License.
# Please refer to the accompanying LICENSE file for the license terms.
# 
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0075 NEW)

include(CheckIncludeFileCXX)

project(knlite_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(run_tests tests.cpp)

set(CMAKE_REQUIRED_LIBRARIES "gtest")
CHECK_INCLUDE_FILE_CXX("gtest/gtest.h" HAS_GTEST)
if (HAS_GTEST)
	message(STATUS "Found the headers for the googletest library")
else()
	message(FATAL_ERROR "Fatal: Could not find the header file `gtest/gtest.h`.\n"
	"Make sure the library `googletest` is installed and configured on your system.\n"
	"You can obtain it from https://github.com/google/googletest")
endif()

target_link_libraries(run_tests gtest)

if (MSVC)
	add_compile_options(/W4 /O2)
else()
	add_compile_options(-Wall -O3)
endif()





