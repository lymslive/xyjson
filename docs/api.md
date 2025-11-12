# xyjson API 参考手册

本手册提供 xyjson 库的完整 API 参考，作为查询手册使用，内容详尽、务实。

## 核心类介绍

所有类均在 `yyjson` 命名空间下定义：

- **Document** - 只读 JSON 文档树包装器
- **MutableDocument** - 可写 JSON 文档树包装器
- **Value** - 只读 JSON 结点值包装器
- **MutableValue** - 可写 JSON 结点值包装器
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

## 操作符重载

### 操作符总览表

<!-- begin operator table -->
| 优先级 | 操作符 | 适用类 | 左参类型 | 右参类型 | 返回类型 | 对应方法 | 功能描述 | 推荐度 |
|--------|--------|--------|----------|----------|----------|----------|----------|--------|
| 0 | `"{}"_xyjson` | 只读文档 | 字符串字面量 | 无 | Document | 构造函数 | 字符串字面量自定义转换 | ★★★☆☆ |
| 2 | `a.b` | 所有类 | 对象 | 成员 | auto | - | 成员访问，不可重载 | ★★★☆☆ |
| 2 | `it->b` | 迭代器 | iteratorT | jsonT 成员方法 | auto | value | 间接成员访问 | ★★★☆☆ |
| 2 | `json->b` | Json结点 | jsonT | jsonT 成员方法 | auto | this | 间接成员访问 | ★★☆☆☆ |
| 2 | `json[key]` | Json结点 | jsonT | 字符串、整数 | jsonT | index | 常规单层索引 | ★★★★☆ |
| 2 | `it++` | 迭代器 | iteratorT | 无 | iteratorT | next | 后缀自增，返回原拷贝 | ★★☆☆☆ |
| 2 | `it--` | 迭代器 | iteratorT | 无 | iteratorT | prev | 后缀自减，返回原拷贝 | ★★☆☆☆ |
| 2 | `bool(a)` | 所有核心类 | 无 | T | bool | isValid | 布尔条件上下文直接判断 | ★★★★☆ |
| 3 | `(bool)a` | - | 无 | T | bool | isValid | 显式转布尔逻辑值 | ★★★☆☆ |
| 3 | `!a` | 所有核心类 | 无 | T | bool | !isValid | 错误判断/无效判断 | ★★★★☆ |
| 3 | `++it` | 迭代器 | iteratorT | 无 | iteratorT& | next | 前缀自增再返回 | ★★★★☆ |
| 3 | `--it` | 迭代器 | iteratorT | 无 | iteratorT& | prev | 前缀自减再返回 | ★★★★☆ |
| 3 | `~doc` | 只读文档 | 无 | Document | MutableDocument | mutate | 可读写模式转换 | ★★★★☆ |
| 3 | `~doc` | 可写文档 | 无 | MutableDocument | Document | freeze | 可读写模式转换 | ★★★★☆ |
| 3 | `~it` | 迭代器 | 无 | iteratorT | jsonT | key | 迭代器当前键结点 | ★★☆☆☆ |
| 3 | `*it` | 迭代器 | 无 | iteratorT | jsonT | value | 迭代器当前值结点 | ★★★★★ |
| 3 | `*doc` | Json文档 | 无 | docT | jsonT | root | 取根结点 | ★★★☆☆ |
| 3 | `(double)json` | Json结点 | 无 | jsonT | double | toNumber | 转浮点数 | ★★★☆☆ |
| 3 | `(int)json` | Json结点 | 无 | jsonT | int | toInteger | 转整数 | ★★★☆☆ |
| 3 | `+json` | Json结点 | 无 | jsonT | int | toInteger | 转整数 | ★★★☆☆ |
| 3 | `+it` | 迭代器 | 无 | iteratorT | size_t | index | 取迭代器当前索引 | ★★☆☆☆ |
| 3 | `(string)json` | Json结点 | 无 | jsonT | std::string | toString | 转字符串 | ★★★☆☆ |
| 3 | `-json` | Json结点 | 无 | jsonT | std::string | toString | 转字符串 | ★★★☆☆ |
| 3 | `-it` | 迭代器 | 无 | iteratorT | const char* | name | 取迭代器当前键名 | ★★☆☆☆ |
| 5 | `json / path` | Json结点 | jsonT | 字符串、整数 | jsonT | pathto | 路径访问，支持 JSON Pointer | ★★★★★ |
| 5 | `it / key` | 对象迭代器 | iteratorT | 字符串 | jsonT | seek | 连续快速查找固定键 | ★★★★★ |
| 5 | `doc * b` | Json文档 | MutableDocument | scalarT | MutableValue | create | 创建 JSON 结点 | ★★★☆☆ |
| 5 | `doc * b` | Json文档 | MutableDocument | jsonT docT | MutableValue | create | 复制 JSON 结点 | ★★★☆☆ |
| 5 | `json * key` | 可写 Json | MutableValue | keyT | KeyValue | tag | 绑定键值对 | ★★★☆☆ |
| 5 | `json % key` | Json结点 | jsonT | 字符串、整数 | iteratorT | iterator | 创建迭代器 | ★★★★☆ |
| 5 | `it % key` | 迭代器 | iteratorT | 字符串、整数 | iteratorT | advance | 拷贝迭代器重定位 | ★★★☆☆ |
| 6 | `it + n` | 迭代器 | iteratorT | size_t | iteratorT | advance | 拷贝迭代器多步前进 | ★★★☆☆ |
| 7 | `os << json` | Json结点 | 输出流 | jsonT | std::ostream& | toString | 序列化输出 | ★★★★☆ |
| 7 | `doc << input` | Json文档 | docT | 字符串、文件 | bool | read | 读入json解析 | ★★★★☆ |
| 7 | `json << b` | 可写 Json | MutableValue | scalarT 等类型 | MutableValue | push | 数组或对象尾部追加结点 | ★★★★☆ |
| 7 | `it << b` | 可写迭代器 | iteratorT | scalarT 等类型 | iteratorT& | insert | 数组或对象迭代器定点插入 | ★★★★☆ |
| 7 | `doc >> output` | Json文档 | docT | 字符串、文件 | bool | write | 序列化输出 | ★★★★☆ |
| 7 | `json >> b` | Json结点 | jsonT | scalartT& | bool | get | 值提取并判断是否成功 | ★★★★☆ |
| 7 | `json >> json` | 可写 Json | MutableValue | MutableValue& | MutableValue& | pop | 数组尾部删除 | ★★★☆☆ |
| 7 | `json >> kv` | 可写 Json | MutableValue | KeyValue& | MutableValue& | pop | 对象尾部删除 | ★★★☆☆ |
| 7 | `it >> json` | 可写迭代器 | iteratorT | MutableValue& | iteratorT& | remove | 数组迭代器定点删除 | ★★★☆☆ |
| 7 | `it >> kv` | 可写迭代器 | iteratorT | KeyValue& | iteratorT& | remove | 对象迭代器定点删除 | ★★★☆☆ |
| 9 | `a < b` | Json结点 | jsonT | jsonT | bool | less | json 有序性定义 | ★★★☆☆ |
| 9 | `a <= b` | Json结点 | jsonT | jsonT | bool | less | json 有序性定义 | ★★★☆☆ |
| 9 | `a > b` | Json结点 | jsonT | jsonT | bool | less | json 有序性定义 | ★★★☆☆ |
| 9 | `a >= b` | Json结点 | jsonT | jsonT | bool | less | json 有序性定义 | ★★★☆☆ |
| 10 | `a == b` | 所有核心类 | T | T | bool | equal | 相等性判断 | ★★★★☆ |
| 10 | `a != b` | 所有核心类 | T | T | bool | !equal | 不等性判断 | ★★★★☆ |
| 11 | `json & b` | Json结点 | jsonT | scalarT | bool | isType | 类型判断 | ★★★★☆ |
| 13 | `json \| b` | Json结点 | jsonT | scalarT | auto | getor | 带默认值提取 | ★★★★★ |
| 13 | `json \| func` | Json结点 | jsonT | 函数 | auto | pipe | 自定义管道函数转换 | ★★★★☆ |
| 16 | `json = json` | Json结点 | jsonT | jsonT | jsonT& | - | 自赋值拷贝 | ★★★★★ |
| 16 | `json = b` | 可写 Json | MutableValue | scalarT | MutableValue& | set | 基本类型赋值转 json | ★★★★★ |
| 16 | `a \|= json` | Json结点 | scalarT | jsonT | scalarT& | get | 复合提取赋值给原变量 | ★★★★☆ |
| 16 | `it += n` | 迭代器类 | iteratorT | size_t | iteratorT& | advance | 迭代器多步前进 | ★★★☆☆ |
| 16 | `it %= key` | 迭代器类 | iteratorT | 字符串、整数 | iteratorT& | advance | 迭代器重定位 | ★★★☆☆ |
<!-- end operator table -->

