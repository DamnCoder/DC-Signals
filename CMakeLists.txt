cmake_minimum_required(VERSION 3.1)

# Project's name
PROJECT(DCPPSignals)

# Build type
SET(CMAKE_BUILD_TYPE Release)

# Set the output folders where the program will be created
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(SRC_DIR ${PROJECT_SOURCE_DIR}/code)

# C++ Dialect
SET(CMAKE_CXX_STANDARD 11) # C++11...
SET(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
SET(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

#Bring the headers, into the project
INCLUDE_DIRECTORIES(${SRC_DIR}/include)
INCLUDE_DIRECTORIES(${SRC_DIR}/include/signals)
INCLUDE_DIRECTORIES(${SRC_DIR}/include/test)

SET(SOURCE
	${SRC_DIR}/src/main.cpp
)

SET(HEADERS
	${SRC_DIR}/include/signals/connection.h
	${SRC_DIR}/include/signals/signal.h
	${SRC_DIR}/include/test/signal_tests.h
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE} ${HEADERS})

