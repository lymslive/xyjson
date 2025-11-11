# xyjson API 参考手册

本手册提供 xyjson 库的完整 API 参考，作为查询手册使用，内容详尽、务实。

## 核心类介绍

所有类均在 `yyjson` 命名空间下定义：

- **Document** - 只读 JSON 文档包装器
- **MutableDocument** - 可写 JSON 文档包装器
- **Value** - 只读 JSON 值包装器
- **MutableValue** - 可写 JSON 值包装器
- **ArrayIterator** - 只读数组迭代器
- **MutableArrayIterator** - 可写数组迭代器
- **ObjectIterator** - 只读对象迭代器
- **MutableObjectIterator** - 可写对象迭代器

详细的类层次关系请参考：[类图](class_diagram.puml)

### 泛型类型约定

本文档使用以下泛型名称：

- **jsonT** - Value 与 MutableValue 类型统称
- **docT** - Document 与 MutableDocument 类型统称
- **iteratorT** - 四种迭代器类型统称
- **scalarT** - 数字与字符串等基本标量类型统称

## 操作符总览表

| 优先级 | 操作符 | 适用类 | 左侧参数 | 右侧参数 | 功能描述 | 推荐度 |
|--------|--------|--------|----------|----------|----------|--------|
| 2 | `[]` | 所有类 | jsonT/docT | `const char*`, `int` | 单层索引访问 | ★★★★★ |
| 3 | `!` | 所有类 | 任意类型 | 无 | 错误判断/逻辑非 | ★★★★☆ |
| 3 | `~` | Document类 | docT | 无 | 模式转换（可读写互转） | ★★★☆☆ |
| 3 | `+` | Value类 | jsonT | 无 | 一元正号，转整数 | ★★★★☆ |
| 3 | `+` | Document类 | docT | 无 | 一元正号，取根结点转整数 | ★★★★☆ |
| 3 | `-` | Value类 | jsonT | 无 | 一元负号，转字符串 | ★★★★☆ |
| 3 | `-` | Document类 | docT | 无 | 一元负号，取根结点转字符串 | ★★★★☆ |
| 3 | `*` | Document类 | docT | 无 | 一元解引用，取根结点 | ★★★★☆ |
| 3 | `++` | 迭代器类 | iteratorT | 无 | 迭代器自增 | ★★★☆☆ |
| 5 | `/` | 所有类 | jsonT/docT | `const char*`, `int` | 路径访问，支持 JSON Pointer | ★★★★★ |
| 5 | `*` | 所有类 | MutableValue | scalarT | 二元乘，创建 JSON 结点 | ★★★☆☆ |
| 5 | `%` | 所有类 | jsonT/docT | `int`, `const char*` | 创建迭代器 | ★★★☆☆ |
| 7 | `<<` | MutableValue | jsonT | 各种类型 | 智能输入（追加/插入） | ★★★★☆ |
| 7 | `<<` | Document类 | docT | `std::ostream` | 流输出 | ★★★★☆ |
| 7 | `>>` | Value类 | jsonT | 引用类型 | 安全值提取 | ★★★★☆ |
| 10 | `==` | 所有类 | 任意类型 | 任意类型 | 相等性判断 | ★★★★☆ |
| 10 | `!=` | 所有类 | 任意类型 | 任意类型 | 不等性判断 | ★★★★☆ |
| 11 | `&` | Value类 | jsonT | scalarT/EmptyXXX | 类型判断 | ★★★★☆ |
| 12 | `\|` | Value类 | jsonT | scalarT/函数/EmptyXXX | 值提取/管道函数 | ★★★★★ |
| 16 | `=` | MutableValue | jsonT | scalarT/jsonT | 赋值操作 | ★★★★★ |
| 16 | `|=` | MutableValue | jsonT& | scalarT | 复合赋值提取 | ★★★★☆ |
| 16 | `+=` | 迭代器类 | iteratorT& | `int` | 迭代器步进 | ★★★☆☆ |