> **重要说明**：
> 1. **`.` 操作符** - 不可重载，用于调用成员方法，优先级很高（2），推荐度中等
>    是因为大部分方法都可用其他操作符重载实现
> 2. **`_xyjson` 字面量操作符** - 用户定义字面量，优先级最高（无表达式优先级问题）
> 3. 操作符重载不能改变 C++ 固有的优先级和结合律，使用时需注意操作符的正确组合
> 4. 操作符优先级为 3 与 16 的结合性从右往左，其他是正常的从左往右

下面将以操作符优先级的顺序介绍每一种操作符重载的用法。因为推荐度、常用、好用等
概念是偏主观的，优先级才是客观的。

### 字面量操作符 `_xyjson`

**语法**：`"json_string"_xyjson`

**功能**：从字符串字面量直接构造 Document

- **参数类型**：字符串字面量 `(const char*, size_t)`
- **返回值类型**：`yyjson::Document`

注意：该字面量操作在 `yyjson::` 命名空间定义，需显式引入。其他常规操作符重载虽
也在 `yyjson::` 命名空间定义，但可由 ADL(Argument-Dependent Lookup) 规则找到，
不定显式引入命名空间。

```cpp
using namespace yyjson;
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;

// 等效：显式调用 yyjson::operator""_xyjson
{
    const char* str = R"({"name": "Alice", "age": 30})";
    size_t len = ::strlen(str);
    auto doc = yyjson::operator""_xyjson(str, len),
}

// 等效：使用构造函数
{
    yyjson::Document doc(R"({"name": "Alice", "age": 30})");
}
```

