#-------------------------------------------------------------------------------------------
# Jorge López González
# DCPP Signals
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5)
project(dcpp_signals)

#-------------------------------------------------------------------------------------------
# Library definition
#-------------------------------------------------------------------------------------------
set(HEADERS
	include/signals/connection.h
	include/signals/signal.h
)

# It's not possible to generate a static or dynamic library with only headers
add_library(${PROJECT_NAME} INTERFACE)

# setup include directories 
target_include_directories(${PROJECT_NAME} INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

#-------------------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------------------
install(TARGETS ${PROJECT_NAME} 
    EXPORT DCPPSIGNALS_EXPORTS
    RUNTIME DESTINATION bin 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)

#  HACK: have the files showing in the IDE, under the name '${PROJECT_NAME}_IDE'
add_custom_target(${PROJECT_NAME}_ide SOURCES ${HEADERS})