> **重要说明**：
> 1. **`.` 操作符** - 不可重载，用于调用成员方法，优先级最高（2）
> 2. **`_xyjson` 字面量操作符** - 用户定义字面量，优先级最高（无表达式优先级问题）
> 3. 操作符重载不能改变 C++ 固有的优先级和结合律，使用时需注意操作符的正确组合

## `/` 路径操作符

**语法**：`jsonT / path` 或 `docT / path`

**功能**：支持链式路径访问和 JSON Pointer 语法

- **左侧参数类型**：`jsonT` 或 `docT`
- **右侧参数类型**：`const char*`（路径字符串）或 `int`（数组索引）
- **返回值类型**：`jsonT`（与左侧参数相同类型）

### 基本路径访问

```cpp
/* use operator */
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
auto node = doc / "user" / "name";

/* use method */
yyjson::Document doc2 = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
auto node2 = doc2.root().pathto("user").pathto("name");
```

### JSON Pointer 路径

```cpp
/* use operator */
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
auto node = doc / "/user/name";

/* use method */
yyjson::Document doc2 = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
auto node2 = doc2.root().pathto("/user/name");
```

### 数组索引访问

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
auto item = doc / "items" / 0;

/* use method */
yyjson::Document doc2 = R"({"items": [10, 20, 30]})"_xyjson;
auto item2 = doc2.root().pathto("items").index(0);
```

## `|` 取值操作符

**语法**：`jsonT | defaultValue` 或 `jsonT | function`

**功能**：从 JSON 结点提取值，支持默认值和管道函数

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`scalarT`（基本类型）、函数、类型常量
- **返回值类型**：与右侧参数类型相同

### 基本类型提取

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice", "age": 30, "active": true})"_xyjson;
std::string name = doc / "name" | "";
int age = doc / "age" | 0;
bool active = doc / "active" | false;

/* use method */
yyjson::Document doc2 = R"({"name": "Alice", "age": 30, "active": true})"_xyjson;
std::string name2 = doc2 / "name".getor("");
int age2 = doc2 / "age".getor(0);
bool active2 = doc2 / "active".getor(false);
```

### 管道函数转换

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "alice"})"_xyjson;
std::string upper = doc / "name" | [](const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::toupper);
    return r;
} | "DEFAULT";

/* use method */
yyjson::Document doc2 = R"({"name": "alice"})"_xyjson;
std::string upper2 = doc2 / "name".pipe([](const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::toupper);
    return r;
}).getor("DEFAULT");
```

### 使用类型常量

```cpp
/* use operator */
yyjson::Document doc = R"({"score": [10, 20, 30]})"_xyjson;
auto arr = doc / "score" | yyjson::kArray;
int size = +(doc / "score" | yyjson::kArray);

/* use method */
yyjson::Document doc2 = R"({"score": [10, 20, 30]})"_xyjson;
auto arr2 = doc2 / "score".getor(yyjson::kArray);
```

## `&=` 复合赋值提取

**语法**：`scalarT& |= jsonT`

**功能**：使用变量原来的默认值提取值

- **左侧参数类型**：`scalarT&`
- **右侧参数类型**：`jsonT`
- **返回值类型**：`scalarT&`

```cpp
/* use operator */
int age = 18;
yyjson::Document doc = R"({"age": 30})"_xyjson;
age |= doc / "age";  // age = 30

/* use method */
int age2 = 18;
yyjson::Document doc2 = R"({"age": 30})"_xyjson;
doc2 / "age".get(age2);  // age2 = 30
```

## `&` 类型判断操作符

**语法**：`jsonT & typeValue`

**功能**：判断 JSON 结点类型

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`scalarT` 或 `EmptyXXX` 类型常量
- **返回值类型**：`bool`

### 基本类型判断

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice", "age": 30})"_xyjson;
bool isString = doc / "name" & "";
bool isNumber = doc / "age" & 0;
bool isObject = doc & "{}";
bool isArray = doc & "[]";

/* use method */
yyjson::Document doc2 = R"({"name": "Alice", "age": 30})"_xyjson;
bool isString2 = doc2 / "name".isType("");
bool isNumber2 = doc2 / "age".isType(0);
```

