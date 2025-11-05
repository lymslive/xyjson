# examples/CMakeLists.txt
# CMake build configuration for xyjson examples

cmake_minimum_required(VERSION 3.15)
project(xyjson_examples)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
# xyjson is a header-only library, use the target from parent scope

# Example 1: jp - Minimal jq-like JSON path query tool
add_executable(jp jp_jq.cpp)
target_link_libraries(jp PRIVATE xyjson)

# Example 2: struct_map - C++ struct to JSON mapping
add_executable(struct_map struct_map.cpp)
target_link_libraries(struct_map PRIVATE xyjson)

# Example 3: json_transformer - JSON data transformation tool
add_executable(json_transformer json_transformer.cpp)
target_link_libraries(json_transformer PRIVATE xyjson)

# Create an "examples" target that builds all examples
add_custom_target(examples ALL
    DEPENDS jp struct_map json_transformer
    COMMENT "Building all xyjson examples"
)

# Set output directory for examples
set_target_properties(jp struct_map json_transformer
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)
