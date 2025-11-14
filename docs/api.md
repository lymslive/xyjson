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
- **stringT** - 字符串，包括 C-Style 的 `const char*` 与 C++ 的 `std::string`

整数一般指最常用的 `int` ，但在用于索引时内部用标准的 `size_t` ，而 `int` 也会
自动提升转为 `size_t` 。yyjson 在存整数时使用的是 `uint64_t` 或 `int64_t` ，平
时使用小整数时一般向下转 `int` 。

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

**语法**：`it->method()` 或 `json->method()`

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

下面将以操作符优先级的顺序重新介绍每一种操作符重载的用法。因为推荐度、常用、好用等
概念是偏主观的，优先级才是客观的。

### 索引操作符 `[]`

**语法**：`jsonT[key]` 或 `jsonT[index]`

**功能**：单层索引访问，MutableValue 可自动创建新字段

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`stringT`（对象键名）或 `size_t`（数组索引）
- **返回值类型**：`jsonT`（与左侧参数相同类型）

索引操作符 `[]` 的优先级与成员访问 `.` 的优先级相同，可以在 `[]` 取子元素
后调用方法，这点比路径操作符 `/` 简洁点。所以如果惯用方法名，可用 `[]` ，否则
更推荐用 `/` 。

注意：索引操作是 O(N) 时间复杂度，不过是用键名索引对象，还是用整数索引数组，都
是线性的。

#### 对象字段访问 `index`

```cpp
yyjson::Document doc = R"({"user": {"name": "Alice"}})"_xyjson;
auto user = doc["user"];
auto name = user["name"];

/* use method: index() */
{
    auto name = user.index("name");
}
```

#### 数组索引访问 `index`

```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
auto items = doc["items"];
auto first = items[0];

/* use method: index() */
{
    auto first = items.index(0);
}
```

#### 可写对象自动添加字段 `index`

```cpp
yyjson::MutableDocument mutDoc;
mutDoc["new_field"] = "value";
mutDoc["array"] = "[]";

/* use method: index() */
{
    yyjson::MutableDocument mutDoc;
    mutDoc.root().index("new_field").set("value");
    mutDoc.root().index("array").set("[]");
}
```

由于 `[]` 会自动添加不存在字段，尽量避免在 `=` 右侧使用，意外增加不想增加的键。
使用 `[]` 有意添加新键时，涉及线性扫描，效率比 `<<` 低，但能避免添加重复键。

### 后缀自增 `i++`

#### 迭代器拷贝前进 `next`

**语法**：`itratorT++`

**功能**：后缀自增，返回原拷贝

- **参数类型**：`iteratorT`
- **返回值类型**：`iteratorT`

```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
auto iter = doc / "items" % 0;
auto oldIter = iter++;  // 返回原迭代器，然后iter前进

/* use method: next() */
{
    auto iter = doc / "items" % 0;
    auto oldIter = iter;
    iter.next();
}
```

> **说明**：该操作符涉及拷贝迭代器，推荐尽可能使用前缀自增 `++it`。

### 后缀自减 `i--`

#### 迭代器拷贝后退 `prev`

**语法**：`itratorT--`

**功能**：后缀自减，返回原拷贝

- **参数类型**：`iteratorT`
- **返回值类型**：`iteratorT`

```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
auto iter = doc / "items" % 1;
auto oldIter = iter--;  // 返回原迭代器，然后iter后退

/* use method: prev() */
{
    auto iter = doc / "items" % 0;
    auto oldIter = iter;
    iter.prev();
}
```

> **说明**：该操作符涉及拷贝迭代器，推荐尽可能使用前缀自增 `--it`。

### 前缀自增 `++i`

#### 迭代器前进 `next`

**语法**：`++iteratorT`

**功能**：前缀自增再返回

- **参数类型**：`iteratorT&`
- **返回值类型**：`iteratorT&`

```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;

auto iter = doc / "items" % 0;
++iter;  // 原迭代器前进并返回

/* use method: next() */
{
    iter.next();
}
```

### 前缀自减 `--i`

#### 迭代器后退 `prev`

**语法**：`--iteratorT`

**功能**：前缀自减再返回

- **参数类型**：`iteratorT&`
- **返回值类型**：`iteratorT&`

```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;

auto iter = doc / "items" % 0;
--iter;  // 原迭代器后退并返回

/* use method: prev() */
{
    iter.prev();
}
```

