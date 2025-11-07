# xyjson 性能测试 CMakeLists.txt

# 性能测试可执行文件
add_executable(perf_test
    perf_common.cpp
    p_parse.cpp
    p_access.cpp
    p_iterator.cpp
    p_chained.cpp
    p_comparison.cpp
)

# 链接库
target_link_libraries(perf_test PRIVATE xyjson couttast)

# 设置输出目录
set_target_properties(perf_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 编译定义
target_compile_definitions(perf_test PRIVATE
    XYJSON_PERF_TEST
)

# 包含数据集目录
target_include_directories(perf_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
