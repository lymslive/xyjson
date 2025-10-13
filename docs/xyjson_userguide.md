# xyjson C++ 封装库使用手册

`xyjson` 是一个基于高性能 [yyjson](https://github.com/ibireme/yyjson) 库的 C++ 封装，通过丰富的**操作符重载**提供简洁、直观的 JSON 操作体验。支持只读和可写两种模式，让 JSON 处理变得像原生 C++ 代码一样自然流畅。

## 快速开始

纯头文件库，可以只拷贝 include/xyjson.h 到项目中，也支持标准 cmake 集成。

### 基本使用
```cpp
#include "xyjson.h"

// 读取 JSON
std::string jsonText = R"({"name": "Alice", "age": 30})";
xyjson::Document doc(jsonText);

// 提取值
std::string name = doc / "name" | "unknown";
int age = doc / "age" | 0;

// 创建和修改 JSON
xyjson::MutableDocument mutDoc("{}");
mutDoc["name"] = "Bob";
mutDoc["age"] = 25;
mutDoc / "scores" << 95 << 87;
```

## 核心操作符

### 路径访问 `/`
```cpp
int value = doc / "user" / "profile" / "age" | 0;
```

### 值提取 `|`
```cpp
int age = doc / "age" | 0;           // 带默认值
std::string name = doc / "name" | "";
```

### 赋值 `=` (仅可写模式)
```cpp
mutDoc / "name" = "Alice";
mutDoc["new_field"] = "value";      // [] 可自动插入
```

### 智能输入 `<<`
```cpp
// 数组追加
mutDoc / "items" << 1 << "two" << 3.14;

// 对象插入
mutDoc.root() << "key1" << "value1" << "key2" << 42;
```

### 迭代器 `%`
```cpp
for (auto iter = doc / "items" % 0; iter; ++iter) {
    std::cout << "Item[" << iter->key << "] = " << (iter->value | "") << std::endl;
}
```

## 类层次结构

### 只读模式
- **`Value`** - 只读 JSON 值包装器
- **`Document`** - 只读 JSON 文档包装器

### 可写模式  
- **`MutableValue`** - 可写 JSON 值包装器
- **`MutableDocument`** - 可写 JSON 文档包装器

### 迭代器
- **`ArrayIterator`** / **`MutableArrayIterator`** - 数组迭代器
- **`ObjectIterator`** / **`MutableObjectIterator`** - 对象迭代器

## 操作符优先级表

| 操作符 | 功能 | 优先级 | 推荐度 |
|--------|------|--------|--------|
| `[]` | 索引访问 | 2 | ★★★★★ |
| `/` | 路径访问 | 5 | ★★★★★ |
| `=` | 赋值 | 16 | ★★★★★ |
| `\|` | 值提取 | 12 | ★★★★★ |
| `<<` | 智能输入 | 7 | ★★★★☆ |
| `>>` | 安全提取 | 7 | ★★★★☆ |
| `+` | 转数字 | 3 | ★★★★☆ |
| `-` | 转字符串 | 3 | ★★★★☆ |
| `%` | 迭代器 | 5 | ★★★☆☆ |
| `~` | 模式转换 | 3 | ★★★☆☆ |

## 性能特性

- **零拷贝设计**：底层基于 yyjson 的高性能特性
- **字符串引用优化**：字面量自动引用，减少拷贝
- **RAII 内存管理**：自动资源释放
- **编译时类型检查**：类型安全操作

## 错误处理

```cpp
// 显式检查
if (doc.isValid()) { /* 有效文档 */ }

// 隐式布尔转换
if (doc) { /* 有效 */ }
if (!doc) { /* 无效 */ }

// 安全提取
int value;
if (doc / "path" >> value) {
    // 成功提取
} else {
    // 提取失败
}
```

## 最佳实践

1. **选择合适模式**：只读操作用 `Document`，修改操作用 `MutableDocument`
2. **路径缓存**：深度嵌套时缓存中间结果
3. **字符串优化**：优先使用字面量获得引用优化
4. **批量操作**：使用 `create()` 预创建节点

## 完整示例

```cpp
#include "xyjson.h"
#include <iostream>

int main() {
    // 1. 读取 JSON
    std::string json = R"({
        "users": [
            {"name": "Alice", "age": 25},
            {"name": "Bob", "age": 30}
        ]
    })";
    
    xyjson::Document doc(json);
    
    // 2. 提取数据
    int userCount = +(doc / "users");
    std::cout << "User count: " << userCount << std::endl;
    
    // 3. 遍历用户
    for (auto iter = doc / "users" % 0; iter; ++iter) {
        auto user = iter->value;
        std::string name = user / "name" | "unknown";
        int age = user / "age" | 0;
        std::cout << name << " (" << age << ")" << std::endl;
    }
    
    return 0;
}
```

## 参考资源

- [yyjson 官方文档](https://github.com/ibireme/yyjson)
- [单元测试代码](utest/t_xyjson.cpp) - 查看完整的使用示例
- [API 头文件](include/xyjson.h) - 详细的接口定义
- [操作符详解](docs/xyjson_operator.md) - json 操作符重载详解

---