### 成员访问操作符 `->`

**语法**：`iteratorT->method()` 或 `jsonT.method()`

**功能**：调用成员方法，`.` 不可重载，但 `->` 可重载

- **左侧参数类型**：iteratorT 或 jsonT
- **右侧参数**：方法名
- **返回值类型**：根据方法定义

备注：重载 `->` 主要是为了满足迭代器的标准操作，`iteratorT::operator->` 返回
`jsonT`, 而 `jsonT::operator->` 返回 `this` 。客户代码用 `->` 调用成员方法是返
回值取决于所调用方法。

```cpp
yyjson::Document doc(R"({"name": "Alice", "age": 30})");

auto json = doc / "age";
auto iter = doc % "age";
if (json->isInt()) {
    int age = json->toInteger(); // 结果：30
}
if (iter->isInt()) {
    int age = iter->toInteger(); // 结果：30
}

// 等效：使用 . 成员访问操作符
// jsonT 类直接使用 . 访问，iteratorT 类要先解引用
{
    auto json = doc / "age";
    auto iter = doc % "age";
    if (json.isInt()) {
        int age = json.toInteger(); // 结果：30
    }
    if ((*iter).isInt()) {
        int age = (*iter).toInteger(); // 结果：30
    }
}
```

<!-- refine line -->

下面将以操作符优先级的顺序重新介绍每一种操作符重载的用法。因为推荐度、常用、好用等
概念是偏主观的，优先级才是客观的。

### `[]` 索引操作符

**语法**：`jsonT[key]` 或 `jsonT[index]`

