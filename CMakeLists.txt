cmake_minimum_required(VERSION 3.10)
project(xyjson)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)
include_directories(/usr/local/include)

# Find yyjson library (assumed to be installed system-wide)
find_library(YYJSON_LIB yyjson)
if(NOT YYJSON_LIB)
    message(FATAL_ERROR "yyjson library not found. Please install yyjson library.")
endif()

# Find couttast library
find_library(COUTTAST_LIB couttast PATHS ~/lib)
if(NOT COUTTAST_LIB)
    message(FATAL_ERROR "couttast library not found. Please install couttast library.")
endif()

# Add include path for couttast
include_directories(~/include)

# Source files for static library
set(LIB_SOURCES
    src/xyjson.cpp
)

# Create static library
add_library(xyjson STATIC ${LIB_SOURCES})
target_link_libraries(xyjson ${YYJSON_LIB})

# Source files for test executable
set(TEST_SOURCES
    utest/t_xyjson.cpp
)

# Create test executable
add_executable(utxyjson ${TEST_SOURCES})
target_link_libraries(utxyjson xyjson ${COUTTAST_LIB} ${YYJSON_LIB})

# Install library and headers
install(TARGETS xyjson
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)