> **说明**：迭代器的后退功能不是 O(1) 而是 O(N) 操作。

### 逻辑非 `!`

#### 无效值判断 `isValid`

**语法**：`!jsonT` 或 `!docT` 或 `!itratorT`

**功能**：错误判断/无效判断

- **参数类型**：jsonT, docT, iteratorT
- **返回值类型**：`bool`

```cpp
yyjson::Document doc = R"({})"_xyjson;

if (!doc) {
    // 文档无效
}
if (!(doc / "nonexistent")) {
    // 路径不存在
}
if (!(doc % "nonexistent")) {
    // 迭代器无效
}

/* use method: isValid() */
if (doc.hasError()) { // 或 !doc.isValid()
    // 文档有错误
}
if (!doc["nonexistent"].isValid()) {
    // 结点无效
}
if (doc.root().iterator("nonexistent").isValid() == false) {
    // 迭代器无效
}
```

### 类型转换 `(type)a` 或 `type(a)`

注意：C 风格的类型转换 `(type) a` 优先级是 3 ，C++ 构造函数风格的类型转换
`type(a)` 优先级是 2 ，略有微小差异。

Value 类支持向 `bool`，`double`, `int` 与 `std::string` 的显式类型转换。
其中 `bool` 类型转换支持在布尔上下文如条件判断中直接使用，与 `!` 是相反的语义，
用于判断各类对象是否有效。而后三者转数字、转整数与转字符串，则根据 json 内容所
做的较复杂的逻辑转换。

两种 Document 类支持在只读与可写类型之间互转（拷贝式），但两种 Value 类不支持
互转。虽然 yyjson 底层指针 `yyjson_mut_val*` 在表示标量叶结点时的内部实现有时
可强转为 `yyjson_val*` ，因为前 16 字节意义相同，但在表示容器时两者有显著区别，
故不支持 `Value` 与 `MutableValue` 的互相转换。

#### 布尔转换 `bool`

**语法**：`bool(jsonT)` 或 `bool(docT)` 或 `bool(iteratorT)`；条件语句中可直接
使用，或使用双重否定 `!!`。

**功能**：显式转布尔逻辑值，判断对象有效性，另见操作符 `!`

- **参数类型**：jsonT, docT, iteratorT
- **返回值类型**：`bool`

```cpp
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;

/* use operator */
if (doc) {
    // 文档有效，解析无错误
}
bool isValid = bool(doc / "name");

/* use method */
if (doc.hasError()) {
    // 文档解析出错
}
bool isValid2 = doc["name"].isValid();
```

注意：json 转 `bool` 只判断所引用的结点是否有效（相当于指针判空），但是不管
json 结点的内容是否为 `null`, `false`, `0` 或空字符串、空数组、空对象等。如果
想判断这类空内容可先转整数再与 `0` 比较。

```cpp
yyjson::Document doc = R"({"null":null, "bool":false, "number":0, "string":"", "array":[], "object":{}})"_xyjson;

bool hasNode = false;
// 以下 hasNode 都返回 true
hasNode = (bool)doc["null"];
hasNode = (bool)doc["bool"];
hasNode = (bool)doc["number"];
hasNode = (bool)doc["string"];
hasNode = (bool)doc["array"];
hasNode = (bool)doc["object"];

bool hasValue = true;
// 以下 hasValue 都返回 false
hasValue = (int)doc["null"];
hasValue = (int)doc["bool"];
hasValue = (int)doc["number"];
hasValue = (int)doc["string"];
hasValue = (int)doc["array"];
hasValue = (int)doc["object"];
```

#### 整数转换 `int`

**语法**：`(int)jsonT` 或 `int(jsonT)`

**功能**：显式转整数，另见一元操作符 `+`

- **参数类型**：jsonT
- **返回值类型**：`int`

各类型 json 结点转整数逻辑：
- null: 0
- bool: false 0, true 1
- number: 截断为 int
- string: 使用 atoi 将字符串转整数，可能只转前缀部分
- array/object: 取容器大小，即子元素个数
- 无效结点也返回 0