**功能**：单层索引访问，MutableValue 可自动创建新字段

- **左侧参数类型**：`jsonT` 或 `docT`
- **右侧参数类型**：`const char*`（对象键）或 `int`（数组索引）
- **返回值类型**：`jsonT`（与左侧参数相同类型）

#### 对象键访问

```cpp
/* use operator */
yyjson::Document doc = R"({"user": {"name": "Alice"}})"_xyjson;
auto name = doc["user"]["name"];

/* use method */
yyjson::Document doc2 = R"({"user": {"name": "Alice"}})"_xyjson;
auto name2 = doc2.root().index("user").index("name");
```

#### 数组索引访问

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
auto first = doc["items"][0];

/* use method */
yyjson::Document doc2 = R"({"items": [10, 20, 30]})"_xyjson;
auto first2 = doc2.root().index("items").index(0);
```

#### 可写模型的自动插入

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

### 迭代器后缀自增 `it++`

**语法**：`iteratorT++`

**功能**：后缀自增，返回原拷贝

- **参数类型**：`iteratorT`
- **返回值类型**：`iteratorT`

```cpp
// 注意：该操作符已定义但较少使用，推荐使用前缀自增 ++it
auto iter = doc / "items" % 0;
auto oldIter = iter++;  // 返回原迭代器，然后iter前进
```

> **说明**：该操作符涉及拷贝迭代器，实际使用较少，推荐使用前缀自增 `++it`。

### 迭代器后缀自减 `it--`

**语法**：`iteratorT--`

**功能**：后缀自减，返回原拷贝

- **参数类型**：`iteratorT`
- **返回值类型**：`iteratorT`

```cpp
// 注意：该操作符已定义但较少使用，推荐使用前缀自减 --it
auto iter = doc / "items" % 5;
auto oldIter = iter--;  // 返回原迭代器，然后iter后退
```

> **说明**：该操作符涉及拷贝迭代器，实际使用较少，推荐使用前缀自增 `--it`。

### 布尔转换 `bool(a)`

**语法**：`bool(jsonT)` 或 `bool(docT)` 或 `bool(iteratorT)`

**功能**：显式转布尔逻辑值，判断对象有效性

- **参数类型**：任意核心类
- **返回值类型**：`bool`

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
if (bool(doc)) {
    // 文档有效
}
bool isValid = bool(doc / "name");

/* use method */
if (doc.hasError()) {
    // 文档有错误
}
bool isValid2 = doc / "name".isValid();
```

## 优先级 3 - 一元操作符与类型转换

### 逻辑非 `!`

**语法**：`!jsonT` 或 `!docT` 或 `!iteratorT`

**功能**：错误判断/无效判断

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

### 迭代器前缀自增 `++it`

**语法**：`++iteratorT`

**功能**：前缀自增再返回

- **参数类型**：`iteratorT&`
- **返回值类型**：`iteratorT&`

```cpp
/* use operator */
auto iter = doc / "items" % 0;
++iter;  // 前缀自增

/* use method */
auto iter2 = doc / "items" % 0;
iter2.next();
```

### 迭代器前缀自减 `--it`

**语法**：`--iteratorT`

**功能**：前缀自减再返回

- **参数类型**：`iteratorT&`
- **返回值类型**：`iteratorT&`

```cpp
// 注意：该操作符当前在 xyjson.h 中已定义
auto iter = doc / "items" % 5;
--iter;  // 前缀自减

// 等效方法
iter.prev();
```

> **说明**：迭代器的后退功能不是 O(1) 而是 O(N) 操作。

### 按位非 `~` - 模式转换

**语法**：`~docT`

**功能**：只读与可写文档互转

- **参数类型**：`Document` 或 `MutableDocument`
- **返回值类型**：`MutableDocument` 或 `Document`

#### 只读转可写

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
yyjson::MutableDocument mutCopy = ~doc;

/* use method */
yyjson::MutableDocument mutCopy2 = doc.mutate();
```

#### 可写转只读

```cpp
/* use operator */
yyjson::MutableDocument mutDoc = R"({"name": "Alice"})"_xyjson;
yyjson::Document readOnlyCopy = ~mutDoc;

