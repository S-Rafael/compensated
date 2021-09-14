# encoding: UTF-8
# main cmake file for knlite.
#------------------------------------------------------------------------------
#
# © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
#
# This software is licensed under the terms of the 3-Clause BSD License.
# Please refer to the accompanying LICENSE file for the license terms.
# 
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.5)
project(knlite LANGUAGES CXX)
include(GNUInstallDirs)

# Add subdirectories:
add_subdirectory(tests)
add_subdirectory(example)

# Add a `check` target to run tests:
add_custom_target(check
                  COMMAND ./tests
                  DEPENDS tests
                  WORKING_DIRECTORY tests)

# Add a dummy target for the header file, which does nothing
add_library(knlite INTERFACE knlite.h)
set_source_files_properties(knlite.h PROPERTIES HEADER_FILE_ONLY TRUE)
set_target_properties(knlite PROPERTIES PUBLIC_HEADER knlite.h)

# Add the install target
install(TARGETS knlite  
        PUBLIC_HEADER
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knlite")