### 使用类型常量

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice", "age": 30, "items": [1,2,3]})"_xyjson;
bool isString = doc / "name" & yyjson::kString;
bool isNumber = doc / "age" & yyjson::kInt;
bool isArray = doc / "items" & yyjson::kArray;
bool isObject = doc & yyjson::kObject;

/* use method */
yyjson::Document doc2 = R"({"name": "Alice", "age": 30, "items": [1,2,3]})"_xyjson;
bool isString2 = doc2 / "name".isType(yyjson::kString);
bool isNumber2 = doc2 / "age".isType(yyjson::kInt);
bool isArray2 = doc2 / "items".isType(yyjson::kArray);
```

## `[]` 单层索引操作符

**语法**：`jsonT[key]` 或 `jsonT[index]`

**功能**：单层索引访问，MutableValue 可自动创建新字段

- **左侧参数类型**：`jsonT` 或 `docT`
- **右侧参数类型**：`const char*`（对象键）或 `int`（数组索引）
- **返回值类型**：`jsonT`（与左侧参数相同类型）

### 对象键访问

```cpp
/* use operator */
yyjson::Document doc = R"({"user": {"name": "Alice"}})"_xyjson;
auto name = doc["user"]["name"];

/* use method */
yyjson::Document doc2 = R"({"user": {"name": "Alice"}})"_xyjson;
auto name2 = doc2.root().index("user").index("name");
```

### 数组索引访问

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
auto first = doc["items"][0];

/* use method */
yyjson::Document doc2 = R"({"items": [10, 20, 30]})"_xyjson;
auto first2 = doc2.root().index("items").index(0);
```

### 可写模型的自动插入

```cpp
/* use operator */
yyjson::MutableDocument mutDoc;
mutDoc["new_field"] = "value";
mutDoc["array"] = "[]";
mutDoc / "array" << 1 << 2 << 3;

/* use method */
yyjson::MutableDocument mutDoc2;
mutDoc2.root().index("new_field").set("value");
mutDoc2.root().index("array").set("[]");
```

## `=` 赋值操作符

**语法**：`jsonT = value`

**功能**：为 MutableValue 赋值

- **左侧参数类型**：`MutableValue`
- **右侧参数类型**：`scalarT`、`jsonT`、或类型常量
- **返回值类型**：`MutableValue&`

```cpp
/* use operator */
yyjson::MutableDocument mutDoc = R"({"name": "Alice"})"_xyjson;
mutDoc / "name" = "Bob";
mutDoc / "age" = 30;
mutDoc / "active" = true;

/* use method */
yyjson::MutableDocument mutDoc2 = R"({"name": "Alice"})"_xyjson;
mutDoc2 / "name".set("Bob");
mutDoc2 / "age".set(30);
mutDoc2 / "active".set(true);
```

## `<<` 智能输入操作符

**语法**：`jsonT << value` 或 `doc << stream`

**功能**：向容器添加元素，或输出到流

- **左侧参数类型**：`MutableValue` 或 `Document`
- **右侧参数类型**：各种类型（对于 MutableValue），`std::ostream`（对于 Document）
- **返回值类型**：`MutableValue&`（对于 MutableValue），`std::ostream&`（对于 Document）

### 数组追加

```cpp
/* use operator */
yyjson::MutableDocument mutDoc;
mutDoc["items"] = "[]";
mutDoc / "items" << 1 << "two" << 3.14;

/* use method */
yyjson::MutableDocument mutDoc2;
mutDoc2.root().index("items").set("[]");
mutDoc2.root().index("items").push(1).push("two").push(3.14);
```

### 对象插入键值对

```cpp
/* use operator */
yyjson::MutableDocument mutDoc;
auto root = *mutDoc;
root << "key1" << "value1" << "key2" << 42;

/* use method */
yyjson::MutableDocument mutDoc2;
auto root2 = *mutDoc2;
root2.add("key1", "value1").add("key2", 42);
```