/* use method */
yyjson::Document readOnlyCopy2 = mutDoc.freeze();
```

### 按位非 `~` - 迭代器键值对

**语法**：`~iteratorT`

**功能**：获取迭代器当前键结点

- **参数类型**：`iteratorT`
- **返回值类型**：`jsonT`

```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
for (auto iter = doc / "user" % ""; iter; ++iter) {
    auto keyNode = ~iter;  // 获取键结点
    auto valueNode = *iter;  // 获取值结点
    auto keyName = -iter;  // 获取键名
}
```

> **说明**：该操作符较少使用，通常直接访问键名和值结点即可满足需求。

### 解引用 `*` - 迭代器

**语法**：`*iteratorT`

**功能**：迭代器当前值结点

- **参数类型**：`iteratorT`
- **返回值类型**：`jsonT`

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    auto value = *iter;  // 获取当前元素值
}

/* use method */
auto iter2 = doc / "items" % 0;
auto value2 = iter2.value();
```

### 解引用 `*` - 文档根结点

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

### 强制转换 `(double)json`

**语法**：`(double)jsonT`

**功能**：转浮点数

- **参数类型**：`jsonT`
- **返回值类型**：`double`

```cpp
/* use operator */
yyjson::Document doc = R"({"price": 19.99})"_xyjson;
double price = (double)(doc / "price");

/* use method */
double price2 = doc / "price".toNumber();
```

> **说明**：该操作符当前在 xyjson.h 中已定义，但使用较少，推荐使用 `toNumber()` 方法。

### 强制转换 `(int)json`

**语法**：`(int)jsonT`

**功能**：转整数

- **参数类型**：`jsonT`
- **返回值类型**：`int`

```cpp
// 注意：该操作符当前在 xyjson.h 中已定义，但使用较少
int age = (int)(doc / "age");

// 推荐使用
int age2 = doc / "age".toInteger();
```

> **说明**：该操作符当前在 xyjson.h 中已定义，但使用较少，推荐使用 `toInteger()` 方法。

### 一元正号 `+` - 整数转换

**语法**：`+jsonT` 或 `+docT`

**功能**：转整数（用于 Value），或取根结点转整数（用于 Document）

- **参数类型**：`jsonT` 或 `docT`
- **返回值类型**：`int`

#### Value 类

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
int size = +(doc / "items");  // 获取数组大小

/* use method */
int size2 = doc / "items".toInteger();
```

#### Document 类

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
int rootSize = +doc;  // 相当于 +(doc.root())

/* use method */
int rootSize2 = doc.root().toInteger();
```

### 一元正号 `+` - 迭代器索引

**语法**：`+iteratorT`

**功能**：取迭代器当前索引

- **参数类型**：`iteratorT`
- **返回值类型**：`size_t`

```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    size_t index = +iter;  // 获取当前索引
    auto value = *iter;
}
```

> **说明**：该操作符较少使用，通常可以通过迭代器方法获取索引。

### 强制转换 `(string)json`

**语法**：`(std::string)jsonT`

**功能**：转字符串

- **参数类型**：`jsonT`
- **返回值类型**：`std::string`

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::string name = (std::string)(doc / "name");

/* use method */
std::string name2 = doc / "name".toString();
```

> **说明**：该操作符当前在 xyjson.h 中已定义，使用较少，推荐使用 `toString()` 方法。

### 一元负号 `-` - 字符串转换

**语法**：`-jsonT` 或 `-docT`

**功能**：转字符串（用于 Value），或取根结点转字符串（用于 Document）

- **参数类型**：`jsonT` 或 `docT`
- **返回值类型**：`std::string`

#### Value 类

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::string str = -(doc / "name");

/* use method */
std::string str2 = doc / "name".toString();
```

#### Document 类

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::string rootStr = -doc;  // 相当于 -(doc.root())

