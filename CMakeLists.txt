# encoding: UTF-8
# main cmake file for the Compensated project.
#------------------------------------------------------------------------------
#
# © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
#
# This software is licensed under the terms of the 3-Clause BSD License.
# Please refer to the accompanying LICENSE file for the license terms.
# 
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.5)
project(compensated LANGUAGES CXX)
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
add_library(compensated INTERFACE compensated.h)
set_source_files_properties(compensated.h PROPERTIES HEADER_FILE_ONLY TRUE)
set_target_properties(compensated PROPERTIES PUBLIC_HEADER compensated.h)

# Add the install commands
install(TARGETS compensated  
        PUBLIC_HEADER
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/compensated")
install(FILES LICENSE
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/compensated")

