cmake_minimum_required(VERSION 3.1)

# Project's name
project("DCPPSignals")

# Build type
set(CMAKE_BUILD_TYPE Release)

# Set the output folders where the program will be created
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# C++ Dialect
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

#Bring the headers, into the project
include_directories(include)

#The file(GLOB...) allows for wildcard additions:
file(GLOB_RECURSE SOURCES "src/*.cpp")

#Generate the static library from the sources
add_library(DCPPSignals STATIC ${SOURCES})

set_target_properties(DCPPSignals PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
 
#Set the location for library installation -- i.e., /usr/lib in this case
# not really necessary in this example. Use "sudo make install" to apply
install(TARGETS DCPPSignals DESTINATION ${PRJ_OUTPUT_PATH})
