# Project initial configuration
cmake_minimum_required(VERSION 3.5)

project(dcpp_signals LANGUAGES CXX)

# Initial check to abort any attempt of in source build
if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
    message(FATAL "no in source building allowed." )
endif()

# Project setup
set(CMAKE_CONFIGURATION_TYPES
	Debug
	Release
	RelWithDebInfo
	MinSizeRel
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (WIN32) # Windows
    add_compile_options(-DWIN32)

    # Set Release output directory for runtime targerts
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
    # Use Release directory for RelWithDebInfo and MinSizeRel builds
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Release)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Release)
    # Set Debug output directory for runtime targets
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)

    # Data destination directories
    set(DATA_DEST_WIN_DEB ${CMAKE_BINARY_DIR}/Debug/data)
    set(DATA_DEST_WIN_REL ${CMAKE_BINARY_DIR}/Release/data)
else() # Linux
    if (UNIX AND NOT APPLE)
        add_compile_options(-DLINUX)
        add_compile_options(-DUNIX)
    endif()

    # Set output directories for libraries and binaries.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

    # Data destination directory
    set(DATA_DEST_LINUX ${CMAKE_BINARY_DIR}/bin/data)
endif()

# Conan setup
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/common)
include(conan)
conan_check(VERSION 1.14.3 REQUIRED)
conan_add_remote(
    NAME bincrafters_remote
    INDEX 1
    URL https://api.bintray.com/conan/bincrafters/public-conan)

conan_add_remote(
    NAME conan_center_remote
    INDEX 2
    URL https://api.bintray.com/conan/conan/conan-center)

# Conan dependences installation
if (WIN32)
    conan_cmake_run(
        CONANFILE conanfile.py
        BASIC_SETUP CMAKE_TARGETS
        SETTINGS compiler.version=14
        BUILD missing
        GENERATORS cmake_find_package_multi cmake_paths)

    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
else()
    conan_cmake_run(
        CONANFILE conanfile.py
        BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS
        PROFILE clang
        BUILD missing)

    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
endif()

add_subdirectory(sources)
add_subdirectory(test)