### 标准流输出

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::cout << doc << std::endl;

/* use method */
yyjson::Document doc2 = R"({"name": "Alice"})"_xyjson;
std::cout << doc2.root().toString() << std::endl;
```

## `>>` 安全值提取操作符

**语法**：`jsonT >> target`

**功能**：安全值提取，返回 bool 表示是否成功

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`scalarT&` 或底层指针类型
- **返回值类型**：`bool`

```cpp
/* use operator */
int age;
std::string name;
if (doc / "age" >> age) {
    // 成功提取
}
if (doc / "name" >> name) {
    // 成功提取
}

/* use method */
int age2;
std::string name2;
if (doc2 / "age".get(age2)) {
    // 成功提取
}
if (doc2 / "name".get(name2)) {
    // 成功提取
}
```

## `*` 解引用操作符

### 一元解引用（取根结点）

**语法**：`*docT`

**功能**：从 Document 获取根结点

- **参数类型**：`docT`
- **返回值类型**：`jsonT`（Document 对应的 Value 类型）

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
auto root = *doc;
yyjson::MutableDocument mutDoc = R"({"name": "Alice"})"_xyjson;
auto mutRoot = *mutDoc;

/* use method */
auto root2 = doc.root();
auto mutRoot2 = mutDoc.root();
```

### 二元乘（创建 JSON 结点）

**语法**：`mutDoc * value` 或 `key * value`

**功能**：创建新的 JSON 结点

- **左侧参数类型**：`MutableDocument` 或 `MutableValue`（作为 key）
- **右侧参数类型**：`scalarT` 或 `MutableValue`
- **返回值类型**：`MutableValue` 或 `KeyValue`

```cpp
/* use operator */
yyjson::MutableDocument mutDoc;
auto newNode = mutDoc * "new_value";
auto newArray = mutDoc * "[]";

/* use method */
auto newNode2 = mutDoc.create("new_value");
auto newArray2 = mutDoc.create("[]");
```

## `+` 一元正号操作符

**语法**：`+jsonT` 或 `+docT`

**功能**：将 JSON 转换为整数

- **参数类型**：`jsonT` 或 `docT`
- **返回值类型**：`int`

### Value 类

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
int size = +(doc / "items");  // 获取数组大小

/* use method */
int size2 = doc / "items".toInteger();
```

### Document 类

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
int rootSize = +doc;  // 相当于 +(doc.root())

/* use method */
int rootSize2 = doc.root().toInteger();
```

## `-` 一元负号操作符

**语法**：`-jsonT` 或 `-docT`

**功能**：将 JSON 转换为字符串

- **参数类型**：`jsonT` 或 `docT`
- **返回值类型**：`std::string`

### Value 类

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::string str = -(doc / "name");

/* use method */
std::string str2 = doc / "name".toString();
```

### Document 类

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::string rootStr = -doc;  // 相当于 -(doc.root())

/* use method */
std::string rootStr2 = doc.root().toString();
```

## `~` 模式转换操作符

**语法**：`~docT`

**功能**：只读与可写文档互转

- **参数类型**：`Document` 或 `MutableDocument`
- **返回值类型**：`MutableDocument` 或 `Document`

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
yyjson::MutableDocument mutCopy = ~doc;
yyjson::Document readOnlyCopy = ~mutCopy;

/* use method */
yyjson::MutableDocument mutCopy2 = doc.mutate();
yyjson::Document readOnlyCopy2 = mutDoc.freeze();
```

## `%` 迭代器创建操作符

**语法**：`jsonT % startPos`

**功能**：创建迭代器

- **左侧参数类型**：`jsonT` 或 `docT`
- **右侧参数类型**：`int`（数组起始索引）或 `const char*`（对象起始键）
- **返回值类型**：`iteratorT`

### 数组迭代器

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    // 处理数组元素
}

/* use method */
yyjson::Document doc2 = R"({"items": [10, 20, 30]})"_xyjson;
for (auto iter = doc2 / "items".iterator(0); iter; iter.next()) {
    // 处理数组元素
}
```