```cpp
yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
"string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;

int iError  = (int)doc["error"];  // 0
int iNull   = (int)doc["null"];   // 0
int iBool   = (int)doc["bool"];   // 1
int iNumber = (int)doc["number"]; // 3
int iString = (int)doc["string"]; // 3
int iArray  = (int)doc["array"];  // 2
int iObject = (int)doc["object"]; // 3

/* use method: toInteger() */
{
    int iError  = doc["error"].toInteger();  // 0
    int iNull   = doc["null"].toInteger();   // 0
    int iBool   = doc["bool"].toInteger();   // 1
    int iNumber = doc["number"].toInteger(); // 3
    int iString = doc["string"].toInteger(); // 3
    int iArray  = doc["array"].toInteger();  // 2
    int iObject = doc["object"].toInteger(); // 3
}
```

#### 实数转换 `double`

**语法**：`(double)jsonT` 或 double(jsonT)

**功能**：转浮点实数，大整数可能丢失精度，非数字类型结点返回 `0.0` 。

- **参数类型**：`jsonT`
- **返回值类型**：`double`

注意：转实数的 `toNUmber()` 方法只针对 json 的 Number 类型，不像转整数的
`toInteger()` 方法试图解释所有 json 类型在整数上下文的意义。

```cpp
yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14, "int":3,
"string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;

double fError  = (double)doc["error"];  // 0.0
double fNull   = (double)doc["null"];   // 0.0
double fBool   = (double)doc["bool"];   // 0.0
double fNumber = (double)doc["number"]; // 3.14
double fInt    = (double)doc["int"];    // 3.0
double fString = (double)doc["string"]; // 0.0
double fArray  = (double)doc["array"];  // 0.0
double fObject = (double)doc["object"]; // 0.0

/* use method: toNumber() */
{
    double fError  = doc["error"].toNumber();  // 0.0
    double fNull   = doc["null"].toNumber();   // 0.0
    double fBool   = doc["bool"].toNumber();   // 0.0
    double fNumber = doc["number"].toNumber(); // 3.14
    double fInt    = doc["int"].toNumber();    // 3.0
    double fString = doc["string"].toNumber(); // 0.0
    double fArray  = doc["array"].toNumber();  // 0.0
    double fObject = doc["object"].toNumber(); // 0.0
}
```

#### 字符串转换 `std::string`

**语法**：`(std::string)jsonT` 或 `std::string(jsonT)`

**功能**：转字符串，另见操作符 `-`

- **参数类型**：`jsonT`
- **返回值类型**：`std::string`

```cpp
yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
"string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;

std::string strError  = (std::string)doc["error"];  // ""
std::string strNull   = (std::string)doc["null"];   // null
std::string strBool   = (std::string)doc["bool"];   // true
std::string strNumber = (std::string)doc["number"]; // 3.14
std::string strString = (std::string)doc["string"]; // 3.14pi
std::string strArray  = (std::string)doc["array"];  // [null,null]
std::string strObject = (std::string)doc["object"]; // {"x":0,"y":0,"z":0}
std::string strRoot   = (std::string)doc.root();

/* use method: toString() */
{
    std::string strError  = doc["error"].toString();  // ""
    std::string strNull   = doc["null"].toString();   // null
    std::string strBool   = doc["bool"].toString();   // true
    std::string strNumber = doc["number"].toString(); // 3.14
    std::string strString = doc["string"].toString(); // 3.14pi
    std::string strArray  = doc["array"].toString();  // [null,null]
    std::string strObject = doc["object"].toString(); // {"x":0,"y":0,"z":0}
    std::string strRoot   = doc.root().toString();
}
```

转字符串其实就相当于 json 序列化，以字符串方式表达任意 json 类型的结点。除了字
符串结点，序列化时会加引号，而转字符串只返回字符串内容值。但 `toString()` 方法
能接收可选参数 `true` ，表示真序列化，字符串类型会加引号，且容器类型会加缩进，
适合打印输出给人工阅读。例如：

```cpp
yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
"string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;

std::string strString = doc["string"].toString(true); // "3.14pi"
std::string strArray  = doc["array"].toString(true);
std::string strObject = doc["object"].toString(true);
```

最后一条语句输出大约是：
```json
{
    "x": 0,
    "y": 0,
    "z": 0
}
```

#### 只读文档转可写 `MutableDocument`

**语法**：`(MutableDocument)doc` 或 `MutableDocument(doc)`

**功能**：只读 json 文档树转可写文档，另见操作符 `~`

