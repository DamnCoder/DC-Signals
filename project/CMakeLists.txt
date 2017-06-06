#-------------------------------------------------------------------------------------------
# Jorge López González
# DCPP Signals
#-------------------------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

PROJECT(DCPPSignals)

#-------------------------------------------------------------------------------------------
# Folder definitions
#-------------------------------------------------------------------------------------------
# Set the output folders where the program will be created
SET(DCPPSIGNALS_LIB_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

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
INCLUDE_DIRECTORIES(include/signals)

SET(HEADERS
	include/signals/connection.h
	include/signals/signal.h
)

# It's not possible to generate a static or dynamic library with only headers
ADD_LIBRARY(${PROJECT_NAME} INTERFACE)
TARGET_SOURCES(${PROJECT_NAME} INTERFACE ${HEADERS})

#-------------------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------------------
# Copy the include directory where the lib is installed
INSTALL(DIRECTORY include DESTINATION ${DCPPSIGNALS_LIB_OUTPUT_PATH})

#  HACK: have the files showing in the IDE, under the name '${PROJECT_NAME}_IDE'
ADD_CUSTOM_TARGET(${PROJECT_NAME}_IDE SOURCES ${HEADERS})