### 对象迭代器

```cpp
/* use operator */
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
for (auto iter = doc / "user" % ""; iter; ++iter) {
    // 处理对象键值对
}

/* use method */
yyjson::Document doc2 = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
for (auto iter = doc2 / "user".iterator(""); iter; iter.next()) {
    // 处理对象键值对
}
```

## `%=` 迭代器搜索操作符

**语法**：`iteratorT %= target`

**功能**：迭代器重新定位搜索

- **左侧参数类型**：`iteratorT&`
- **右侧参数类型**：`int` 或 `const char*`
- **返回值类型**：`iteratorT&`

```cpp
/* use operator */
auto iter = doc / "items" % 0;
iter %= 5;  // 搜索索引为 5 的元素

/* use method */
auto iter2 = doc / "items".iterator(0);
iter2.begin().advance(5);
```

## `++` 迭代器自增操作符

**语法**：`++iteratorT` 或 `iteratorT++`

**功能**：迭代器前进

- **参数类型**：`iteratorT&` 或 `iteratorT`
- **返回值类型**：`iteratorT&` 或 `iteratorT`

```cpp
/* use operator */
auto iter = doc / "items" % 0;
++iter;  // 前缀自增

/* use method */
auto iter2 = doc / "items".iterator(0);
iter2.next();
```

## `+=` 迭代器步进操作符

**语法**：`iteratorT += step`

**功能**：迭代器前进指定步数

- **左侧参数类型**：`iteratorT&`
- **右侧参数类型**：`size_t`
- **返回值类型**：`iteratorT&`

```cpp
/* use operator */
auto iter = doc / "items" % 0;
iter += 3;  // 跳过 3 个元素

/* use method */
auto iter2 = doc / "items".iterator(0);
iter2.advance(3);
```

## `!` 错误判断操作符

**语法**：`!jsonT` 或 `!docT` 或 `!iteratorT`

**功能**：判断对象有效性

- **参数类型**：任意类型
- **返回值类型**：`bool`

```cpp
/* use operator */
if (!doc) {
    // 文档无效
}
if (!(doc / "nonexistent")) {
    // 路径不存在
}

/* use method */
if (doc.hasError()) {
    // 文档有错误
}
if (!doc / "path".isValid()) {
    // 结点无效
}
```

## `==` / `!=` 相等性判断操作符

**语法**：`lhs == rhs` 或 `lhs != rhs`

**功能**：判断两个对象是否相等

- **左侧参数类型**：任意类型
- **右侧参数类型**：任意类型
- **返回值类型**：`bool`

### 文档比较

```cpp
/* use operator */
if (doc1 == doc2) {
    // 内容相同
}

/* use method */
if (doc1.root().equal(doc2.root())) {
    // 内容相同
}
```

### 值比较

```cpp
/* use operator */
if (doc / "version" == "1.0") {
    // 版本匹配
}
if (doc / "status" != "active") {
    // 状态不匹配
}

/* use method */
if (doc / "version".equal("1.0")) {
    // 版本匹配
}
if (!doc / "status".equal("active")) {
    // 状态不匹配
}
```

## `.` 成员访问操作符

**语法**：`jsonT.method()` 或 `iteratorT->method()`

**功能**：调用成员方法（不可重载）

- **左侧参数类型**：任意类型
- **右侧参数**：方法名
- **返回值类型**：根据方法定义

```cpp
// 标准方法调用
auto name = doc / "name".toString();
auto size = doc / "items".size();

// 迭代器使用 ->
auto iter = doc / "items" % 0;
if (iter->isValid()) {
    auto value = *iter;
}
```

## `_xyjson` 字面量操作符

**语法**：`"json_string"_xyjson`

**功能**：从字符串字面量直接构造 Document

- **参数类型**：`const char*`, `std::size_t`
- **返回值类型**：`Document`

