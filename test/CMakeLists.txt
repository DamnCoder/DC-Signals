#-------------------------------------------------------------------------------------------
# Jorge López González
# DCPP Signals Test
#-------------------------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

PROJECT(DCPPSignalsTest)

#-------------------------------------------------------------------------------------------
# Folder definitions
#-------------------------------------------------------------------------------------------
# Set the output folders where the program will be created
SET(TEST_EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(PROJECT_PATH ${PROJECT_SOURCE_DIR}/../project)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
# C++ Dialect
SET(CMAKE_CXX_STANDARD 11) # C++11...
SET(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
SET(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

#-------------------------------------------------------------------------------------------
# Library definition
#-------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(include/test)

SET(SOURCE
	src/main.cpp
)

SET(HEADERS
	include/test/signal_tests.h
    include/test/silly_tests.h
    include/test/perfect_forwarding_tests.h
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE} ${HEADERS})

#-------------------------------------------------------------------------------------------
# Sub-projects
#-------------------------------------------------------------------------------------------

ADD_SUBDIRECTORY(${PROJECT_PATH} ${PROJECT_BINARY_DIR}/dcpp_signals)
INCLUDE_DIRECTORIES(${PROJECT_PATH}/include)

ADD_DEPENDENCIES(${PROJECT_NAME} DCPPSignals)

#-------------------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------------------
# Set the location for executable installation
INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${TEST_EXECUTABLE_OUTPUT_PATH})









