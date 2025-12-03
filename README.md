# xyjson

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[English Version](README-en.md) | [中文文档](README.md)

xyjson 是基于高性能 [yyjson](https://github.com/ibireme/yyjson) 封装的 C++ 纯头文件库，通过操作符重载提供直观的 JSON 处理体验。

命名 xyjson 的含义：
- 基于 yyjson 库，封装的 C++ 类相当于对应 yyjson 结构的代理(Pro**xy**)，命名空间也
  沿用 `yyjson::`；
- **x y** 常用于数学符号，xyjson 库追求像数学变量符号一样操作 JSON 数据。

## 特性

- 🚀 **高性能** - 底层基于 yyjson，零拷贝设计
- ✨ **直观语法** - 丰富的操作符重载，类似原生 C++
- 🔒 **类型安全** - 编译时类型检查，运行时安全提取
- 📚 **完整功能** - 支持读/写、迭代、文件操作等
- 🛠️ **易于集成** - **纯头文件库**，也支持 CMake 构建与 `find_package` 集成

## 依赖项

- **[yyjson](https://github.com/ibireme/yyjson)** - 核心依赖，高性能 JSON 解析库
- **[couttast](https://github.com/lymslive/couttast)** - 可选依赖，仅测试与开发使用
- **C++ 标准**: C++17 或更高
- **平台**: Linux, macOS, Windows (MinGW)

## 快速开始

### 免安装直接使用

在已安装 yyjson 底层库后，仅需将单头文件 `include/xyjson.h` 拷贝到项目适合位置，
开箱即用。

**代码示例：**
<!-- example:readme_quick_start -->
```cpp
#include "xyjson.h"

// 读取 JSON
std::string json = R"({"name": "Alice", "age": 30})";
yyjson::Document doc(json);

// 提取值
std::string name = doc / "name" | ""; // 读到 "Alice"
int age = doc / "age" | 0;            // 读到 30
```

### CMake 集成安装

支持 cmake 的标准构建流程：

```bash
# 克隆项目
git clone https://github.com/lymslive/xyjson
cd xyjson

# 构建（如果未安装依赖将自动下载）
mkdir build && cd build
cmake .. && make

# 安装到系统目录
sudo make install
```

默认是完整构建，包含单元测试与示例，但不包括性能测试（需 `BUILD_PERF=ON` 选项开启）。

#### 最小化构建与自定义安装

如果仅为依赖安装，可在克隆项目后指定 `XYJSON_LIB_ONLY=ON` 选项，跳过单元测试与示例构建，只安装库文件。
如果没有系统目录写入权限，可以通过标准选项 `CMAKE_INSTALL_PREFIX` 指定安装路径，如 `$HOME` 。

```bash
# 只构建库文件，跳过测试和示例
mkdir build && cd build
cmake .. -DXYJSON_LIB_ONLY=ON -DCMAKE_INSTALL_PREFIX=$HOME && make

# 安装到 $HOME 目录
make install
```

#### 其他目依赖集成 xyjson

安装后可在客户项目中使用 `find_package` 集成：

```cmake
# 在 CMakeLists.txt 中使用
find_package(xyjson REQUIRED)
target_link_libraries(your-target PRIVATE xyjson::xyjson)
```

或者在不想独立预安装时，可以用 FetchContent 自动下载并集成 xyjson，也推荐使用 `XYJSON_LIB_ONLY=ON` 选项来避免编译不必要的测试和示例：

```cmake
include(FetchContent)

FetchContent_Declare(
    xyjson
    GIT_REPOSITORY https://github.com/lymslive/xyjson.git
    GIT_TAG main
    # 只构建库，跳过测试和示例（推荐）
    OPTIONS "-DXYJSON_LIB_ONLY=ON"
)

FetchContent_MakeAvailable(xyjson)
target_link_libraries(your-target PRIVATE xyjson::xyjson)
```

详细用法请参考 [FetchContent 使用指南](docs/fetchcontent_usage.md)。

## 核心用法示例

### 基本操作

<!-- example:readme_basic_ops -->
```cpp
// 从 json 串创建文档对象，也可对已有对象 doc << 输入 json 串解析
yyjson::Document doc(R"({"name": "Alice", "scores": [95, 87]})");

// 路径访问
std::string name = doc / "name" | ""; // "Alice"

// 数组访问
int firstScore = doc / "scores" / 0 | 0; // 95

// 类型判断
bool isString = doc / "name" & ""; // true
bool isNumber = doc / "scores" / 0 & 0;   // true
```

### 可写文档操作

<!-- example:readme_mutable_ops -->
```cpp
// 创建可写文档对象，默认构建也是创建空 {} 根结点
yyjson::MutableDocument mutDoc("{}"); // 特殊字面量表示空对象

// 添加新键不能用路径操作符 / ，索引操作 [] 支持自动添加
mutDoc["name"] = "Bob";
mutDoc["scores"] = "[]"; // 特殊字面量表示空数组

// 数组追加
mutDoc / "scores" << 95 << 87;

// 文件写入
mutDoc.writeFile("output.json");

// 标准流输出: {"name":"Bob","scores":[95,87]}
std::cout << mutDoc << std::endl;
```

### 迭代遍历

<!-- example:readme_iterator -->
```cpp
yyjson::Document doc(R"({"user":{"name":"Alice", "age":"30"}, "items": ["apple","banana","cherry"]})");

// 数组迭代，iter.value() 可简化为解引用 *iter
for (auto iter = doc / "items" % 0; iter; ++iter) {
    std::cout << "Item " << iter.index() << ": " << (iter.value() | "") << std::endl;
}

// 对象迭代，iter.value() 可简化为解引用 *iter
for (auto iter = doc / "user" % ""; iter; ++iter) {
    std::cout << iter.name() << " = " << (iter.value() | "") << std::endl;
}
```

## 项目状态

- ✅ **稳定可用** - 核心功能已完成并通过测试
- 🔄 **持续开发** - 按 [需求列表](task_todo.md) 逐步完善功能
- 🧪 **测试覆盖** - 完善的单元测试确保质量
- ✅ **持续集成** - 配置了 [Actions](https://github.com/lymslive/xyjson/actions) CI/CD 流水线

## 开发流程

详见 [规范指南](DEVELOPMENT_GUIDE.md) 。

### 运行测试

```bash
cd build
./utxyjson --cout=silent  # 静默模式
./utxyjson                # 详细输出
```

### 代码结构

```
xyjson/
├── include/xyjson.h     # 主头文件（纯头文件库）
├── utest/               # 单元测试（含丰富示例）
├── examples/            # 应用示例（不依赖测试框架）
├── perf/                # 性能测试（主要与原生 yyjson 对比）
├── docs/                # 详细文档
├── task_todo.md         # 开发需求管理
├── task_log.md          # 任务完成记录
└── CMakeLists.txt       # 构建配置（支持 find_package）
```

### 文档导航

- 📖 [使用指南](docs/usage.md) - 详细的使用教程和最佳实践
- 🔧 [API 参考](docs/api.md) - 完整的操作符和类方法文档
- 🎨 [设计理念](docs/design.md) - 库的设计思路和哲学
- 📦 [依赖集成](docs/fetchcontent_usage.md) - 在其他项目中集成 xyjson 的详细指南
- 🧪 [单元测试](utest/README.md) - 单元测试说明
- 🧪 [性能测试](perf/README.md) - 性能测试说明
- 🧪 [应用示例](examples/README.md) - 应用用例说明
- 📋 [开发需求](task_todo.md) - 当前项目开发的需求列表
- 📊 [任务日志](task_log.md) - AI 协作的任务完成记录

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件

## 相关项目

- [yyjson](https://github.com/ibireme/yyjson) - 底层高性能 JSON 库
- [couttast](https://github.com/lymslive/couttast) - 单元测试框架

---

*简洁直观的 JSON 操作，让 C++ 开发更愉快！*