```cpp
// 直接从字面量构造
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
auto name = doc / "name" | "";
auto age = doc / "age" | 0;
```

## Document 转调 Root 的同义操作符

以下操作符在 Document 上使用时，会自动转调 `root()` 方法：

| 操作符 | 实际调用 | 示例 |
|--------|----------|------|
| `/` | `root() / path` | `doc / "user" / "name"` |
| `%` | `root() % pos` | `doc % 0` 或 `doc % ""` |
| `+` | `+root()` | `+doc`（转整数） |
| `-` | `-root()` | `-doc`（转字符串） |
| `<<` | `root() << value` | `mutDoc << "key" << "value"` |
| `==` | `root() == other.root()` | `doc1 == doc2` |
| `!=` | `root() != other.root()` | `doc1 != doc2` |
| `*` | `*doc` → `root()` | `*doc`（取根结点） |

这些操作符在 Document 上的行为与在 Value 上基本一致，主要用于简化代码。

## 类型常量定义

xyjson 定义了一系列类型代表值常量，用于类型判断和值提取：

```cpp
namespace yyjson {
    // 基础类型常量
    constexpr std::nullptr_t kNull = nullptr;
    constexpr bool kBool = false;
    constexpr int kInt = 0;
    constexpr int64_t kSint = 0L;
    constexpr uint64_t kUint = 0uL;
    constexpr double kReal = 0.0;

    // 特殊标记类型
    struct ZeroNumber {};
    struct EmptyString {};
    struct EmptyArray {};
    struct EmptyObject {};

    // 类型常量实例
    constexpr ZeroNumber kNumber;
    constexpr EmptyString kString;
    constexpr EmptyArray kArray;
    constexpr EmptyObject kObject;

    // 底层指针类型
    constexpr yyjson_val* kNode = nullptr;
    constexpr yyjson_mut_val* kMutNode = nullptr;
    constexpr yyjson_mut_doc* kMutDoc = nullptr;
}
```

### 使用场景

#### 类型判断

```cpp
// 使用字面量
bool isString = doc / "name" & "";
bool isNumber = doc / "age" & 0;
bool isObject = doc / "config" & "{}";
bool isArray = doc / "items" & "[]";

// 使用类型常量（推荐）
bool isString2 = doc / "name" & kString;
bool isNumber2 = doc / "age" & kInt;
bool isObject2 = doc / "config" & kObject;
bool isArray2 = doc / "items" & kArray;
```

#### 值提取

```cpp
// 使用字面量
std::string name = doc / "name" | "";
int age = doc / "age" | 0;
double score = doc / "score" | 0.0;

// 使用类型常量
std::string name2 = doc / "name" | kString;
int age2 = doc / "age" | kInt;
double score2 = doc / "score" | kReal;
```

#### 特殊容器类型

```cpp
// kArray 和 kObject 返回容器包装类
auto arr = doc / "items" | kArray;      // 返回 ConstArray
auto obj = doc / "config" | kObject;    // 返回 ConstObject

// 支持标准容器操作
for (auto& item : arr) {
    // 遍历数组
}
for (auto& [key, value] : obj) {
    // 遍历对象
}
```

### 操作符命名常量

为了方便理解和文档编写，xyjson 定义了操作符的命名常量：

```cpp
namespace yyjson {
    constexpr const char* okExtract = "|";
    constexpr const char* okPipe = "|";
    constexpr const char* okType = "&";
    constexpr const char* okPath = "/";
    constexpr const char* okAssign = "=";
    constexpr const char* okInput = "<<";
    constexpr const char* okOutput = ">>";
    constexpr const char* okEqual = "==";
    constexpr const char* okCreate = "*";
    constexpr const char* okRoot = "*";
    constexpr const char* okNumberify = "+";
    constexpr const char* okStringify = "-";
    constexpr const char* okConvert = "~";
    constexpr const char* okIterator = "%";
    constexpr const char* okIncreace = "++";
}
```

这些常量主要用于文档目的，帮助理解操作符的语义。