- **参数类型**：`Document`
- **返回值类型**：`MutableDocument`

```cpp
yyjson::Document doc("{}");
yyjson::MutableDocument mutDoc(doc);

/* use method: mutate() */
{
    yyjson::MutableDocument mutDoc = doc.mutate();
}
```

#### 可写文档转只读 `Document`

**语法**：`(Document)doc` 或 `Document(doc)`

**功能**：可写 json 文档树转只读文档，另见操作符 `~`

- **参数类型**：`MutableDocument`
- **返回值类型**：`Document`

```cpp
yyjson::MutableDocument mutDoc("{}");
yyjson::Document doc(mutDoc);

/* use method: freeze() */
{
    yyjson::Document doc = mutDoc.freeze();
}
```

### 按位非 `~`

重载 `~` 操作符用于文档类在只读与可写类型之间互转；
也用于迭代器取当前键结点，为与 `*` 取键结点相对应。

#### 只读文档转可写 `mutabte`

**语法**：`~doc`

**功能**：只读 json 文档树转可写文档

- **参数类型**：`Document`
- **返回值类型**：`MutableDocument`

```cpp
yyjson::Document doc("{}");
yyjson::MutableDocument mutDoc(doc);

/* use method: mutate() */
{
    yyjson::MutableDocument mutDoc = doc.mutate();
}
```

#### 可写文档转只读 `freeze`

**语法**：`~doc`

**功能**：可写 json 文档树转只读文档

- **参数类型**：`MutableDocument`
- **返回值类型**：`Document`

```cpp
yyjson::MutableDocument mutDoc("{}");
yyjson::Document doc(mutDoc);

/* use method: freeze() */
{
    yyjson::Document doc = mutDoc.freeze();
}
```

#### 迭代器取键结点 `key`

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

/* use method: key() */
for (auto iter = doc / "user" % ""; iter; ++iter) {
    auto keyNode = iter.key();  // 获取键结点
    auto valueNode = iter.value();  // 获取值结点
    auto keyName = iter.name();  // 获取键名
}
```

> **说明**：该操作符较少使用，通常直接访问键名和值结点即可满足需求。

### 一元解引用 `*`

重载 `*` 操作符用于迭代器取当前值结点；也用于文档类取根结点，有很多操作符作用
于文档时其实是转为作用于其根结点。

#### 迭代器取值结点 `value`

**语法**：`*iteratorT`

**功能**：迭代器当前值结点

- **参数类型**：`iteratorT`
- **返回值类型**：`jsonT`

```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    auto value = *iter;  // 获取当前元素值
}

/* use method: value() */
for (auto iter = doc / "items" % 0; iter; ++iter) {
    auto value = iter.value();  // 获取当前元素值
}
```

#### 文档取根结点 `root`

**语法**：`*docT`

**功能**：从 Document 获取根结点

- **参数类型**：`docT`
- **返回值类型**：`jsonT`（Document 对应的 Value 类型）

```cpp
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
auto root = *doc;

yyjson::MutableDocument mutDoc(doc);
auto mutRoot = *mutDoc;

/* use method: root() */
{
    auto root = doc.root();
    auto mutRoot = mutDoc.root();
}
```

### 一元正号 `+`

重载一元 `+` 操作符用于 json 结点类时转整数，用于迭代器类时取当前索引。尽量在
整数上下文中表达合适的意义。

#### Json 类型转整数 `toInteger`

**语法**：`+jsonT`

**功能**：为各种 json 类型解释在整数上下文的意义

- **参数类型**：`jsonT`
- **返回值类型**：`int`

各类型 json 结点转整数逻辑：
- null: 0
- bool: false 0, true 1
- number: 截断为 int
- string: 使用 atoi 将字符串转整数，可能只转前缀部分
- array/object: 取容器大小，即子元素个数
- 无效结点也返回 0

```cpp
yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
"string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;

int iError  = +doc["error"];  // 0
int iNull   = +doc["null"];   // 0
int iBool   = +doc["bool"];   // 1
int iNumber = +doc["number"]; // 3
int iString = +doc["string"]; // 3
int iArray  = +doc["array"];  // 2
int iObject = +doc["object"]; // 3

