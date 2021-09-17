# encoding: UTF-8
# cmake file for the unit tests of Compensated.
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

project(compensated_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(tests
               tests.cpp
               basic.cpp
               std.cpp
               custom-types.cpp)

find_package(GTest REQUIRED)
if (NOT GTest_FOUND)
	message(FATAL_ERROR 
	"Make sure the library `googletest` is installed and configured on your system.\n"
	"You can obtain it from https://github.com/google/googletest")
endif()

target_link_libraries(tests gtest)

if (MSVC)
	add_compile_options(/W4 /O2)
else()
	add_compile_options(-Wall -O3)
endif()

add_test(tests tests)


