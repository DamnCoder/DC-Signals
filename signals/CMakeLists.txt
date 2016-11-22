cmake_minimum_required(VERSION 3.1)

# Project's name
project("DCPPSignals")

# Build type
set(CMAKE_BUILD_TYPE Release)

# Set the output folders where the program will be created
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#Bring the headers, into the project
include_directories(include)
include_directories(include/signals)
include_directories(include/test)

# C++ Dialect
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

SET(SOURCE
	./src/main.cpp
)

SET(HEADERS
	./include/signals/connection.h
	./include/signals/signal.h
	./include/test/signal_tests.h
)

add_executable(DCPPSignals ${SOURCE} ${HEADERS})