/* use method */
std::string rootStr2 = doc.root().toString();
```

### 一元负号 `-` - 迭代器键名

**语法**：`-iteratorT`

**功能**：取迭代器当前键名

- **参数类型**：`iteratorT`
- **返回值类型**：`const char*`

```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
for (auto iter = doc / "user" % ""; iter; ++iter) {
    const char* keyName = -iter;  // 获取当前键名
    auto value = *iter;
}
```

### `/` 路径操作符

**语法**：`jsonT / path`

**功能**：支持链式路径访问和 JSON Pointer 语法

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`const char*`（路径字符串）或 `int`（数组索引）
- **返回值类型**：`jsonT`（与左侧参数相同类型）

#### 基本路径访问

```cpp
/* use operator */
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
auto node = doc / "user" / "name";

/* use method */
yyjson::Document doc2 = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
auto node2 = doc2.root().pathto("user").pathto("name");
```

#### 数组索引访问

```cpp
/* use operator */
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
auto item = doc / "items" / 0;

/* use method */
yyjson::Document doc2 = R"({"items": [10, 20, 30]})"_xyjson;
auto item2 = doc2.root().pathto("items").index(0);
```

#### JSON Pointer 路径

```cpp
/* use operator */
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
auto node = doc / "/user/name";

/* use method */
yyjson::Document doc2 = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
auto node2 = doc2.root().pathto("/user/name");
```

#### `/` 迭代器搜索

**语法**：`iteratorT / key`

**功能**：连续快速查找固定键（对象迭代器）

- **左侧参数类型**：`iteratorT`
- **右侧参数类型**：`const char*`
- **返回值类型**：`jsonT`

```cpp
yyjson::Document doc = R"({"config": {"theme": "dark", "lang": "zh"}})"_xyjson;
auto iter = doc / "config" % "";
auto theme = iter / "theme";  // 快速查找
auto lang = iter / "lang";    // 继续在相同对象中查找
```

### `*` 乘法操作符

**语法**：`docT * value` 或 `jsonT * key`

**功能**：创建 JSON 结点

- **左侧参数类型**：`MutableDocument` 或 `MutableValue`（作为 key）
- **右侧参数类型**：`scalarT` 或 `MutableValue`
- **返回值类型**：`MutableValue` 或 `KeyValue`

#### 文档创建结点

```cpp
/* use operator */
yyjson::MutableDocument mutDoc;
auto newNode = mutDoc * "new_value";
auto newArray = mutDoc * "[]";

/* use method */
auto newNode2 = mutDoc.create("new_value");
auto newArray2 = mutDoc.create("[]");
```

#### 可写结点绑定键值对

```cpp
yyjson::MutableDocument mutDoc;
auto keyValue = mutDoc * "key" * "value";
// 或者链式创建
mutDoc * "config" * "[]";
```

### `%` 取模操作符

**语法**：`jsonT % startPos`

**功能**：创建迭代器

- **左侧参数类型**：`jsonT` 或 `docT`
- **右侧参数类型**：`int`（数组起始索引）或 `const char*`（对象起始键）
- **返回值类型**：`iteratorT`

#### 数组迭代器

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

#### 对象迭代器

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

### `%` 迭代器重定位

**语法**：`iteratorT %= target`

**功能**：拷贝迭代器重定位

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

### `+` 迭代器前进

**语法**：`iteratorT + n`

**功能**：拷贝迭代器多步前进

- **左侧参数类型**：`iteratorT`
- **右侧参数类型**：`size_t`
- **返回值类型**：`iteratorT`

```cpp
yyjson::Document doc = R"({"items": [10, 20, 30, 40, 50]})"_xyjson;
auto iter = doc / "items" % 0;
auto iter3 = iter + 3;  // 前进3步，指向第4个元素
```

> **说明**：该操作符返回新的迭代器，原迭代器不变。

### `<<` 左移操作符 - 序列化输出

**语法**：`std::ostream << jsonT` 或 `std::ostream << docT`

**功能**：序列化输出到流

- **左侧参数类型**：`std::ostream&`
- **右侧参数类型**：`jsonT` 或 `docT`
- **返回值类型**：`std::ostream&`

#### Json结点输出

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::cout << doc / "name" << std::endl;

/* use method */
std::cout << (doc / "name").toString() << std::endl;
```

