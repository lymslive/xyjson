# xyjson

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

C++ 封装的 JSON 操作库，基于高性能 [yyjson](https://github.com/ibireme/yyjson)，通过操作符重载提供直观的 JSON 处理体验。

## 特性

- 🚀 **高性能** - 底层基于 yyjson，零拷贝设计
- ✨ **直观语法** - 丰富的操作符重载，类似原生 C++
- 🔒 **类型安全** - 编译时类型检查，运行时安全提取
- 📚 **完整功能** - 支持读/写、迭代、文件操作等
- 🛠️ **易于集成** - 头文件库，CMake 构建支持

## 快速开始

### 安装依赖

确保系统已安装 yyjson：
```bash
# Ubuntu/Debian
sudo apt-get install libyyjson-dev

# 或从源码安装到 /usr/local/include/yyjson.h
```

### 集成到项目

1. 复制头文件到项目：
```bash
cp include/xyjson.h your-project/include/
```

2. 在代码中使用：
```cpp
#include "xyjson.h"

// 读取 JSON
std::string json = R"({"name": "Alice", "age": 30})";
xyjson::Document doc(json);

// 提取值
std::string name = doc / "name" | "unknown";
int age = doc / "age" | 0;
```

### 构建示例

```bash
# 克隆项目
git clone <repository-url>
cd xyjson

# 构建
mkdir build && cd build
cmake .. && make

# 运行测试
./utxyjson --cout=silent
```

## 核心用法

### 基本操作

```cpp
// 创建文档
xyjson::Document doc(R"({"name": "Alice", "scores": [95, 87]})");

// 路径访问
std::string name = doc / "name" | "";

// 数组访问
int firstScore = doc / "scores" / 0 | 0;

// 创建可写文档
xyjson::MutableDocument mutDoc("{}");
mutDoc["name"] = "Bob";
mutDoc["scores"] = "[]";
mutDoc / "scores" << 95 << 87;
```

### 文件操作

```cpp
// 读取文件
xyjson::Document doc;
doc.readFile("config.json");

// 写入文件
xyjson::MutableDocument mutDoc;
mutDoc["version"] = "1.0";
mutDoc.writeFile("output.json");
```

### 迭代遍历

```cpp
// 数组迭代
for (auto iter = doc / "items" % 0; iter; ++iter) {
    std::cout << "Item " << iter->key << ": " << (iter->value | "") << std::endl;
}

// 对象迭代
for (auto iter = doc / "user" % ""; iter; ++iter) {
    std::cout << iter->key << " = " << (iter->value | "") << std::endl;
}
```

## API 概览

### 主要操作符

| 操作符 | 功能 | 示例 |
|--------|------|------|
| `/` | 路径访问 | `doc / "user" / "name"` |
| `\|` | 值提取 | `doc / "age" \| 0` |
| `=` | 赋值 | `mutDoc / "name" = "Alice"` |
| `<<` | 智能输入 | `mutDoc / "items" << 1 << 2` |
| `%` | 迭代器 | `doc / "items" % 0` |

### 核心类

- **`xyjson::Document`** - 只读 JSON 文档
- **`xyjson::MutableDocument`** - 可写 JSON 文档  
- **`xyjson::Value`** - 只读 JSON 值
- **`xyjson::MutableValue`** - 可写 JSON 值

## 构建选项

### CMake 目标

```cmake
# 链接静态库
target_link_libraries(your-target xyjson)

# 或直接包含头文件
target_include_directories(your-target PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
```

### 编译选项

- **C++ 标准**: C++17 或更高
- **依赖**: yyjson, couttast (测试)
- **平台**: Linux, macOS, Windows (MinGW)

## 示例项目

查看 [examples/](examples/) 目录获取更多使用示例：

```cpp
// 配置解析示例
#include "xyjson.h"

xyjson::Document config;
config.readFile("app_config.json");

std::string host = config / "server" / "host" | "localhost";
int port = config / "server" / "port" | 8080;
bool debug = config / "debug" | false;
```

## 性能对比

基于 yyjson 的高性能特性，xyjson 在保持易用性的同时提供优秀的性能：

- **零拷贝解析** - 直接操作原始 JSON 数据
- **高效内存管理** - RAII 自动资源释放
- **字符串优化** - 字面量引用减少拷贝

## 开发

### 运行测试

```bash
cd build
./utxyjson --cout=silent  # 静默模式
./utxyjson                # 详细输出
```

### 代码结构

```
xyjson/
├── include/xyjson.h     # 主头文件
├── src/xyjson.cpp       # 实现文件
├── utest/t_xyjson.cpp   # 单元测试
├── docs/                # 详细文档
└── CMakeLists.txt       # 构建配置
```

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件

## 相关项目

- [yyjson](https://github.com/ibireme/yyjson) - 底层高性能 JSON 库
- [couttast](https://github.com/lymslive/couttast) - 单元测试框架

---

*简洁直观的 JSON 操作，让 C++ 开发更愉快！*