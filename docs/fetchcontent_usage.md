# 在其他项目中使用 FetchContent 集成 xyjson

## 基本用法

当使用 FetchContent 将 xyjson 作为依赖项时，可以通过以下方式指定 `XYJSON_LIB_ONLY=ON` 选项来避免编译不必要的测试和示例：

### 方法 1：在 FetchContent_Declare 中设置选项

```cmake
include(FetchContent)

FetchContent_Declare(
    xyjson
    GIT_REPOSITORY https://github.com/lymslive/xyjson.git
    GIT_TAG main
    # 指定只构建库，跳过测试和示例
    OPTIONS "-DXYJSON_LIB_ONLY=ON"
)

FetchContent_MakeAvailable(xyjson)

# 链接到你的目标
target_link_libraries(your_target PRIVATE xyjson::xyjson)
```

### 方法 2：使用全局缓存变量

```cmake
include(FetchContent)

# 在 FetchContent_MakeAvailable 之前设置选项
set(XYJSON_LIB_ONLY ON CACHE BOOL "Build only xyjson library" FORCE)

FetchContent_Declare(
    xyjson
    GIT_REPOSITORY https://github.com/lymslive/xyjson.git
    GIT_TAG main
)

FetchContent_MakeAvailable(xyjson)

# 链接到你的目标
target_link_libraries(your_target PRIVATE xyjson::xyjson)
```

### 方法 3：使用命令行参数

如果用户希望在配置时动态控制，可以在 CMake 命令行中指定：

```bash
cmake -DXYJSON_LIB_ONLY=ON ..
```

## 完整示例

以下是一个完整的项目 CMakeLists.txt 示例：

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 FetchContent
include(FetchContent)

# 配置 xyjson 依赖
FetchContent_Declare(
    xyjson
    GIT_REPOSITORY https://github.com/lymslive/xyjson.git
    GIT_TAG main
    OPTIONS "-DXYJSON_LIB_ONLY=ON"
)

# 获取并构建 xyjson
FetchContent_MakeAvailable(xyjson)

# 创建你的可执行文件
add_executable(my_app main.cpp)

# 链接 xyjson
target_link_libraries(my_app PRIVATE xyjson::xyjson)
```

## 选项说明

- `XYJSON_LIB_ONLY=ON`：只构建 xyjson 接口库，跳过：
  - 单元测试（utxyjson, utdocs, miniut）
  - 示例程序（jp, struct_map, json_transformer）
  - 性能测试（perf_test）
  - couttast 测试框架依赖

- `XYJSON_LIB_ONLY=OFF`（默认）：构建所有内容，包括测试和示例

## 优势

使用 `XYJSON_LIB_ONLY=ON` 的优势：

1. **更快的构建时间**：跳过不必要的测试和示例编译
2. **更少的依赖**：不需要下载和编译 couttast 测试框架
3. **更小的构建输出**：只生成必要的库文件
4. **更适合 CI/CD**：在持续集成中减少构建时间和资源消耗

## 注意事项

- 如果你的项目也需要 xyjson 的测试功能，请不要设置此选项
- 该选项不会影响 xyjson 的核心功能，所有 API 都可以正常使用
- 当使用 `find_package(xyjson)` 安装版本时，此选项不适用（因为已经预编译）