#### 文档输出

```cpp
/* use operator */
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::cout << doc << std::endl;

/* use method */
yyjson::Document doc2 = R"({"name": "Alice"})"_xyjson;
std::cout << doc2.root().toString() << std::endl;
```

#### `<<` 文档读入

**语法**：`docT << input`

**功能**：读入json解析

- **左侧参数类型**：`docT`
- **右侧参数类型**：`const char*` 或 `std::istream&`
- **返回值类型**：`bool`（是否成功）

```cpp
yyjson::Document doc;
bool success = doc << R"({"name": "Alice"})";

yyjson::MutableDocument mutDoc;
std::ifstream file("data.json");
if (file) {
    bool success = mutDoc << file;
}
```

#### `<<` 追加插入

**语法**：`jsonT << value` 或 `iteratorT << value`

**功能**：数组或对象尾部追加结点，迭代器定点插入

- **左侧参数类型**：`MutableValue` 或 `iteratorT`
- **右侧参数类型**：各种类型
- **返回值类型**：`MutableValue&` 或 `iteratorT&`

#### 数组追加

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

#### 对象插入键值对

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

#### 迭代器定点插入

```cpp
yyjson::MutableDocument mutDoc;
mutDoc["items"] = "[]";
auto iter = mutDoc / "items" % 0;
iter << 10 << 20 << 30;  // 在迭代器位置插入元素
```

### `>>` 右移操作符

#### 序列化输出

**语法**：`docT >> output`

**功能**：序列化输出

- **左侧参数类型**：`docT`
- **右侧参数类型**：`std::ostream&` 或 `const char*`
- **返回值类型**：`bool`（是否成功）

```cpp
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::ofstream file("output.json");
doc >> file;

std::string str;
doc >> str;  // 输出到字符串
```

#### 安全值提取

**语法**：`jsonT >> target`

**功能**：值提取并判断是否成功

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
if (doc / "age".get(age2)) {
    // 成功提取
}
if (doc / "name".get(name2)) {
    // 成功提取
}
```

#### 数组尾部删除

**语法**：`jsonT >> jsonT` 或 `jsonT >> KeyValue&`

**功能**：数组或对象尾部删除

- **左侧参数类型**：`MutableValue`
- **右侧参数类型**：`MutableValue&` 或 `KeyValue&`
- **返回值类型**：`MutableValue&`

```cpp
yyjson::MutableDocument mutDoc;
mutDoc["items"] = "[]";
mutDoc / "items" << 1 << 2 << 3;
auto popped = mutDoc / "items" >> *(mutDoc / "items");  // 删除尾部元素
```

> **说明**：该操作符使用较少，通常使用 `pop()` 方法更直观。

#### 迭代器定点删除

**语法**：`iteratorT >> jsonT` 或 `iteratorT >> KeyValue&`

**功能**：数组或对象迭代器定点删除

- **左侧参数类型**：`iteratorT`
- **右侧参数类型**：`MutableValue&` 或 `KeyValue&`
- **返回值类型**：`iteratorT&`

```cpp
yyjson::MutableDocument mutDoc;
mutDoc["items"] = "[]";
mutDoc / "items" << 1 << 2 << 3;
auto iter = mutDoc / "items" % 1;
iter >> *(mutDoc / "items" / 1);  // 删除索引1的元素
```

> **说明**：该操作符使用较少，通常使用 `remove()` 方法更直观。

### 小于 `<`

**语法**：`jsonT < jsonT`

**功能**：json 有序性定义

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`jsonT`
- **返回值类型**：`bool`

```cpp
yyjson::Document doc = R"({"a": 10, "b": 20})"_xyjson;
if (doc / "a" < doc / "b") {
    // 比较结果
}
```

> **说明**：json的有序性定义基于内容，具体比较规则参考`less()`方法。

### 小于等于 `<=`

**语法**：`jsonT <= jsonT`

**功能**：json 有序性定义

```cpp
if (doc / "a" <= doc / "b") {
    // 比较结果
}
```

### 大于 `>`

**语法**：`jsonT > jsonT`

**功能**：json 有序性定义

```cpp
if (doc / "b" > doc / "a") {
    // 比较结果
}
```

### 大于等于 `>=`

**语法**：`jsonT >= jsonT`

**功能**：json 有序性定义

```cpp
if (doc / "b" >= doc / "a") {
    // 比较结果
}
```

### 相等 `==`

**语法**：`lhs == rhs`

**功能**：相等性判断

- **左侧参数类型**：任意类型
- **右侧参数类型**：任意类型
- **返回值类型**：`bool`

#### 文档比较

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

#### 值比较

```cpp
/* use operator */
if (doc / "version" == "1.0") {
    // 版本匹配
}

