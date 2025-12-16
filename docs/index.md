---
title: xyjson Documentation
layout: default
permalink: /
---

# xyjson Documentation

**xyjson** 是一个基于 [yyjson](https://github.com/ibireme/yyjson) 的 C++ 封装库，提供直观的操作符重载 JSON 操作功能。现在是一个 **header-only** 库，支持 CMake 集成。

## ✨ 特性

- **Header-only 库** - 易于集成，无需编译
- **高性能** - 基于 yyjson 的零拷贝设计
- **操作符重载** - 直观的 API 设计 (`/`, `|`, `=`, `<<`, `%` 等)
- **RAII 内存管理** - 自动内存管理，防止泄漏
- **类型安全** - 编译时检查与安全的运行时值提取
- **CMake 集成** - 支持 `find_package()` 集成

## 🚀 快速开始

### 安装方式

#### 方式1: CMake 集成 (推荐)
```cmake
find_package(xyjson REQUIRED)
target_link_libraries(your-target PRIVATE xyjson::xyjson)
```

#### 方式2: 直接复制
```bash
# 复制头文件到你的项目
cp include/xyjson.h /path/to/your/project/include/
```

### 简单示例

```cpp
#include <xyjson.h>
using namespace yyjson;

// 解析 JSON 字符串
Document doc(R"({"name": "xyjson", "version": "1.0"})");

// 使用 / 操作符访问值
std::string name = doc/"name";           // "xyjson"
std::string version = doc/"version";     // "1.0"

// 创建新的 JSON 文档
MutableDocument newDoc;
newDoc["type"] = "library";
newDoc["features"] = {"header-only", "fast", "safe"};
```

## 📚 文档

- [**API 文档**](api.html) - 完整的 API 参考文档
- [**使用指南**](usage.html) - 详细的使用说明和示例
- [**设计文档**](design.html) - 架构设计和实现原理
- [**FetchContent 使用**](fetchcontent_usage.html) - CMake FetchContent 集成指南

## 🔧 架构概览

xyjson 维护两个并行的层次结构：

| 特性 | 只读模型 | 可写模型 |
|------|---------|----------|
| **文档类** | Document | MutableDocument |
| **值结点** | Value | MutableValue |
| **数组迭代器** | ArrayIterator | MutableArrayIterator |
| **对象迭代器** | ObjectIterator | MutableObjectIterator |
| **主要用途** | 高效访问、只读操作 | 动态构建、内容修改 |
| **内存管理** | 只读访问，不可修改 | 读写权限，可动态修改 |
| **性能特点** | 零拷贝，最高性能 | 支持修改，稍有开销 |
| **操作符支持** | `/`, `|`, `=`, `!`, `==` 等 | 额外支持 `<<`, `>>`, `+=` 等 |

### 核心设计模式

1. **操作符重载** - 通过操作符提供核心功能
2. **RAII 内存管理** - 自动 yyjson 内存管理
3. **零拷贝设计** - 利用 yyjson 的高性能特性
4. **类型安全** - 编译时检查与安全的运行时值提取

## 🧪 测试

```bash
# 克隆并构建
git clone https://github.com/lymslive/xyjson.git
cd xyjson
mkdir build && cd build
cmake .. && make

# 运行测试
./utxyjson --cout=silent  # 静默模式
./utxyjson                # 详细输出
```

## 📖 依赖

- **yyjson** - 核心JSON处理库 (系统安装或自动下载)
- **C++17** - 现代C++特性支持
- **CMake 3.15+** - 构建系统

## 🔗 相关链接

- [GitHub 仓库](https://github.com/lymslive/xyjson)
- [问题反馈](https://github.com/lymslive/xyjson/issues)
- [releases](https://github.com/lymslive/xyjson/releases)

---

*本文档由 Jekyll 自动生成，最后更新时间: {{ site.time | date: "%Y-%m-%d %H:%M:%S" }}*
