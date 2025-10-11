cmake_minimum_required(VERSION 3.15)
project(xyjson VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add $HOME to search paths for some dependent libs may install there
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}")
find_package(yyjson REQUIRED)
find_package(couttast REQUIRED)

# Source files for static library
set(LIB_SOURCES
    src/xyjson.cpp
)

# Create static library
add_library(xyjson STATIC ${LIB_SOURCES})
target_include_directories(xyjson PUBLIC include)
target_link_libraries(xyjson PUBLIC yyjson)

# Source files for test executable
set(TEST_SOURCES
    utest/t_xyjson.cpp
)

# Create test executable
add_executable(utxyjson ${TEST_SOURCES})
target_link_libraries(utxyjson PRIVATE xyjson couttast)

# Install library and headers
install(TARGETS xyjson
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
