# xyjson performance test CMakeLists.txt

# Note: Force optimization flags for performance tests
# Even in Debug builds, we want accurate performance measurements
message(STATUS "Building performance tests with -O2 optimization")

# Performance test executable
add_executable(perf_test
    perf_common.cpp
    p_parse.cpp
    p_access.cpp
    p_iterator.cpp
    p_chained.cpp
    p_micro.cpp
)

# Link libraries
target_link_libraries(perf_test PRIVATE xyjson couttast)

# Set output directory
set_target_properties(perf_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Compile definitions
target_compile_definitions(perf_test PRIVATE
    XYJSON_PERF_TEST
)

# Include datasets directory
target_include_directories(perf_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Force Release compilation flags (for accurate performance measurements)
set_target_properties(perf_test PROPERTIES
    CXX_STANDARD 17
    COMPILE_FLAGS "-O2 -DNDEBUG"
)