/* use method */
if (doc / "version".equal("1.0")) {
    // 版本匹配
}
```

### 不等 `!=`

**语法**：`lhs != rhs`

**功能**：不等性判断

```cpp
/* use operator */
if (doc / "status" != "active") {
    // 状态不匹配
}

/* use method */
if (!doc / "status".equal("active")) {
    // 状态不匹配
}
```

### `&` 按位与操作符

**语法**：`jsonT & typeValue`

**功能**：类型判断

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`scalarT` 或 `EmptyXXX` 类型常量
- **返回值类型**：`bool`

#### 基本类型判断

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

#### 使用类型常量

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

### `|` 按位或操作符

**语法**：`jsonT | defaultValue` 或 `jsonT | function`

**功能**：从 JSON 结点提取值，支持默认值和管道函数

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`scalarT`（基本类型）、函数、类型常量
- **返回值类型**：与右侧参数类型相同

#### 基本类型提取

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

#### 管道函数转换

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

#### 使用类型常量

```cpp
/* use operator */
yyjson::Document doc = R"({"score": [10, 20, 30]})"_xyjson;
auto arr = doc / "score" | yyjson::kArray;
int size = +(doc / "score" | yyjson::kArray);

/* use method */
yyjson::Document doc2 = R"({"score": [10, 20, 30]})"_xyjson;
auto arr2 = doc2 / "score".getor(yyjson::kArray);
```

### `=` 赋值操作符

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

### `|=` 复合赋值提取

**语法**：`scalarT& |= jsonT`

**功能**：复合提取赋值给原变量

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

### `+=` 加法赋值

**语法**：`iteratorT += n`

**功能**：迭代器多步前进

- **左侧参数类型**：`iteratorT&`
- **右侧参数类型**：`size_t`
- **返回值类型**：`iteratorT&`

```cpp
/* use operator */
auto iter = doc / "items" % 0;
iter += 3;  // 跳过 3 个元素

/* use method */
auto iter2 = doc / "items" % 0;
iter2.advance(3);
```

### `%=` 取模赋值

**语法**：`iteratorT %= key`

**功能**：迭代器重定位

- **左侧参数类型**：`iteratorT&`
- **右侧参数类型**：`int` 或 `const char*`
- **返回值类型**：`iteratorT&`

```cpp
/* use operator */
auto iter = doc / "items" % 0;
iter %= 5;  // 重定位到索引 5

/* use method */
auto iter2 = doc / "items" % 0;
iter2.advance(5);
```

### Document 转调 Root 的同义操作符

以下操作符在 Document 上使用时，会自动转调 `root()` 方法：

| 操作符 | 实际调用 | 示例 |
|--------|----------|------|
| `doc[key]` | `root().index(key)` | `doc["user"]["name"]` |
| `+doc` | `+root()` | `+doc`（转整数） |
| `-doc` | `-root()` | `-doc`（转字符串） |
| `doc / path` | `root().pathto(path)` | `doc / "user/name"` |
| `doc % key` | `root().iterator(key)` | `doc % ""` |
| `os << doc` | `os << root().toString()` | `std::cout << doc` |
| `doc == doc` | `root() == other.root()` | `doc1 == doc2` |

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

### 类型判断

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

### 值提取

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

### 特殊容器类型

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

## 操作符命名常量

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
