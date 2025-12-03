# xyjson unit test CMakeLists.txt

# Source files for test executable (main tests)
set(TEST_SOURCES
    t_basic.cpp
    t_mutable.cpp
    t_stream.cpp
    t_iterator.cpp
    t_conversion.cpp
    t_advanced.cpp
    t_experiment.cpp
)

# Create main test executable
add_executable(utxyjson ${TEST_SOURCES})
target_link_libraries(utxyjson PRIVATE xyjson couttast)

# Set output directory to build root
set_target_properties(utxyjson PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Document examples test executable
set(DOC_TEST_SOURCES
    t_readme.cpp
    t_usage.cpp
    t_api.cpp
    t_design.cpp
)

add_executable(utdocs ${DOC_TEST_SOURCES})
target_link_libraries(utdocs PRIVATE xyjson couttast)

# Set output directory to build root
set_target_properties(utdocs PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# miniut: Minimal unit test (readonly only, no mutable features)
add_executable(miniut
    t_basic.cpp
    t_experiment.cpp
)
target_link_libraries(miniut PRIVATE xyjson couttast)
target_compile_definitions(miniut PRIVATE XYJSON_DISABLE_MUTABLE XYJSON_DISABLE_CHAINED_INPUT)

# Set output directory to build root
set_target_properties(miniut PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)