/* use method: toInteger() */
{
    int iError  = doc["error"].toInteger();  // 0
    int iNull   = doc["null"].toInteger();   // 0
    int iBool   = doc["bool"].toInteger();   // 1
    int iNumber = doc["number"].toInteger(); // 3
    int iString = doc["string"].toInteger(); // 3
    int iArray  = doc["array"].toInteger();  // 2
    int iObject = doc["object"].toInteger(); // 3
}
```

#### 迭代器取当前索引 `index`

**语法**：`+iteratorT`

**功能**：取迭代器当前索引

- **参数类型**：`iteratorT`
- **返回值类型**：`size_t`

```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    size_t idx = +iter;
    auto val = *iter;
}

/* use method: index() */
for (auto iter = doc / "items" % 0; iter; ++iter) {
    size_t idx = iter.index();
    auto val = iter.value();
}
```

### 一元负号 `-`

重载一元 `-` 操作符用于 json 结点类时转字符串，用于迭代器类时取当前键名。

#### Json 类型转字符串 `toString`

**语法**：`-jsonT`

**功能**：Json 结点转字符串

- **参数类型**：`jsonT` 或 `docT`
- **返回值类型**：`std::string`

```cpp
yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
"string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;

std::string strError  = -doc["error"];  // ""
std::string strNull   = -doc["null"];   // null
std::string strBool   = -doc["bool"];   // true
std::string strNumber = -doc["number"]; // 3.14
std::string strString = -doc["string"]; // 3.14pi
std::string strArray  = -doc["array"];  // [null,null]
std::string strObject = -doc["object"]; // {"x":0,"y":0,"z":0}
std::string strRoot   = -doc.root();

/* use method: toString() */
{
    std::string strError  = doc["error"].toString();  // ""
    std::string strNull   = doc["null"].toString();   // null
    std::string strBool   = doc["bool"].toString();   // true
    std::string strNumber = doc["number"].toString(); // 3.14
    std::string strString = doc["string"].toString(); // 3.14pi
    std::string strArray  = doc["array"].toString();  // [null,null]
    std::string strObject = doc["object"].toString(); // {"x":0,"y":0,"z":0}
    std::string strRoot   = doc.root().toString();
}
```

另注：`-` 相当于 json 单行序列化，但对于字符串结点不加引号；方法 `toString()`
能接收可选参数 `true`，表示真序列化，字符串类型会加引号，且容器类型会加缩进。

### 迭代器取当前键名 `name`

**语法**：`-iteratorT`

**功能**：取迭代器当前键名

- **参数类型**：`iteratorT`
- **返回值类型**：`const char*`

```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
for (auto iter = doc / "user" % ""; iter; ++iter) {
    const char* keyName = -iter;
    auto valNode = *iter;
}

/* use method: name() */
for (auto iter = doc / "user" % ""; iter; ++iter) {
    const char* keyName = iter.name();
    auto valNode = iter.value();
}
```

<!-- refine line -->
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

#### 索引操作符 `[]`

**语法**：`docT[key]` 或 `docT[index]`

**功能**：单层索引根结点，作用于可写文档时会自动创建新字段

- **左侧参数类型**：`docT`
- **右侧参数类型**：`stringT`（对象键名）或 `size_t`（数组索引）
- **返回值类型**：`jsonT`（与 docT 对应的 Value 类型）

```cpp
yyjson::Document array("[10, 20, 30]");
yyjson::MutableDocument object("{}");

auto first = array[0];
object["first"] = 10;

/* use method: root() */
{
    auto first = array.root()[0];
    object.root()["second"] = 20;
}
```

#### 一元正号 `+` 转整数

**语法**：`+docT`

**功能**：或取根结点转整数

- **参数类型**：`docT`
- **返回值类型**：`int`

```cpp
yyjson::Document doc = R"([10, 20, 30])"_xyjson;
int size = +doc;  // 3

/* use method: root() */
{
    int size = +doc.root(); // 3
}
```

#### 一元负号 `-` 转字符串

**语法**：`-docT`

**功能**：取根结点转字符串

- **参数类型**：`docT`
- **返回值类型**：`std::string`


```cpp
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::string str = -doc; // {"name":"Alice"}

/* use method: root() */
{
    std::string str = -doc.root();
}
```

#### 除法 `/` 重载路径查找
#### 取模 `%` 重载创建迭代器
#### 左移 `<<` 重载序列化流输出
#### 相等操作符 `==` 

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
