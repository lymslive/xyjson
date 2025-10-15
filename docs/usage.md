# xyjson 使用指南

本指南从库使用者的角度，详述如何利用 xyjson 库在 C++ 项目中执行常见的 JSON 操作。

## 概述

xyjson 基于 [yyjson](https://github.com/ibireme/yyjson) 高性能 JSON 库构建，继承其两种核心模型：

- **只读模型** (`yyjson_doc`, `yyjson_val`) - 高性能解析，零拷贝访问
- **可写模型** (`yyjson_mut_doc`, `yyjson_mut_val`) - 灵活修改，完整构建

在 `yyjson::` 命名空间浅封装了四个对应的类：
- Document: 仅 `yyjson_doc*` 指针
- Value: 仅 `yyjson_val*` 指针
- MutableDocument: 仅 `yyjson_mut_doc*` 指针
- MutableValue: 包括 `yyjson_mut_val*` 与 `yyjson_mut_doc*` 指针方便修改

Document 与 MutableDocument 负责整个 Json 树的内存自动管理，而 Value 与
MutableValue 是对某个 Json 结点的引用。然后通过 C++ 操作符重载封装了常用操作，
同时提供对应的具名方法供不同编程风格的用户选择。

## 只读模型基本操作

只读模型的基本操作，大多也适于可写模型。故本节 Document 一般泛指两个文档类，
Value 泛指两个 Json 结点类。

### 读入与解析 JSON 文档

Document 类对象可以从字符串直接构造，也能在创建对象后用 `<<` 操作符读入字符串
再解析。也支持从文件读取 Json ，但提供文件名参数时，只能用 `readFile` 方法，不
能用 `<<` 操作符，避免与 Json 串本身作为参数的歧义。然后在提供文件对象 `FILE*`
或 `std::iftream` 时，支持用 `<<` 操作符。

```cpp
#include "xyjson.h"

// 从字符串创建文档
std::string jsonText = R"({"name": "Alice", "age": 30})";
yyjson::Document doc(jsonText); // doc << jsonText;

// 从文件读取
yyjson::Document docFromFile;
docFromFile.readFile("config.json");

// 检查文档有效性
if (!doc) {
    std::cerr << "JSON 解析失败" << std::endl;
    return;
}
```

### 访问 JSON 结点

Value 支持用常规的 `[]` 索引访问对象或数组，但更推荐使用路径操作符 `/` 。
也支持直接从 Document 作路径操作，相当于从其根结点开始索引。

```cpp
// 路径访问（单层索引）
yyjson::Value nameNode = doc / "name";

// JSON Pointer 多级路径（以 / 开头）
yyjson::Value userNode = doc / "/user/profile/name";

// 数组索引
yyjson::Value firstItem = doc / "items" / 0;

// 链式访问
yyjson::Value deepValue = doc / "user" / "profile" / "address" / "city";
```

路径操作符 `/` 强于索引操作符 `[]` 之处在于可以访问多级路径，遵循 JSON Pointer
标准，必须以 `/` 开头，中间键名本身若含 `/` 或 `~` 特殊字符需要分别转义为 `~1`
与 `~1`。当然对于实际项目，强烈建议避免键名含特殊字符，仅须注意以 `/` 开头。多
级路径适于动态配置，若键名固定，分开链式访问显然性能更高。

### 读取值

对于 Json 叶结点，非容器类型的 Value ，可以用取值操作符 `|` 按适配类型取值。这
是位或符号的重载，就可读作或，表示如果 Value 值无效或类型不匹配，就返回右侧的
默认值。

也支持 `|=` 复合操作，`result |= json` 相当于 `result = json | result` ，表示
只有当 json 类型类型符合 `result` 这样的基本类型时才读取并赋值。

如果希望显式知道读取是否成功，可用 `json >> target` 操作，它返回 true 时会更新
target 的值。但一般情况下仅用 `|` 带默认值读取最方便。

```cpp
// 带默认值的提取
std::string name = doc / "name" | "unknown";
int age = doc / "age" | 0;
double price = doc / "price" | 0.0;
bool active = doc / "active" | false;

// 再尝试读另一个键
name |= doc / "nickName";

// 安全提取（返回 bool 表示成功）
std::string extractedName;
if (doc / "name" >> extractedName) {
    // 成功提取
}

// 自定义管道函数
std::string uppercaseName = doc / "name" | [](const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
} | "DEFAULT";
```

### 类型判断与错误处理

```cpp
// 类型判断操作符
bool isString = doc / "name" & "";
bool isNumber = doc / "age" & 0;
bool isObject = doc / "user" & "{}";
bool isArray = doc / "items" & "[]";

// 具名类型判断方法
if (doc / "age".isNumber()) {
    // 是数字类型
}

// 获取类型名称
std::string typeName = doc / "name".typeName();

// 有效性检查
if (doc / "nonexistent") {
    // 结点存在
} else {
    // 结点不存在或无效
}
```

### 重新序列化与输出

```cpp
// 转字符串
std::string jsonString = doc.toString();

// 流输出
std::cout << doc << std::endl;

// 带格式化的输出
std::cout << doc.toString(true) << std::endl;

// 写入文件
doc.writeFile("output.json");
```

## 可写 JSON 模型操作

### 创建与修改

```cpp
// 创建空文档
yyjson::MutableDocument mutDoc("{}");

// 赋值操作
mutDoc / "name" = "Alice";
mutDoc / "age" = 30;

// 数组操作
mutDoc / "scores" = "[]";
mutDoc / "scores" << 95 << 87 << 92;

// 对象插入
mutDoc.root() << "city" << "Beijing" << "country" << "China";

// 嵌套结构构建
mutDoc / "user" / "profile" = "{}";
mutDoc / "user" / "profile" / "name" = "Bob";
mutDoc / "user" / "profile" / "age" = 25;
```

### 复杂数据结构构建

```cpp
// 构建用户列表
mutDoc / "users" = "[]";

// 添加第一个用户
mutDoc / "users" << "{}";
mutDoc / "users" / 0 << "name" << "Alice" << "age" << 25;

// 添加第二个用户
mutDoc / "users" << "{}";
mutDoc / "users" / 1 << "name" << "Bob" << "age" << 30;

// 添加用户爱好数组
mutDoc / "users" / 0 / "hobbies" = "[]";
mutDoc / "users" / 0 / "hobbies" << "reading" << "music" << "travel";
```

### 删除结点（暂不支持）

目前 xyjson 未封装删除结点的功能，如果需要删除结点，可以获取底层指针直接操作：

```cpp
// 获取底层指针直接操作（高级用法）
yyjson_mut_val* nodePtr;
if (mutDoc / "to_delete" >> nodePtr) {
    // 使用 yyjson C API 删除结点
}
```

## 迭代器使用

### 创建迭代器

```cpp
// 数组迭代器
for (auto iter = doc / "items" % 0; iter; ++iter) {
    std::cout << "数组索引: " << iter->key << std::endl;
    std::cout << "值: " << (iter->value | "") << std::endl;
}

// 对象迭代器
for (auto iter = doc / "user" % ""; iter; ++iter) {
    std::cout << "键: " << iter->key << std::endl;
    std::cout << "值: " << (iter->value | "") << std::endl;
}
```

### 迭代器自增步进

```cpp
// 步进操作
auto iter = doc / "items" % 0;
++iter;  // 前进到下一个元素

// 跳跃前进
iter += 3;  // 跳过3个元素

// 搜索前进
iter %= "target";  // 搜索特定键或值
```

### 迭代器跳跃搜索

```cpp
// 在数组中搜索特定值
for (auto iter = doc / "users" % 0; iter; iter %= 25) {
    if (iter->value / "age" | 0 == 25) {
        std::cout << "找到年龄25的用户: " << (iter->value / "name" | "") << std::endl;
        break;
    }
}

// 在对象中搜索特定键
for (auto iter = doc / "config" % ""; iter; iter %= "debug") {
    if (iter->key == "debug") {
        bool debugMode = iter->value | false;
        std::cout << "调试模式: " << debugMode << std::endl;
        break;
    }
}
```

## 高级用法

### 一元操作符

```cpp
// 转换为数字
int size = +(doc / "items");  // 获取数组/对象大小

// 转换为字符串
std::string str = -(doc / "name");

// 解引用获取根结点
yyjson::Value root = *doc;

// 模式转换
yyjson::MutableDocument mutableCopy = ~doc;  // 只读转可写
yyjson::Document readonlyCopy = ~mutDoc;     // 可写转只读
```

### 比较操作

```cpp
// 深度比较
if (doc1 == doc2) {
    std::cout << "两个文档内容相同" << std::endl;
}

// 不等性判断
if (doc / "version" != "1.0") {
    std::cout << "版本不是1.0" << std::endl;
}
```

### 管道函数自定义转换

```cpp
// 自定义转换管道
struct User {
    std::string name;
    int age;
};

User user = doc / "user" | [](const yyjson::Value& v) -> User {
    return {
        v / "name" | "",
        v / "age" | 0
    };
};

// 链式管道
std::string result = doc / "data" 
    | [](const yyjson::Value& v) { return v.toString(); }
    | [](const std::string& s) { return "处理结果: " + s; }
    | "默认值";
```

## 性能优化实践

### 保存中间结点

```cpp
// 避免重复路径访问（推荐）
yyjson::Value userNode = doc / "user" / "profile";
std::string name = userNode / "name" | "";
int age = userNode / "age" | 0;
std::string email = userNode / "contact" / "email" | "";

// 不推荐的方式（多次路径解析）
std::string name = doc / "user" / "profile" / "name" | "";
int age = doc / "user" / "profile" / "age" | 0;
```

### 遍历时使用迭代器

```cpp
// 高效遍历（推荐）
for (auto iter = doc / "large_array" % 0; iter; ++iter) {
    processItem(iter->value);
}

// 低效方式（多次索引计算）
int size = +(doc / "large_array");
for (int i = 0; i < size; i++) {
    processItem(doc / "large_array" / i);
}
```

### 字符串优化

```cpp
// 字面量优化（零拷贝）
mutDoc / "status" = "active";           // 字面量，引用优化

// 动态字符串（需要拷贝）
std::string dynamicStr = "active";
mutDoc / "status" = dynamicStr;          // 需要拷贝

// 使用引用方法避免拷贝
mutDoc.setStringRef("status", "active"); // 引用字面量
```

## 常见错误与陷阱

### Document 类不可拷贝

```cpp
yyjson::Document doc1("{}");
// yyjson::Document doc2 = doc1;  // 错误！Document 不可拷贝

// 正确方式：使用模式转换
yyjson::MutableDocument mutCopy = ~doc1;  // 创建可写副本
```

### 路径操作符 `/` 的限制

```cpp
// / 操作符只读，不能自动添加结点
yyjson::Document doc("{}");
// doc / "new_field" = "value";  // 错误！只读文档不能赋值

// 正确方式：使用可写文档
yyjson::MutableDocument mutDoc("{}");
mutDoc / "new_field" = "value";           // 正确
// 或使用 [] 操作符自动插入
mutDoc["another_field"] = "value";        // 正确，自动插入
```

### 对非标量叶结点取值无效

```cpp
yyjson::Document doc(R"({"user": {"name": "Alice"}})");

// 错误：对对象结点使用 | 操作符
// std::string userName = doc / "user" | "";  // 编译错误

// 正确：继续访问子结点
std::string userName = doc / "user" / "name" | "";
```

### 输入操作符 `<<` 的多重含义

```cpp
yyjson::MutableDocument doc("{}");

// 对数组：追加元素
doc / "array" = "[]";
doc / "array" << 1 << 2 << 3;          // 追加 [1, 2, 3]

// 对对象：插入键值对
doc / "object" = "{}";
doc / "object" << "key1" << "value1";  // 插入 {"key1": "value1"}

// 对标量：赋值
doc / "scalar" << "new_value";          // 赋值 "new_value"

// 注意：业务逻辑中不要混淆这些用法
```

## 未实现的功能

### 部分 yyjson 功能未封装

xyjson 封装了 yyjson 的核心功能，但部分高级功能需要通过底层 API 访问：

- **结点删除**：使用 `yyjson_mut_obj_remove()` 等 C API
- **自定义分配器**：通过底层指针设置
- **流式解析**：直接使用 yyjson C API

### 获取底层指针直接操作

```cpp
// 获取底层指针进行高级操作
yyjson_mut_val* rootPtr;
if (mutDoc >> rootPtr) {
    // 使用 yyjson C API 直接操作
    yyjson_mut_obj_remove_key(rootPtr, "to_remove");
}

// 获取文档指针
yyjson_mut_doc* docPtr;
if (mutDoc >> docPtr) {
    // 使用完整的 yyjson C API
}
```

## 总结

xyjson 通过操作符重载提供了直观的 JSON 操作体验，同时保持了底层 yyjson 的高性能特性。在实际使用中：

1. **优先使用操作符**：语法简洁直观
2. **注意模式选择**：只读用 `Document`，修改用 `MutableDocument`
3. **善用优化技巧**：缓存中间结果，使用迭代器遍历
4. **了解限制**：熟悉常见错误和未实现的功能

更多详细 API 参考请查看 [API 文档](api.md)，设计理念请查看 [设计文档](design.md)。
