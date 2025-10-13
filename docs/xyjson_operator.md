# yyjson C++ 封装库操作符重载使用详解

`xyjson` 是一个基于高性能 [yyjson](https://github.com/ibireme/yyjson) 库的 C++ 封装，通过丰富的**操作符重载**提供简洁、直观的 JSON 操作体验。该库支持只读和可写两种模式，让 JSON 处理变得像原生 C++ 代码一样自然流畅。

## 操作符总览表

| 操作符号 | 对应方法名 | 简要说明 | 操作符优先级 | 推荐等级 |
|---------|-----------|---------|------------|---------|
| `[]` | `index()` | 数组/对象索引访问 | 2 | 1 |
| `/` | `pathto()` | 路径访问（支持链式和JSON Pointer） | 5 | 1 |
| `=` | `set()` | 赋值操作（仅MutableValue） | 16 | 1 |
| `\|` | `getor()` | 值提取（带默认值） | 12 | 1 |
| `<<` | `input()` | 智能输入（数组追加/对象插入/标量赋值） | 7 | 2 |
| `>>` | `get()` | 安全值提取（返回bool） | 7 | 2 |
| `+` | `toNumber()` | 转换为数字（数组/对象返回size） | 3 | 2 |
| `-` | `toString()` | 转换为字符串 | 3 | 2 |
| `!` | `hasError()` | 逻辑非（检查错误） | 4 | 2 |
| `~` | `mutate()`/`freeze()` | 文档模式转换 | 3 | 3 |
| `%` | `arrayIter()`/`objectIter()` | 迭代器创建 | 5 | 3 |
| `\|` (函数) | `pipe()` | 管道操作（自定义函数转换） | 12 | 3 |
| `<<` (流) | `toString()` | 流输出到std::ostream | 7 | 2 |
| `==`/`!=` | `equal()` | 相等性比较 | 10 | 2 |
| `\|=` | `get()` | 复合赋值提取 | 16 | 2 |
| `++`/`+=`/`%=` | `next()`/`advance()`/`seek()` | 迭代器操作 | 3/16/5 | 3 |

> **说明**：
> - **操作符优先级**：遵循C++标准，数值越小优先级越高
> - **推荐等级**：数值越小越推荐使用操作符而非方法名（1=强烈推荐，2=推荐，3=可选）

## 核心设计理念

- **操作符重载优先**：通过重载 C++ 操作符，提供类似原生语言的 JSON 操作语法
- **零拷贝性能**：底层基于 yyjson，保持高性能和低内存占用
- **类型安全**：编译时类型检查，运行时安全的值提取
- **链式操作**：支持流畅的链式调用和操作符组合

## 主要类结构

### 只读模式
- **`Value`**：只读 JSON 值包装器
- **`Document`**：只读 JSON 文档包装器

### 可写模式  
- **`MutableValue`**：可写 JSON 值包装器
- **`MutableDocument`**：可写 JSON 文档包装器

### 迭代器
- **`ArrayIterator`** / **`MutableArrayIterator`**：数组迭代器
- **`ObjectIterator`** / **`MutableObjectIterator`**：对象迭代器

## 核心操作符重载

### 1. 路径访问操作符 `/`

**语法**：`json / path` 或 `doc / path`

**功能**：支持链式路径访问和 JSON Pointer 语法

```cpp
// 链式访问
int value = doc / "user" / "profile" / "age" | 0;

// JSON Pointer 语法（必须以 `/` 开头）
int value = doc / "/user/profile/age" | 0;  // 标准 JSON Pointer（推荐）

// 数组索引
std::string item = doc / "items" / 2 | "";

// ⚠️ 重要注意事项：
// 多层路径必须以 '/' 开头，否则会被视为单层索引
int value1 = doc / "/user/profile/age" | 0;  // ✅ 正确：使用标准 JSON Pointer
int value2 = doc / "user/profile/age" | 0;   // ❌ 错误：会被当作单层键名查找
```

### 2. 值提取操作符 `|`

**语法**：`json | defaultValue`

**功能**：安全地提取值，如果路径不存在或类型不匹配则返回默认值

```cpp
// 基本类型提取
int age = doc / "age" | 0;
std::string name = doc / "name" | "unknown";
bool active = doc / "active" | false;

// 字符串提取（支持 const char* 和 std::string）
const char* title = doc / "title" | "";
std::string content = doc / "content" | std::string();

// 复杂类型提取
double price = doc / "price" | 0.0;
uint64_t id = doc / "id" | 0ULL;
```

### 3. 赋值操作符 `=`

**语法**：`mutableJson = value`

**功能**：为可写 JSON 值设置新值（仅适用于 `MutableValue`）

```cpp
// 基本赋值，仅对已存在键赋值，不会自动插入不存在的键（[]才可自动插入）
doc / "name" = "Alice";
doc / "age" = 25;
doc / "active" = true;

// 字符串赋值（自动处理字符串字面量）
doc / "message" = "Hello World";
doc / "empty" = "";  // 空字符串

// 特殊字面量处理
doc / "config" = "{}";  // 自动创建空对象
doc / "list" = "[]";    // 自动创建空数组
```

### 4. 输入操作符 `<<`

**语法**：`mutableJson << value`

**功能**：智能输入操作符，根据目标类型自动选择操作：
- **数组**：追加元素
- **对象**：插入键值对（配合 pending key）
- **标量**：直接赋值

```cpp
// 数组追加
doc.root() << 1 << "two" << 3.14 << false;

// 对象插入（链式 key-value）
doc.root() << "name" << "Alice" << "age" << 25;

// 使用 KV 宏（推荐）
doc.root() << KV("name", "Bob") << KV("score", 95);
```

### 5. 输出操作符 `>>`

**语法**：`json >> variable`

**功能**：安全提取值到变量，返回是否成功

```cpp
int age;
if (doc / "age" >> age) {
    // 提取成功
    std::cout << "Age: " << age << std::endl;
} else {
    // 提取失败（路径不存在或类型不匹配）
    std::cout << "Age not found or invalid type" << std::endl;
}
```

### 6. 一元操作符 `+` 和 `-`

**语法**：
- `+json`：转换为数字
- `-json`：转换为字符串

**功能**：提供简洁的类型转换

```cpp
// 数字转换（+操作符）
int count = +(doc / "items");        // 数组/对象返回 size()
int number = +(doc / "value");       // 数字返回值，字符串尝试转换
int boolean = +(doc / "flag");       // 布尔返回 1/0

// 字符串转换（-操作符）
std::string jsonStr = -(doc / "data");  // 返回 JSON 字符串表示
std::string text = -(doc / "message");  // 字符串值直接返回
```

### 7. 文档转换操作符 `~`

**语法**：
- `~document`：只读文档转可写文档
- `~mutableDocument`：可写文档转只读文档

**功能**：在只读和可写模式间无缝转换

```cpp
// 只读转可写
yyjson::MutableDocument mutableDoc = ~readOnlyDoc;

// 可写转只读  
yyjson::Document readOnlyDoc = ~mutableDoc;
```

### 8. 迭代器创建操作符 `%`

**语法**：
- `json % index`：创建数组迭代器
- `json % key`：创建对象迭代器

**功能**：提供简洁的迭代器创建语法

```cpp
// 数组迭代
for (auto iter = doc / "items" % 0; iter; ++iter) {
    std::cout << "Item[" << iter->key << "] = " 
              << (iter->value | "") << std::endl;
}

// 对象迭代
for (auto iter = doc / "user" % ""; iter; ++iter) {
    std::cout << "Key: " << iter->key 
              << ", Value: " << (iter->value | "") << std::endl;
}
```

## 高级特性

### KeyValue 优化

通过 `*` 操作符创建优化的键值对，避免重复的字符串拷贝：

```cpp
// 创建键值对
auto kv = doc.create("value") * "key";
doc.root().add(kv);

// 链式操作（推荐）
doc.root() << (doc * "hello" * "greeting");
doc.root() << (doc * 42 * "number");

// 字符串字面量自动引用优化
doc.root() << (doc * "literal" * "key");  // key 使用引用而非拷贝
```

### 管道操作符 `|`

支持自定义函数转换，实现函数式编程风格：

```cpp
// 自定义转换函数
std::string toUppercase(const yyjson::Value& value) {
    std::string str = value | "";
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// 使用管道操作符
std::string upperName = doc / "name" | toUppercase;

// Lambda 表达式支持
std::string prefixed = doc / "name" | [](const auto& val) {
    return "Hello " + (val | std::string());
};
```

### 迭代器操作符

迭代器支持丰富的操作符重载：

```cpp
// 前置/后置递增
++iter;    // iter.next()
iter++;    // iter.Next()

// 位置跳转
iter += 3;     // advance(3)
iter %= "key"; // seek("key")

// 比较操作
if (iter1 == iter2) { /* ... */ }
if (iter) { /* valid */ }
if (!iter) { /* invalid */ }
```

## 基本使用示例

### 1. 读取 JSON

```cpp
#include "yyjson_operator.h"

// 从字符串创建文档
std::string jsonText = R"({"name": "Alice", "age": 30, "active": true})";
yyjson::Document doc(jsonText);

// 安全提取值
std::string name = doc / "name" | "unknown";
int age = doc / "age" | 0;
bool active = doc / "active" | false;

// 路径访问
int deepValue = doc / "settings" / "threshold" | 0;
```

### 2. 创建和修改 JSON

```cpp
// 创建可写文档
yyjson::MutableDocument doc("{}");

// 设置值，使用 [] 自动插入，/ 操作符不能新增字段
doc["new_field"] = "auto_inserted";
doc["name"] = "Bob";
doc["age"] = 25;
doc["scores"] = "[]";  // 创建空数组

// 追加数组元素
doc / "scores" << 95 << 87 << 92;

// 插入对象属性
doc.root() << KV("department", "Engineering") 
           << KV("level", 3);
```

### 3. 文件操作

```cpp
// 读取文件
yyjson::Document doc;
doc.readFile("config.json");

// 写入文件
yyjson::MutableDocument mutableDoc;
// ... 修改文档 ...
mutableDoc.writeFile("output.json");

// 流操作符
std::ifstream ifs("input.json");
doc << ifs;

std::ofstream ofs("output.json");
doc >> ofs;
```

### 4. 迭代遍历

```cpp
// 数组遍历
auto items = doc / "items";
for (auto iter = items.beginArray(); iter != items.endArray(); ++iter) {
    std::cout << "Item " << iter->key << ": " << -(iter->value) << std::endl;
}

// 对象遍历  
auto user = doc / "user";
for (auto iter = user.beginObject(); iter != user.endObject(); ++iter) {
    std::cout << iter->key << " = " << -(iter->value) << std::endl;
}

// 简洁语法
for (auto iter = doc / "items" % 0; iter; ++iter) {
    // 处理每个元素
}
```

## 性能优化特性

### 1. 字符串引用优化

- **字符串字面量**：自动使用引用而非拷贝，提高性能
- **`StringRef` 类型**：显式控制字符串存储策略
- **`setRef()` / `setCopy()`**：手动选择引用或拷贝策略

### 2. 内存管理

- **RAII 自动释放**：Document 和 MutableDocument 自动管理内存
- **零拷贝设计**：底层 yyjson 的零拷贝特性得以保留
- **高效的节点创建**：通过 `create()` 方法批量创建节点

### 3. 类型特征支持

提供编译时类型特征，支持模板元编程：

```cpp
// 类型特征
yyjson::is_value<Value>::value        // true
yyjson::is_document<Document>::value  // true  
yyjson::is_iterator<ArrayIterator>::value // true

// C++17 变量模板
yyjson::is_value_v<Value>             // true
```

## 错误处理

### 1. 有效性检查

```cpp
// 显式检查
if (doc.isValid()) { /* ... */ }
if (!doc.hasError()) { /* ... */ }

// 隐式转换（布尔上下文）
if (doc) { /* valid */ }
if (!doc) { /* invalid */ }

// 值检查
if (value) { /* valid value */ }
if (!value) { /* invalid/null */ }
```

### 2. 安全提取

所有提取操作都是安全的，不会抛出异常：

```cpp
// 使用默认值
int safeValue = json / "path" | 0;

// 使用输出操作符检查成功
int value;
if (json / "path" >> value) {
    // 成功提取
}
```

## 最佳实践

### 1. 路径访问优化
- **多层路径**：必须使用 `/` 开头的标准 JSON Pointer 语法
  ```cpp
  // ✅ 正确用法
  doc / "/user/profile/age" | 0;
  doc / "/config/settings/theme" | "";
  
  // ❌ 错误用法
  doc / "user/profile/age" | 0;      // 会被当作单层键名
  doc / "config/settings/theme" | ""; // 同上
  ```
- **链式访问**：对于固定路径，链式访问更直观
  ```cpp
  // 链式访问（推荐用于固定路径）
  doc / "user" / "profile" / "age" | 0;
  ```

### 2. 选择合适的模式
- **只读操作**：使用 `Document` 和 `Value`
- **修改操作**：使用 `MutableDocument` 和 `MutableValue`
- **混合场景**：通过 `~` 操作符在两种模式间转换

### 3. 字符串处理
- 优先使用字符串字面量以获得引用优化
- 对于动态字符串，考虑使用 `StringRef` 显式控制

### 4. 批量操作
- 使用 `create()` 方法预创建节点
- 利用 KeyValue 优化减少重复操作

### 5. JSON Pointer 转义规则
- `/` 字符转义为 `~1`：`doc / "/a~1b" | ""` 对应键名 `"a/b"`
- `~` 字符转义为 `~0`：`doc / "/c~0d" | ""` 对应键名 `"c~d"`

## 总结

`xyjson` 通过精心设计的操作符重载，将 JSON 操作提升到了 C++ 原生语法的体验水平。其核心优势包括：

- **直观的语法**：操作符重载让 JSON 操作如行云流水
- **高性能**：基于 yyjson，保持零拷贝和高效内存管理
- **类型安全**：编译时检查和运行时安全提取
- **功能完整**：支持所有 JSON 操作场景，从简单读取到复杂修改

通过掌握这些操作符的使用，开发者可以写出既简洁又高效的 JSON 处理代码，大大提升开发体验和代码可读性。