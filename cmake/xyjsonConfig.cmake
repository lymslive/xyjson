# xyjson CMake configuration file
# This file is used by find_package(xyjson) to locate the xyjson library

# Set package version
set(xyjson_VERSION "1.0.0")

# Import targets from the installation
include("${CMAKE_CURRENT_LIST_DIR}/xyjsonTargets.cmake")

# Check if all required components are found
set(xyjson_FOUND TRUE)

# Set variables for backward compatibility
set(xyjson_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include")
set(xyjson_LIBRARIES xyjson::xyjson)

# Print status message
if(NOT xyjson_FIND_QUIETLY)
    message(STATUS "Found xyjson: ${CMAKE_CURRENT_LIST_DIR}/../ (found version ${xyjson_VERSION})")
endif()