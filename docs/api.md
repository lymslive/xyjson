# xyjson API 参考手册

本手册提供 xyjson 库的完整 API 参考，作为查询手册之用。

## 1 核心类介绍

在 xyjson 中定义的类，基本都是对 yyjson 底层 C 结构体指针的封装，
所有类均在 `yyjson::` 命名空间（而不是 `xyjson::`）下定义：

- **Document** - 只读 JSON 文档树包装器
- **MutableDocument** - 可写 JSON 文档树包装器
- **Value** - 只读 JSON 结点值包装器
- **MutableValue** - 可写 JSON 结点值包装器
- **ArrayIterator** - 只读数组迭代器
- **MutableArrayIterator** - 可写数组迭代器
- **ObjectIterator** - 只读对象迭代器
- **MutableObjectIterator** - 可写对象迭代器

详细的类层次关系请参考：[类图](class_diagram.puml)

### 1.1 泛型类型约定

本文档使用以下泛型名称：

- **jsonT** - Value 与 MutableValue 类型统称
- **docT** - Document 与 MutableDocument 类型统称
- **iteratorT** - 四种迭代器类型统称
- **scalarT** - 数字与字符串等基本标量类型统称
- **stringT** - 字符串，包括 C-Style 的 `const char*` 与 C++ 的 `std::string`
- **keyT** - 同 stringT ，json 键只能用字符串

整数一般指最常用的 `int` ，但在用于索引时内部用标准的 `size_t` ，而 `int` 也会
自动提升转为 `size_t` 。yyjson 在存整数时使用的是 `uint64_t` 或 `int64_t` ，平
时使用小整数时一般向下转 `int` 。

## 2 操作符重载

### 2.1 操作符总览表

<!-- begin operator table -->
| 优先级 | 操作符 | 适用类 | 左参类型 | 右参类型 | 返回类型 | 对应方法 | 功能描述 | 推荐度 |
|------|------|------|------|------|------|------|------|------|
| 0 | "{}"_xyjson | 只读文档 | 字符串字面量 | 无 | Document | 构造函数 | 字符串字面量自定义转换 | ★★★☆☆ |
| 2 | a.b | 所有类 | 对象 | 成员 | auto |  | 成员访问，不可重载 | ★★★☆☆ |
| 2 | it->b | 迭代器 | iteratorT | jsonT 成员方法 | auto | value | 间接成员访问 | ★★★☆☆ |
| 2 | json->b | Json结点 | jsonT | jsonT 成员方法 | auto | this | 间接成员访问 | ★★☆☆☆ |
| 2 | json[key] | Json结点 | jsonT | 字符串、整数 | jsonT | index | 常规单层索引 | ★★★★☆ |
| 2 | it++ | 迭代器 | iteratorT | 无 | iteratorT | next | 后缀自增，返回原拷贝 | ★★☆☆☆ |
| 2 | it-- | 迭代器 | iteratorT | 无 | iteratorT | prev | 后缀自减，返回原拷贝 | ★☆☆☆☆ |
| 2 | bool(a) | 所有核心类 | 无 | T | bool | isValid | 布尔条件上下文直接判断 | ★★★★☆ |
| 3 | (bool)a |  | 无 | T | bool | isValid | 显式转布尔逻辑值 | ★★★☆☆ |
| 3 | ++it | 迭代器 | iteratorT | 无 | iteratorT& | next | 前缀自增再返回 | ★★★★☆ |
| 3 | --it | 迭代器 | iteratorT | 无 | iteratorT& | prev | 前缀自减再返回 | ★★★☆☆ |
| 3 | !a | 所有核心类 | 无 | T | bool | !isValid | 错误判断/无效判断 | ★★★★★ |
| 3 | (int)json | Json结点 | 无 | jsonT | int | toInteger | 转整数 | ★★★☆☆ |
| 3 | (double)json | Json结点 | 无 | jsonT | double | toNumber | 转浮点数 | ★★★☆☆ |
| 3 | (string)json | Json结点 | 无 | jsonT | std::string | toString | 转字符串 | ★★★☆☆ |
| 3 | ~doc | 只读文档 | 无 | Document | MutableDocument | mutate | 可读写模式转换 | ★★★★☆ |
| 3 | ~doc | 可写文档 | 无 | MutableDocument | Document | freeze | 可读写模式转换 | ★★★★☆ |
| 3 | ~it | 迭代器 | 无 | iteratorT | jsonT | key | 迭代器当前键结点 | ★★☆☆☆ |
| 3 | *it | 迭代器 | 无 | iteratorT | jsonT | value | 迭代器当前值结点 | ★★★★★ |
| 3 | *doc | Json文档 | 无 | docT | jsonT | root | 取根结点 | ★★★★☆ |
| 3 | +json | Json结点 | 无 | jsonT | int | toInteger | 转整数 | ★★★☆☆ |
| 3 | +it | 迭代器 | 无 | iteratorT | size_t | index | 取迭代器当前索引 | ★★☆☆☆ |
| 3 | -json | Json结点 | 无 | jsonT | std::string | toString | 转字符串 | ★★★☆☆ |
| 3 | -it | 迭代器 | 无 | iteratorT | const char* | name | 取迭代器当前键名 | ★★☆☆☆ |
| 5 | json / path | Json结点 | jsonT | 字符串、整数 | jsonT | pathto | 路径访问，支持 JSON Pointer | ★★★★★ |
| 5 | it / key | 对象迭代器 | iteratorT | 字符串 | jsonT | seek | 连续快速查找固定键 | ★★★★☆ |
| 5 | doc * b | 可写文档 | MutableDocument | scalarT | MutableValue | create | 创建 JSON 结点 | ★★★☆☆ |
| 5 | doc * b | 可写文档 | MutableDocument | jsonT docT | MutableValue | create | 复制 JSON 结点 | ★★★☆☆ |
| 5 | doc * b | 可写文档 | MutableDocument | MutableValue&& | MutableValue | create | 移动 JSON 结点 | ★★☆☆☆ |
| 5 | json * json | 可写 Json | MutableValue&& | MutableValue&& | KeyValue | tag | 绑定键值对 | ★★★☆☆ |
| 5 | json * key | 可写 Json | MutableValue | keyT | KeyValue | tag | 值绑定键名 | ★★★☆☆ |
| 5 | json % key | Json结点 | jsonT | 字符串、整数 | iteratorT | iterator | 创建迭代器 | ★★★★☆ |
| 5 | it % key | 迭代器 | iteratorT | 字符串、整数 | iteratorT | advance | 拷贝迭代器重定位 | ★★☆☆☆ |
| 6 | it + n | 迭代器 | iteratorT | size_t | iteratorT | advance | 拷贝迭代器多步前进 | ★★☆☆☆ |
| 7 | os << json | Json结点 | 输出流 | jsonT | std::ostream& | toString | 序列化输出 | ★★★★☆ |
| 7 | doc << input | Json文档 | docT | 字符串、文件 | bool | read | 读入json解析 | ★★★★☆ |
| 7 | json << b | 可写 Json | MutableValue | scalarT 等类型 | MutableValue | push | 数组或对象尾部追加结点 | ★★★★★ |
| 7 | it << b | 可写迭代器 | iteratorT | scalarT 等类型 | iteratorT& | insert | 数组或对象迭代器定点插入 | ★★★★☆ |
| 7 | doc >> output | Json文档 | docT | 字符串、文件 | bool | write | 序列化输出 | ★★★★☆ |
| 7 | json >> b | Json结点 | jsonT | scalartT& | bool | get | 值提取并判断是否成功 | ★★★★☆ |
| 7 | json >> json | 可写 Json | MutableValue | MutableValue& | MutableValue& | pop | 数组尾部删除 | ★★★☆☆ |
| 7 | json >> kv | 可写 Json | MutableValue | KeyValue& | MutableValue& | pop | 对象尾部删除 | ★★★☆☆ |
| 7 | it >> json | 可写迭代器 | iteratorT | MutableValue& | iteratorT& | remove | 数组迭代器定点删除 | ★★★☆☆ |
| 7 | it >> kv | 可写迭代器 | iteratorT | KeyValue& | iteratorT& | remove | 对象迭代器定点删除 | ★★★☆☆ |
| 9 | a < b | Json结点 | jsonT | jsonT | bool | less | json 有序性定义 | ★★★☆☆ |
| 9 | a <= b | Json结点 | jsonT | jsonT | bool | less | json 有序性定义 | ★★☆☆☆ |
| 9 | a > b | Json结点 | jsonT | jsonT | bool | less | json 有序性定义 | ★★☆☆☆ |
| 9 | a >= b | Json结点 | jsonT | jsonT | bool | less | json 有序性定义 | ★★☆☆☆ |
| 10 | a == b | 所有核心类 | T | T | bool | equal | 相等性判断 | ★★★★☆ |
| 10 | a != b | 所有核心类 | T | T | bool | !equal | 不等性判断 | ★★★★☆ |
| 11 | json & b | Json结点 | jsonT | scalarT | bool | isType | 类型判断 | ★★★★☆ |
| 13 | json \| b | Json结点 | jsonT | scalarT | auto | getor | 带默认值提取 | ★★★★★ |
| 13 | json \| func | Json结点 | jsonT | 函数 | auto | pipe | 自定义管道函数转换 | ★★★★☆ |
| 16 | json = json | Json结点 | jsonT | jsonT | jsonT& |  | 自赋值拷贝 | ★★★★★ |
| 16 | json = b | 可写 Json | MutableValue | scalarT | MutableValue& | set | 基本类型赋值转 json | ★★★★★ |
| 16 | a \|= json | Json结点 | scalarT | jsonT | scalarT& | get | 复合提取赋值给原变量 | ★★★★☆ |
| 16 | it += n | 迭代器类 | iteratorT | size_t | iteratorT& | advance | 迭代器多步前进 | ★★★☆☆ |
| 16 | it %= key | 迭代器类 | iteratorT | 字符串、整数 | iteratorT& | advance | 迭代器重定位 | ★★★☆☆ |
|  |  |  |  |  |  |  | Document 转调 root 的操作符 |  |
| 2 | doc[key] | Json文档 | docT | 字符串、整数 |  | index | 取根结点单层索引 | ★★★★☆ |
| 3 | +doc | Json文档 | 无 | jsonT |  | toInteger | 取根结点转整数 | ★★★☆☆ |
| 3 | -doc | Json文档 | 无 | jsonT |  | toString | 取根结点转字符串 | ★★★☆☆ |
| 5 | doc / path | Json文档 | docT | 字符串、整数 | jsonT | pathto | 根结点路径访问 | ★★★★★ |
| 5 | doc % key | Json文档 | docT | 字符串、整数 | iteratorT | iterator | 根结点创建迭代器 | ★★★★☆ |
| 7 | os << doc | Json文档 | 输出流 | docT | os& | toString | 根结点序列化流输出 | ★★★★☆ |
| 10 | doc == doc | Json文档 | docT | docT | bool | equal | 根结点相等性判断 | ★★★★☆ |
<!-- end operator table -->

> **重要说明**：
> 1. **`.` 操作符** - 不可重载，用于调用成员方法，优先级很高（2），推荐度中等
>    是因为大部分方法都可用其他操作符重载实现
> 2. **`_xyjson` 字面量操作符** - 用户定义字面量，优先级最高（无表达式优先级问题）
> 3. 操作符重载不能改变 C++ 固有的优先级和结合律，使用时需注意操作符的正确组合
> 4. 操作符优先级为 3 与 16 的结合性从右往左，其他是正常的从左往右

下面将以操作符优先级的顺序介绍每一种操作符重载的用法。因为推荐度、常用、好用等
概念是偏主观的，优先级才是客观的。

### 2.2 字面量操作符 `_xyjson`

**语法**：`"json_string"_xyjson`

**功能**：从字符串字面量直接构造 Document

- **参数类型**：字符串字面量 `(const char*, size_t)`
- **返回值类型**：`yyjson::Document`

注意：该字面量操作在 `yyjson::` 命名空间定义，需显式引入。其他常规操作符重载虽
也在 `yyjson::` 命名空间定义，但可由 ADL(Argument-Dependent Lookup) 规则找到，
不定显式引入命名空间。

<!-- example:api_2_2_literal_operator -->
```cpp
using namespace yyjson;
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;

// 等效：显式调用 yyjson::operator""_xyjson
{
    const char* str = R"({"name": "Alice", "age": 30})";
    size_t len = ::strlen(str);
    auto doc = yyjson::operator""_xyjson(str, len);
}

// 等效：使用构造函数
{
    yyjson::Document doc(R"({"name": "Alice", "age": 30})");
}
```

### 2.3 成员访问操作符 `->`

**语法**：`it->method()` 或 `json->method()`

**功能**：调用成员方法，`.` 不可重载，但 `->` 可重载

- **左侧参数类型**：iteratorT 或 jsonT
- **右侧参数**：方法名
- **返回值类型**：根据方法定义

备注：重载 `->` 主要是为了满足迭代器的标准操作，`iteratorT::operator->` 返回
`jsonT`, 而 `jsonT::operator->` 返回 `this` 。客户代码用 `->` 调用成员方法是返
回值取决于所调用方法。

<!-- example:api_2_3_member_access -->
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

### 2.4 索引操作符 `[]`

**语法**：`jsonT[key]` 或 `jsonT[index]`

**功能**：单层索引访问，MutableValue 可自动创建新字段

- **左侧参数类型**：`jsonT`，要求对象或数组类型
- **右侧参数类型**：`stringT`（对象键名）或 `size_t`（数组索引）
- **返回值类型**：`jsonT`（与左侧参数相同类型）

索引操作符 `[]` 的优先级与成员访问 `.` 的优先级相同，可以在 `[]` 取子元素
后调用方法，这点比路径操作符 `/` 简洁点。所以如果惯用方法名，可用 `[]` ，否则
更推荐用 `/` 。

注意：索引操作是 O(N) 时间复杂度，不论是用键名索引对象，还是用整数索引数组，都
是线性的。

#### 2.4.1 对象字段访问 `index`

<!-- example:api_2_4_1_object_field_access -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice"}})"_xyjson;
auto user = doc["user"];
auto name = user["name"];

/* use method: index() */
{
    auto name = user.index("name");
}
```

#### 2.4.2 数组索引访问 `index`

<!-- example:api_2_4_2_array_index_access -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
auto items = doc["items"];
auto first = items[0];

/* use method: index() */
{
    auto first = items.index(0);
}
```

#### 2.4.3 可写对象自动添加字段 `index`

<!-- example:api_2_4_3_mutable_object_add_field -->
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

### 2.5 后缀自增 `i++`

#### 2.5.1 迭代器拷贝前进 `next`

**语法**：`itratorT++`

**功能**：后缀自增，返回原拷贝

- **参数类型**：`iteratorT`
- **返回值类型**：`iteratorT`

<!-- example:api_2_5_1_iterator_copy_advance -->
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

### 2.6 后缀自减 `i--`

#### 2.6.1 迭代器拷贝后退 `prev`

**语法**：`itratorT--`

**功能**：后缀自减，返回原拷贝

- **参数类型**：`iteratorT`
- **返回值类型**：`iteratorT`

<!-- example:api_2_6_1_iterator_copy_retreat -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
auto iter = doc / "items" % 1;
auto oldIter = iter--;  // 返回原迭代器，然后iter后退

/* use method: prev() */
{
    auto iter = doc / "items" % 1;
    auto oldIter = iter;
    iter.prev();
}
```

> **说明**：该操作符涉及拷贝迭代器，推荐尽可能使用前缀自增 `--it`。

### 2.7 前缀自增 `++i`

#### 2.7.1 迭代器前进 `next`

**语法**：`++iteratorT`

**功能**：前缀自增再返回

- **参数类型**：`iteratorT&`
- **返回值类型**：`iteratorT&`

<!-- example:api_2_7_1_iterator_advance -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;

auto iter = doc / "items" % 0;
++iter;  // 原迭代器前进并返回

/* use method: next() */
{
    auto iter = doc / "items" % 0;
    iter.next();
}
```

### 2.8 前缀自减 `--i`

#### 2.8.1 迭代器后退 `prev`

**语法**：`--iteratorT`

**功能**：前缀自减再返回

- **参数类型**：`iteratorT&`
- **返回值类型**：`iteratorT&`

<!-- example:api_2_8_1_iterator_retreat -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;

auto iter = doc / "items" % 1;
--iter;  // 原迭代器后退并返回

/* use method: prev() */
{
    auto iter = doc / "items" % 1;
    iter.prev();
}
```

> **说明**：迭代器的后退功能不是 O(1) 而是 O(N) 操作。

### 2.9 逻辑非 `!`

#### 2.9.1 无效值判断 `isValid`

**语法**：`!jsonT` 或 `!docT` 或 `!itratorT`

**功能**：错误判断/无效判断

- **参数类型**：jsonT, docT, iteratorT
- **返回值类型**：`bool`

<!-- example:api_2_9_1_invalid_value_check -->
```cpp
yyjson::Document doc = R"({})"_xyjson;


if (!doc) {
    // 文档无效，可能是解析错误
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

### 2.10 类型转换 `(type)a` 或 `type(a)`

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

#### 2.10.1 布尔转换 `bool`

**语法**：`bool(jsonT)` 或 `bool(docT)` 或 `bool(iteratorT)`

**功能**：显式转布尔逻辑值，判断对象有效性。条件语句中可直接使用，或使用双重否
定 `!!`。另见操作符 `!`

- **参数类型**：jsonT, docT, iteratorT
- **返回值类型**：`bool`

<!-- example:api_2_10_1_bool_conversion -->
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

<!-- example:api_2_10_1_bool_conversion_extended -->
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

#### 2.10.2 整数转换 `int`

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

<!-- example:api_2_10_2_int_conversion -->
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

#### 2.10.3 实数转换 `double`

**语法**：`(double)jsonT` 或 double(jsonT)

**功能**：转浮点实数，大整数可能丢失精度，非数字类型结点返回 `0.0` 。

- **参数类型**：`jsonT`
- **返回值类型**：`double`

注意：转实数的 `toNUmber()` 方法只针对 json 的 Number 类型，不像转整数的
`toInteger()` 方法试图解释所有 json 类型在整数上下文的意义。

<!-- example:api_2_10_3_double_conversion -->
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

#### 2.10.4 字符串转换 `std::string`

**语法**：`(std::string)jsonT` 或 `std::string(jsonT)`

**功能**：转字符串，另见操作符 `-`

- **参数类型**：`jsonT`
- **返回值类型**：`std::string`

<!-- example:api_2_10_4_string_conversion -->
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

<!-- example:api_2_10_4_string_conversion_pretty -->
```cpp
yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
"string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;

std::string strString = doc["string"].toString(true); // "3.14pi"
std::string strArray  = doc["array"].toString(true);
std::string strObject = doc["object"].toString(true);
```

最后一条语句输出格式大约是：
```json
{
    "x": 0,
    "y": 0,
    "z": 0
}
```

#### 2.10.5 只读文档转可写 `MutableDocument`

**语法**：`(MutableDocument)doc` 或 `MutableDocument(doc)`

**功能**：只读 json 文档树转可写文档，另见操作符 `~`

- **参数类型**：`Document`
- **返回值类型**：`MutableDocument`

<!-- example:api_2_10_5_doc_to_mutabledoc -->
```cpp
yyjson::Document doc("{}");
yyjson::MutableDocument mutDoc(doc);

/* use method: mutate() */
{
    yyjson::MutableDocument mutDoc = doc.mutate();
}
```

#### 2.10.6 可写文档转只读 `Document`

**语法**：`(Document)doc` 或 `Document(doc)`

**功能**：可写 json 文档树转只读文档，另见操作符 `~`

- **参数类型**：`MutableDocument`
- **返回值类型**：`Document`

<!-- example:api_2_10_6_mutabledoc_to_doc -->
```cpp
yyjson::MutableDocument mutDoc("{}");
yyjson::Document doc(mutDoc);

/* use method: freeze() */
{
    yyjson::Document doc = mutDoc.freeze();
}
```

### 2.11 按位非 `~`

重载 `~` 操作符用于文档类在只读与可写类型之间互转；
也用于迭代器取当前键结点，为与 `*` 取键结点相对应。

#### 2.11.1 只读文档转可写 `mutabte`

**语法**：`~doc`

**功能**：只读 json 文档树转可写文档

- **参数类型**：`Document`
- **返回值类型**：`MutableDocument`

<!-- example:api_2_11_1_doc_to_mutabledoc_bitwise -->
```cpp
yyjson::Document doc("{}");
yyjson::MutableDocument mutDoc = ~doc;

/* use method: mutate() */
{
    yyjson::MutableDocument mutDoc = doc.mutate();
}
```

#### 2.11.2 可写文档转只读 `freeze`

**语法**：`~doc`

**功能**：可写 json 文档树转只读文档

- **参数类型**：`MutableDocument`
- **返回值类型**：`Document`

<!-- example:api_2_11_2_mutabledoc_to_doc_bitwise -->
```cpp
yyjson::MutableDocument mutDoc("{}");
yyjson::Document doc = ~mutDoc;

/* use method: freeze() */
{
    yyjson::Document doc = mutDoc.freeze();
}
```

#### 2.11.3 迭代器取键结点 `key`

**语法**：`~iteratorT`

**功能**：获取迭代器当前键结点

- **参数类型**：`iteratorT`
- **返回值类型**：`jsonT`

<!-- example:api_2_11_3_iterator_get_key -->
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

### 2.12 一元解引用 `*`

重载 `*` 操作符用于迭代器取当前值结点；也用于文档类取根结点，有很多操作符作用
于文档时其实是转为作用于其根结点。

#### 2.12.1 迭代器取值结点 `value`

**语法**：`*iteratorT`

**功能**：迭代器当前值结点

- **参数类型**：`iteratorT`
- **返回值类型**：`jsonT`

<!-- example:api_2_12_1_iterator_get_value -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
int sum = 0;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    auto value = *iter;  // 获取当前元素值
    sum += value | 0;
}

/* use method: value() */
sum = 0;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    auto value = iter.value();  // 获取当前元素值
    sum += value | 0;
}
```

#### 2.12.2 文档取根结点 `root`

**语法**：`*docT`

**功能**：从 Document 获取根结点

- **参数类型**：`docT`
- **返回值类型**：`jsonT`（Document 对应的 Value 类型）

<!-- example:api_2_12_2_doc_get_root -->
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

### 2.13 一元正号 `+`

重载一元 `+` 操作符用于 json 结点类时转整数，用于迭代器类时取当前索引。尽量在
整数上下文中表达合适的意义。

#### 2.13.1 Json 类型转整数 `toInteger`

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

<!-- example:api_2_13_1_json_to_int -->
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

#### 2.13.2 迭代器取当前索引 `index`

**语法**：`+iteratorT`

**功能**：取迭代器当前索引

- **参数类型**：`iteratorT`
- **返回值类型**：`size_t`

<!-- example:api_2_13_2_iterator_get_index -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
size_t lastIndex = 0;
int sum = 0;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    size_t idx = +iter;
    auto val = *iter;
    lastIndex = idx;
    sum += val | 0;
}

/* use method: index() */
lastIndex = 0;
sum = 0;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    size_t idx = iter.index();
    auto val = iter.value();
    lastIndex = idx;
    sum += val | 0;
}
```

### 2.14 一元负号 `-`

重载一元 `-` 操作符用于 json 结点类时转字符串，用于迭代器类时取当前键名。

#### 2.14.1 Json 类型转字符串 `toString`

**语法**：`-jsonT`

**功能**：Json 结点转字符串

- **参数类型**：`jsonT` 或 `docT`
- **返回值类型**：`std::string`

<!-- example:api_2_14_1_json_to_string -->
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

### 2.15 迭代器取当前键名 `name`

**语法**：`-iteratorT`

**功能**：取迭代器当前键名

- **参数类型**：`iteratorT`
- **返回值类型**：`const char*`

<!-- example:api_2_15_1_iterator_get_name -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
int count = 0;
for (auto iter = doc / "user" % ""; iter; ++iter) {
    const char* keyName = -iter;
    auto valNode = *iter;
    count++;
}

/* use method: name() */
count = 0;
for (auto iter = doc / "user" % ""; iter; ++iter) {
    const char* keyName = iter.name();
    auto valNode = iter.value();
    count++;
}
```

### 2.16 除法运算 `/`

重载操作符 `/` 是 xyjson 库的核心特色，在大部分情况下可平替 `[]` 索引操作，且
衍生支持更高级的扩展功能。

#### 2.16.1 链式路径访问 `pathto`

**语法**：`jsonT / key` 或 `jsonT / index`

**功能**：类似 `[]` 索引的单层访问与线性查找，更方便链式逐级访问

- **左侧参数类型**：`jsonT`，要求对象或数组类型
- **右侧参数类型**：`stringT`（对象键名）或 `size_t`（数组索引）
- **返回值类型**：`jsonT`（与左侧参数相同类型）

参数类型不匹配，或键名不存在，或数组索引越界时，返回无效值。即使可写 json 对象
路径访问不存在键时返回无效值，不会自动添加新键。

<!-- example:api_2_16_1_path_access -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
    "items": [10, 20, 30]})"_xyjson;
auto name = *doc / "user" / "name";
auto item = *doc / "items" / 0;

/* use method: pathto() */
{
    auto name = doc.root().pathto("user").pathto("name");
    auto item = doc.root().pathto("items").index(0);
}
```

路径操作符 `/` 优先级比 `.` 的成员访问优先级低，如果想对返回值调用方法，需要加
括号提升优先级，或者可改用 `[]`。

<!-- example:api_2_16_1_path_access_with_method -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
int age = (*doc / "user" / "age").toInteger();

/* use operator[] */
{
    int age = doc.root()["user"]["age"].toInteger();
}

/* use method: pathto() */
{
    int age = doc.root().pathto("user").pathto("age").toInteger();
}
```

> **说明**：
操作符 `/` 也可直接用于 Document 类，但 `pathto` 只是 Value 类的方法。

#### 2.16.2 JSON Pointer 路径访问 `pathto`

**语法**：`jsonT / "/json/pointer/path"`

**功能**：支持标准 JSON Pointer 语法

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`stringT`（JSON Pointer 格式路径）
- **返回值类型**：`jsonT`

注意：右侧参数需要遵循 JSON Pointer 标准。尤其要以 `/` 字符开头，否则认为是简
单的单层键名查找。

<!-- example:api_2_16_2_json_pointer_access -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
auto node = *doc / "/user/name";

/* use method: pathto() */
{
    auto node = doc.root().pathto("/user/name");
}
```

#### 2.16.3 对象迭代器顺序查找 `seek`

**语法**：`iteratorT / key`

**功能**：对象迭代器连续快速查找固定键

- **左侧参数类型**：`iteratorT`（限对象迭代器）
- **右侧参数类型**：`stringT`
- **返回值类型**：`jsonT`

当连续查找的键名顺序与 json 实际所存的键序相同时，只涉及一遍线性查找，而不是每
个键都一次线性查找。所以在最好情况下，每次查找键是 `O(1)` 时间复杂度。当键名不
存在或乱序时，会退化为 `O(N)` 查找。

<!-- example:api_2_16_3_iterator_seek -->
```cpp
yyjson::Document doc = R"({"config": {"theme": "dark", "lang": "zh"}})"_xyjson;
auto iter = doc / "config" % "";
auto theme = iter / "theme";  // 第一次查找
auto lang = iter / "lang";    // 继续在相同对象中查找

/* use method: seek() */
{
    auto iter = doc / "config" % "";
    auto theme = iter.seek("theme");
    auto lang = iter.seek("lang");
}
```

### 2.17 乘法运算 `*`

常规的乘法 `*` 是 `/` 的逆运算。在 xyjson 已经用 `/` 表示按路径查找 json 结点，
显然对于从头构建的 json 来说，要能用 `/` 查找到的前提就是先在 json 所在内存池
中创建结点，所以重载 `*` 用来表达这个功能。

很多时候不需要主动使用 `*` 操作符，因为往 json 容器中添加结点的操作符会隐含自
动先创建结点。理解 `*` 操作揭示了内部实现方法。

#### 2.17.1 文档创建新结点 `create`

**语法**：`MutableDocument * value`

**功能**：创建 JSON 结点，支持基本类型和类型常量

- **左侧参数类型**：`MutableDocument`
- **右侧参数类型**：`scalarT` 或特殊参数（如 "[]", "{}"）
- **返回值类型**：`MutableValue`

当右侧参数是字符串字面量时，创建字符串类型的 json 结点是引用字面量，不发生字符
串拷贝。且支持特殊字面 `"[]"` 与 `"{}"` 分别表示创建空数组与空对象结点。

新创建建的 json 结点使用左参数文档的内存池，与该文档具有相同的生命周期，但不会
自动挂载到该文档的根结点或其他子树结点，属于独立结点，须由用户再决定如何处理新
结点，是否要添加到文档已有的某个容器结点中。

<!-- example:api_2_17_1_doc_create_node -->
```cpp
yyjson::MutableDocument mutDoc;
auto intNode = mutDoc * 314;
auto strNode = mutDoc * "value";
auto newArray = mutDoc * "[]";
auto newObject = mutDoc * "{}";

/* use method: create() */
{
    auto intNode = mutDoc.create(314);
    auto strNode = mutDoc.create("new_value");
    auto newArray = mutDoc.create("[]");
    auto newObject = mutDoc.create("{}");
}
```

#### 2.17.2 复制结点 `create`

**语法**：`MutableDocument * jsonT` 或 `MutableDocument * docT`

**功能**：复制已有的 json 结点，或文档的根结点

- **左侧参数类型**：`MutableDocument`
- **右侧参数类型**：`jsonT` 或 `docT`
- **返回值类型**：`MutableValue`

复制结点不会影响右侧参数的结点，如果是容器结点将深度复制。复制结点与新建结点一
样使用目标文档的内存池，但不会自动挂载到根结点或子树结点。

<!-- example:api_2_17_2_doc_copy_node -->
```cpp
yyjson::MutableDocument dstDoc;
yyjson::MutableDocument srcDocMut(R"({"name": "Alice", "age": 30})");
yyjson::Document srcDoc(R"({"name": "Alice", "age": 30})");

auto copyName = dstDoc * (srcDoc / "name");
auto copyAge = dstDoc * (srcDocMut / "age");
auto copyDoc = dstDoc * srcDoc;
auto copyDocMut = dstDoc * srcDocMut;

/* use method: create() */
{
    auto copyName = dstDoc.create(srcDoc / "name");
    auto copyAge = dstDoc.create(srcDocMut / "age");
    auto copyDoc = dstDoc.create(srcDoc);
    auto copyDocMut = dstDoc.create(srcDocMut);
}
```

#### 2.17.3 移动结点 `create`

**语法**：`MutableDocument * MutableValue`

**功能**：移动一个右值语义的 json 结点代理值

- **左侧参数类型**：`MutableDocument`
- **右侧参数类型**：`MutableValue&&`
- **返回值类型**：`MutableValue`

与复制结点类似也返回一个 `MutableValue` 新值，但当右侧参数是 `MutableValue&&`
时，移动后原值失效。原来被移动的 `MutableValue` 必须与源于 `MutableDocument`
使用相同的内存池，否则移动后会出现内存问题，生命周期不一致。

另要注意，用该方法的移动结点，并不会真实影响底层 json 树的结点关系，因为
`MutableValue` 类只相当于底层结点的代理指针。它只强调指针代理权的转交。所以并
不能只靠该操作来真正移动原来某个 json 容器内的结点，需要先将结点从原容器中删除
，或将新建的独立结点用移动的方式添加到其他容器的某个位置，避免将同一个底层结点
重复添加到不同的容器中，造成所有权混乱，或者避免将新建结点再次不必要的重复复制
。

移动结点不会在文档的内存池中创建新结点，仍用 `create` 方法命名主要是内部实现中
上层模板方法转调底层方法时统一命名比较方便，只用参数重载表达语义的差异。

<!-- example:api_2_17_3_doc_move_node -->
```cpp
yyjson::MutableDocument mutDoc(R"(["Alice", 30])");

auto node = mutDoc * 2.5;     // node 有效
auto level = mutDoc * std::move(node); // node 失效
mutDoc.root() << std::move(level);   // level 失效
//^ 结果：["Alice",30,2.5]

// 直接用链式表达式，临时值是纯右值，属移动语义
*mutDoc << mutDoc * true; // 结果：["Alice",30,2.5,true]

// 添加数组可以更简洁点，会自动调用 create
*mutDoc << 100 << 200; // 结果：["Alice",30,2.5,true,100,200]
```

#### 2.17.4 可写结点绑定键值对 `tag`

**语法**：`MutableValue * MutableValue`

**功能**：将分别表示键与值的两个结点绑定在一起，支持联用创建结点的链式连乘。

- **左侧参数类型**：`MutableValue&&`
- **右侧参数类型**：`MutableValue&&`
- **返回值类型**：`KeyValue`

键值对绑定后生成一个中间类型 `KeyValue` ，它的主要只用是将来用于插入 json 对象。
为了避免将 `MutableValue` 所引用的结点重复插入对象造成的错误，要求移动语义，绑
定后原来的两个值失效。如果使用链式连乘，绑定的两个值都是用 `*` 创建的临时值，
那它们正好是右值，但如果保存了中间变量成了左值，就要显式使用 `std::move` 才可
以绑定。

<!-- example:api_2_17_4_bind_key_value -->
```cpp
yyjson::MutableDocument mutDoc("{}");
auto intNode = mutDoc * 314;
auto keyNode = mutDoc * "key";
auto kv = std::move(keyNode) * std::move(intNode);
mutDoc.root() << std::move(kv); // 结果：{"key":314}

// 使用连乘临时值，一元 `*` 表示 root()
{
    yyjson::MutableDocument mutDoc("{}");
    *mutDoc << (mutDoc * "key") * (mutDoc * 314);
}
```

#### 2.17.5 可写结点绑定键名 `tag`

**语法**：`MutableValue * key` 或 `key * MutableValue`

**功能**：为可写结点绑定键名，支持联用创建结点的链式连乘。

- **左侧参数类型**：`MutableValue&&`
- **右侧参数类型**：`stringT` 可用于键的字符串类型
- **返回值类型**：`KeyValue`

将 `MutableValue` 与一个字符串绑定时，字符串会自动先创建键结点，再绑定为键值对
。这时的 `*` 支持交换律，字符串可写在左边，更符合键值对阅读顺序。但使用对应的
方法名时，字符串参数只能写在右边的括号内。

<!-- example:api_2_17_5_bind_key_name -->
```cpp
yyjson::MutableDocument mutDoc;
auto keyValue = mutDoc * "value" * "key";
auto kvConfig = "config" * (mutDoc * "{}");

/* use method: tag() */
{
    auto keyValue = mutDoc.create("value").tag("key");
    auto kvConfig = mutDoc.create("{}").tag("config");
}

// 将绑定的结果插入根对象，生成: {"key":"value","config":{}}
mutDoc.root() << std::move(keyValue) << std::move(kvConfig);

// 连乘直接插入，一元 `*` 表示 root()
{
    yyjson::MutableDocument mutDoc;
    *mutDoc << mutDoc * "value" * "key" << "config" * (mutDoc * "{}");
}
```

### 2.18 取模运算 `%`

操作符 `%` 与 `/` 相关，故 xyjson 重载 `%` 用于创建迭代器，用另一方式来查找子
结点，相当于是 `/` 路径查找的变体。

#### 2.18.1 数组迭代器创建 `iterator`

**语法**：`jsonT % size_t`

**功能**：创建数组迭代器，可以指定起始索引，最常用的 `0` 表示从头开始迭代。

- **左侧参数类型**：`jsonT`（数组）
- **右侧参数类型**：`size_t`（起始索引）
- **返回值类型**：`ArrayIterator` 或 `MutableArrayIterator`

<!-- example:api_2_18_1_array_iterator_create -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
int sum = 0;
for (auto iter = doc / "items" % 0; iter; ++iter) {
    sum += *iter | 0;
}

/* use method: iterator() */
sum = 0;
for (auto iter = doc["items"].iterator(0); iter; ++iter) {
    sum += *iter | 0;
}
```

#### 2.18.2 对象迭代器创建 `iterator`

**语法**：`jsonT % key`

**功能**：创建对象迭代器，可以指定起始索引，最常用的 `""` 表示从头开始迭代，遍
历所有键值对。

- **左侧参数类型**：`jsonT`（对象）
- **右侧参数类型**：`const char*`（C 字符串表示的键名）
- **返回值类型**：`ObjectIterator` 或 `MutableObjectIterator`

因为从特定键名开始迭代的需求场景比较罕见，所以右侧参数暂时只支持 `const char*`
类型的字符串，简单实现。

<!-- example:api_2_18_2_object_iterator_create -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
int count = 0;
for (auto iter = doc / "user" % ""; iter; ++iter) {
    count++;
}

/* use method: iterator() */
count = 0;
for (auto iter = doc["user"].iterator(""); iter; ++iter) {
    count++;
}
```

#### 2.18.3 迭代器重定位 `advance`

**语法**：`iteratorT % target`

**功能**：迭代器拷贝重定位，另见不拷贝重定位自身的操作符 `%=`

- **左侧参数类型**：`iteratorT&`
- **右侧参数类型**：`int` 或 `const char*`
- **返回值类型**：`iteratorT&`

迭代器用 `%` 重定位时会先重置到开始位置，保证用相同的参数重定位时与直接以该参
数创建迭代器时得到相同的迭代器状态。重载该操作符主要为符号语义一致性，实用场景
可能并不多，而且 `%` 会拷贝新值，效率上也不如 `%=`。

<!-- example:api_2_18_3_iterator_relocate -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
    "items": [10, 20, 30]})"_xyjson;
auto itArray = doc / "items" % 1; // 创建迭代器初始位置 1
auto itArrayCopy = itArray % 3;  // 重定位到索引3的元素

auto itObject = *doc % "items"; // 创建迭代器初始位置 "items"
auto itObjectCopy = itObject % "user"; // 重定位到 "user" 位置

/* use method: advance() */
{
    auto itArray = doc["items"].iterator(1);
    auto itArrayCopy = itArray;
    itArrayCopy.begin().advance(3);
    
    auto itObject = doc.root().iterator("items");
    auto itObjectCopy = itObject;
    itObjectCopy.begin().advance("user");
}
```

### 2.19 加法运算 `+`

目前只有迭代器支持加法操作 `+` ，这属性标准迭代器语义重载，不算特殊重载。

#### 2.19.1 迭代器多步前进 `advance`

**语法**：`iteratorT + n`

**功能**：拷贝迭代器多步前进，另见不拷贝的前进操作符 `+=`

- **左侧参数类型**：`iteratorT`
- **右侧参数类型**：`size_t`
- **返回值类型**：`iteratorT`

该操作符返回新的迭代器，原迭代器不变。另外由于 yyjson 的迭代器不支持随机定位，
`it + n` 是通过循环 `++` 操作实现的，因而是 O(N) 时间复杂度。

<!-- example:api_2_19_1_iterator_advance_multistep -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30, 40, 50]})"_xyjson;
auto iter = doc / "items" % 0;
auto iterCopy = iter + 3;  // 前进3步，指向第4个元素

/* use method: advance() */
{
    auto iterCopy = iter;
    iterCopy.advance(3);
}
```

### 2.20 左移运算 `<<`

这可能是 C++ 中应用最广泛的重载了，标准库带头重载为流式插入。取其象形，xyjson
也为各个类重载了 `<<` 操作符，用于流式输出，或向 json 容器以至整体文档的输入。

#### 2.20.1 Json 标准流输出 `toString`

**语法**：`std::ostream << jsonT`

**功能**：Json 结点序列化输出到流

- **左侧参数类型**：`std::ostream&`
- **右侧参数类型**：`jsonT`
- **返回值类型**：`std::ostream&` 原左参引用

<!-- example:api_2_20_1_stream_output -->
```cpp
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::cout << doc / "name" << std::endl;
std::ostringstream oss;
oss << doc / "name";

/* use method: toString() */
{
    std::cout << (doc / "name").toString() << std::endl;
    std::ostringstream oss;
    oss << (doc / "name").toString();
}
```

#### 2.20.2 读入 Json 字符串 `read`

**语法**：`docT << input`

**功能**：读入并解析 Json 字符串

- **左侧参数类型**：`docT`
- **右侧参数类型**：`stringT`
- **返回值类型**：`bool`（解析是否成功）

<!-- example:api_2_20_2_doc_read_string -->
```cpp
yyjson::Document doc;
bool success = doc << R"({"name": "Alice"})";

/* use method: read() */
{
    yyjson::Document doc;
    bool success = doc.read(R"({"name": "Alice"})");
}

// 也可直接用构造函数
{
    yyjson::Document doc(R"({"name": "Alice"})");
}
```

>**说明**：
如果 Document 原来已经管理一个底层文档树，读入操作将会先释放原有的文档树，替换
为新文档树。使用构造函数则不涉及释放再重建文档树。

#### 2.20.3 读入 Json 文件解析 `read`

**语法**：`docT << input`

**功能**：读取 Json 文件并解析，可能替换原有底层文档树

- **左侧参数类型**：`docT`
- **右侧参数类型**：`std::istream&` 或 `FILE*`
- **返回值类型**：`bool`（解析是否成功）

<!-- example:api_2_20_3_doc_read_file -->
```cpp
yyjson::Document doc;
std::ifstream file("/tmp/data.json");
bool success = doc << file;

/* use method: read() */
{
    yyjson::Document doc;
    std::ifstream file("/tmp/data.json");
    bool success = doc.read(file);
}

// 另有 readFile() 方法接收文件名参数
{
    yyjson::Document doc;
    bool success = doc.readFile("/tmp/data.json");
}
```

#### 2.20.4 数组追加元素 `push`

**语法**：`MutableValue << value`

**功能**：数组尾部追加元素

- **左侧参数类型**：`MutableValue`（数组）
- **右侧参数类型**：各种能用 `MutableDocument` 创建结点的参数类型
- **返回值类型**：`MutableValue&` 原左参引用

<!-- example:api_2_20_4_array_append -->
```cpp
yyjson::MutableDocument mutDoc;
mutDoc["items"] = "[]";
mutDoc / "items" << 1 << "two" << 3.14;
//^ 结果：{"items":[1,"two",3.14]}

/* use method: push() */
{
    yyjson::MutableDocument mutDoc;
    mutDoc["items"] = "[]";
    mutDoc["items"].push(1).push("two").push(3.14);
}

// push 是 json 容器通用方法，数组也可用 append()
{
    yyjson::MutableDocument mutDoc;
    mutDoc["items"] = "[]";
    mutDoc["items"].append(1).append("two").append(3.14);
}
```

#### 2.20.5 对象插入键值对 `push`

**语法**：`MutableValue << key << value` 或 `MutableValue << KeyValue&&`

**功能**：对象插入键值对（也是尾部追加式）

- **左侧参数类型**：`MutableValue`（对象）
- **右侧参数类型**：各种能用 `MutableDocument`创建结点的参数类型
- **返回值类型**：`MutableValue&` 原左参引用

<!-- example:api_2_20_5_object_insert -->
```cpp
yyjson::MutableDocument mutDoc;
auto root = *mutDoc;
root << "key1" << "value1" << "key2" << 42;

/* use method: push() */
{
    yyjson::MutableDocument mutDoc;
    auto root = *mutDoc;
    root.push("key1").push("value1").push("key2").push(42);
}

// 也可用 add() 方法接收两个参数
{
    yyjson::MutableDocument mutDoc;
    auto root = *mutDoc;
    root.add("key1", "value1").add("key2", 42);
}
```

对象要支持这样的链式交替插入键与值，需要额外维护一个内部状态，是有一定代价的。
如果定义了条件编译宏 `XYJSON_DISABLE_CHAINED_INPUT`，则该操作方法不可用，默认
是可用的。

另外，对象的 `<<` 操作与 `add` 方法也支持接收一个打包了键值对的 `KeyValue` 
参数。需要右值 `KeyValue` ，通过 `*` 连乘创建的 `KeyValue` 临时值是右值，可直
接与 `<<` 联用。在禁用了交换插入键与值的情况下可用该操作方式。或者从其他对象删
除的键值对也可保存为 `KeyValue` ，再用 `<<` 或 `add` 方法再插入到其他地方。

<!-- example:api_2_20_5_object_insert_keyvalue -->
```cpp
yyjson::MutableDocument mutDoc;
*mutDoc << mutDoc * "Alice" * "name" << "age" * (mutDoc * 25);
//^ 结果：{"name":"Alice","age":25}

// 删除结点
yyjson::KeyValue age, name;
*mutDoc >> age >> name;

// 换个顺序重新插入
*mutDoc << std::move(age) << std::move(name);
//^ 结果：{"age":25,"name":"Alice"}
```

#### 2.20.6 迭代器定点插入 `insert`

**语法**：`iteratorT << value`

**功能**：在可写数组或对象迭代器指定位置插入元素

- **左侧参数类型**：`iteratorT`
- **右侧参数类型**：各种类型，与支持 `MutableValue <<` 的参数类型一样
- **返回值类型**：`iteratorT&` 原左参引用

操作符 `<<` 作用于 `MutableValue` 时，是在数组或对象末尾添加元素的，作用于可写
迭代器时，可在当前位置之前插入元素。它们所支持的参数意义是一样。

但是迭代器的操作符 `<<` 与它所对应的 `insert` 方法有点微小差异。调用 `insert`
方法后迭代器状态指向新插入的元素，也即原来的当前元素之前。如果支持链式调用
`it.insert(x).insert(y)` 将会往前插入，结果是 `y -> x -> 原来的当前元素` 。而
链式操作符 `<<` 直觉上应该保留顺序，批量将一些元素插入当前位置（之前），结果是
`x -> y -> 原来的当前元素` 。因此，操作符 `<<` 在调用 `insert` 方法后，还会自
动前进一步。

<!-- example:api_2_20_6_array_insert -->
```cpp
yyjson::MutableDocument mutDoc;
mutDoc["items"] = "[]";
auto iter = mutDoc / "items" % 0;
iter << 10 << 20 << 30;

/* use method: insert() */
{
    yyjson::MutableDocument mutDoc;
    mutDoc["items"] = "[]";
    auto iter = mutDoc / "items" % 0;
    iter.insert(10); ++iter;
    iter.insert(20); ++iter;
    iter.insert(30); ++iter;
}
```

用对象迭代器插入元素，默认也支持链式交替插入键与值，当用
`XYJSON_DISABLE_CHAINED_INPUT` 条件编译宏禁用该功能时，可以插入一个 `KeyValue`
右值参数，另外 `insert` 方法也支持直接传入键与值两个参数。

<!-- example:api_2_20_6_object_insert -->
```cpp
yyjson::MutableDocument mutDoc;
mutDoc["user"] = "{}";
auto iter = mutDoc / "user" % "";
iter << "name" << "Alice" << "age" * (mutDoc * 25);

/* use method: insert() */
{
    yyjson::MutableDocument mutDoc;
    mutDoc["user"] = "{}";
    auto iter = mutDoc / "user" % "";
    iter.insert("name"); iter.insert("Alice"); ++iter;
    iter.insert("age", 25); ++iter;
}
```

注意，操作符 `<<` 与 `insert` 方法的另一个差别是返回值不同。操作符 `<<` 返回迭
代器引用本身以支持链式操作。而 `insert` 方法返回 `bool` 值表示插入操作成功与否
。在插入数组时，一般都会成功。在交替插入对象时，先插入键时返回 `false` ，再插
入配对的值时返回 `true` 。在接收两个键与值参数，或一个键值对 `KeyValue` 参数，
如果键不是字符串也会返回 `false` 表示插入失败。

### 2.21 右移运算 `>>`

xyjson 重载了 `>>` 操作，基本满足 `<<` 操作的逆运算。但是不支持标准输入流在
`>>` 左侧，只支持自定义类在左侧。

#### 2.21.1 文档序列化字符串 `write`

**语法**：`docT >> output`

**功能**：将文档序列化为字符串

- **左侧参数类型**：`docT`
- **右侧参数类型**：`std::string`
- **返回值类型**：`bool`（是否成功）

与 `<<` 或 `read` 方法不一样的是，`>>` 或 `write` 的目标只能是 `std::string`
这种拥有字符串所有权的字符串类，不能是 `const char*` 。

<!-- example:api_2_21_1_doc_write_string -->
```cpp
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::string str;
doc >> str;

/* use method: write() */
{
    std::string str;
    doc.write(str);
}
```

#### 2.21.2 文档序列化写入文件

**语法**：`docT >> output`

**功能**：将文档序列化直接写入文件

- **左侧参数类型**：`docT`
- **右侧参数类型**：`std::ostream&` 或 `FILE*`
- **返回值类型**：`bool`（是否成功）

<!-- example:api_2_21_2_doc_write_file -->
```cpp
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::ofstream file("/tmp/output.json");
bool success = doc >> file;

/* use method: write() */
{
    std::ofstream file("/tmp/output.json");
    bool success = doc.write(file);
}

// 另有 writeFile() 方法接收文件名参数
{
    bool success = doc.writeFile("/tmp/output.json");
}
```

写入字符串基本是成功的，除非内存不足。写入文件就有更多场景可能失败了，比如文件
未打开，没有写入权限等。

#### 2.21.3 安全值提取 `get`

**语法**：`jsonT >> target`

**功能**：从 Json 标量叶结点提取值并判断是否成功

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`scalarT&` 或底层指针类型
- **返回值类型**：`bool`

该操作主要用于 json 叶结点，例如存储着一个标量数字或字符串。如果该 json 的类型
与 `>>` 右侧参数匹配，就会将 json 结点的内容提取存到右侧参数中，并返回 `bool`
表示该操作是否成功。该操作适用于需要显式判断提取是否成功的场景，如果希望在失败
时能回滚一个默认值时，使用 `|` 更为常见。

<!-- example:api_2_21_3_value_extract -->
```cpp
yyjson::Document doc(R"({"name": "Alice", "age":25})");

std::string name;
if (doc / "name" >> name) {
    // 成功提取
}

int age;
if (doc / "age" >> age) {
    // 成功提取
}

/* use method: get() */
{
    std::string name;
    if (doc["name"].get(name)) {
    }

    int age;
    if (doc["age"].get(age)) {
    }
}
```

#### 2.21.4 数组尾部删除 `pop`

**语法**：`MutableValue >> MutableValue&`

**功能**：在 Json 数组尾部删除元素

- **左侧参数类型**：`MutableValue`（json 数组）
- **右侧参数类型**：`MutableValue&`
- **返回值类型**：`MutableValue&` 左参自身引用

从尾部删除，支持链式调用。方法要求有 `MutableValue` 左值接收被删除的结点，实际
业务可弃用。

<!-- example:api_2_21_4_array_pop -->
```cpp
yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
yyjson::MutableValue p1, p2, p3;
mutDoc / "items" >> p3;
mutDoc / "items" >> p2 >> p1;

/* use method: pop() */
{
    yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
    yyjson::MutableValue p1, p2, p3;
    mutDoc["items"].pop(p3);
    mutDoc["items"].pop(p2).pop(p1);
}
```

#### 2.21.5 对象尾部删除 `pop`

**语法**：`MutableValue >> KeyValue&`

**功能**：在 Json 对象尾部删除元素

- **左侧参数类型**：`MutableValue`（json 对象）
- **右侧参数类型**：`KeyValue&`
- **返回值类型**：`MutableValue&` 左参自身引用

<!-- example:api_2_21_5_object_pop -->
```cpp
yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
yyjson::KeyValue name, age;
mutDoc / "user" >> age >> name;

/* use method: pop() */
{
    yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
    yyjson::KeyValue name, age;
    mutDoc["user"].pop(age).pop(name);
}
```

#### 2.21.6 数组迭代器定点删除 `remove`

**语法**：`iteratorT >> MutableValue&`

**功能**：可写数组迭代器定点删除当前元素，迭代器指向下一个元素

- **左侧参数类型**：`iteratorT` 限可写数组迭代器
- **右侧参数类型**：`MutableValue&`
- **返回值类型**：`iteratorT&` 左参自身引用

对应的 `remove` 方法返回 `MutableValue` ，但 `>>` 返回自身引用支持链式删除。
如果链式调用 `it.remove().remove()` ，第二个 `remove` 其实是尝试对第一个删除
返回的临时值（将其视为数组）再作删除，往往不是想要的。

<!-- example:api_2_21_6_array_iterator_remove -->
```cpp
yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
yyjson::MutableValue p1, p2,p3;
auto it = mutDoc / "items" % 0;
it >> p1;
it >> p2 >> p3;

/* use method: remove() */
{
    yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
    auto it = mutDoc / "items" % 0;
    auto p1 = it.remove();
    auto p2 = it.remove();
    auto p3 = it.remove();
}
```

#### 2.21.7 对象迭代器定点删除 `remove`

**语法**：`iteratorT >> KeyValue&`

**功能**：可写对象迭代器定点删除当前键值对，迭代器指向下一个键值对

- **左侧参数类型**：`iteratorT` 限可写对象迭代器
- **右侧参数类型**：`KeyValue&`
- **返回值类型**：`iteratorT&` 左参自身引用

对应的 `remove` 方法返回 `KeyValue` ，但 `>>` 返回自身引用支持链式删除。
`KeyValue` 没有 `remove` 方法，所以尝试对此链式调用 `remove` 将编译错误。

<!-- example:api_2_21_7_object_iterator_remove -->
```cpp
yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
yyjson::KeyValue name, age;
auto it = mutDoc / "user" % "";
it >> name >> age;

/* use method: remove() */
{
    yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
    auto it = mutDoc / "user" % "";
    auto name = it.remove();
    auto age = it.remove();
}
```

### 2.22 有序性比较 `<`

#### 2.22.1 Json 结点比较 `less`

**语法**：`jsonT < jsonT`

**功能**：Json 有序性定义小于比较

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`jsonT`
- **返回值类型**：`bool`

按如下规则定义两个 json 的大小，在需要的场合下规定某种确定的顺序：

- 首先按 yyjson 内部定义的类型编号（枚举）比较，如 null 最小，对象最大；
- 相同类型的标量，按值比较，Number 类型统一转 double 比较大小，String 类型采用
  `strcmp` 比较，Bool 值的 `false` 比 `true` 比，Null 值只有一个是相等的；
- 容器类型，只比较容器大小，不递归比较，如果容器大小相同，按内存地址比较。

<!-- example:api_2_22_1_json_less -->
```cpp
yyjson::Document doc = R"({"a": 10, "b": 20})"_xyjson;
bool result = doc / "a" < doc / "b";

/* use method: less() */
{
    auto result = doc["a"].less(doc["b"]);
}
```

#### 2.22.2 其他有序性比较操作符

除了 `<` 小于比较，还有其他几种大小比较，但都可以从 `<` 推导，所以一般自定义类
只需重载 `less` 方法。

- 大于 `>`：`a > b` 由 `b < a` 推导；
- 小于等于 `<=`：`a <= b` 由 `!(b < a)` 推导；
- 于于等于 `>=`：`a >= b` 由 `!(a < b)` 推导；

理论上，相等比较 `==` 也可由 `!(a < b) && !(b < a)` 推导。但是 xyjson 为各个核
心类都定义了自己的 `==` 比较操作。

### 2.23 相等性比较 `==`

#### 2.23.1 Json 结点值比较 `equal`

**语法**：`jsonT == jsonT`

**功能**：深度比较两个 json 树的内容，即递归比较其类型、容器大小及内容。

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`jsonT`
- **返回值类型**：`bool`

<!-- example:api_2_23_1_json_equal -->
```cpp
yyjson::Document doc1 = R"({"name": "Alice"})"_xyjson;
yyjson::Document doc2 = R"({"name": "Alice"})"_xyjson;
auto root1 = *doc1;
auto root2 = *doc2;

bool result = root1 == root2;

/* use method: equal() */
{
    bool result = root1.equal(root2);
}
```

#### 2.23.2 Json 叶结点与基本标量类型比较

**语法**：`jsonT == value`

**功能**：Json 结点值与给定值比较，要求类型匹配且内容相等。

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`scalarT`
- **返回值类型**：`bool`

<!-- example:api_2_23_2_json_scalar_equal -->
```cpp
yyjson::Document doc = R"({"name":"Alice", "age": 25})"_xyjson;

bool nameMatch = doc / "name" == "Alice"; // true
bool ageMatch = doc / "age" == 25; // true
bool ageFloatMatch = doc / "age" == 25.0; // false
```

#### 2.23.3 迭代器比较

**语法**：`iteratorT == iteratorT`

**功能**：比较两个迭代器是否相等

- **左侧参数类型**：`iteratorT`
- **右侧参数类型**：`iteratorT`
- **返回值类型**：`bool`

迭代器的很多操作前提都要判断迭代器是否失效，或者迭代器是否相等。只有从相同容器
创建的迭代器才可能相等，然后比较其当前索引。从相同容器任意起点创建的迭代器，经
过不断前进的前进，应该都能达到相同的终点失效状态。

<!-- example:api_2_23_3_iterator_equal -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;

// 用迭代器判断键是否存在
auto it = doc / "user" % "age";
if (it == doc["user"].endObject()) {} // false

/* use method: equal() */
if (it.equal(doc["user"].endObject())) {} // false

// 更简洁的判断
if(doc / "user" / "age") {} // true
if(doc / "user" % "age") {} // true
```

#### 2.23.4 不等比较 `!=`

在 xyjson 库中，所有重载定义了 `==` 的类，也都支持 `!=` 操作，后者明显可以且应
该由前者推导，也即相当于调用 `!equal()` 方法。

这在 begin/end 迭代器对循环中尤为常见。

<!-- example:api_2_23_4_array_iterator_not_equal -->
```cpp
yyjson::Document doc = R"({"items":[10, 20, 30]})"_xyjson;

int sum = 0;
for (auto it = doc["items"].beginArray(); it != doc["items"].endArray(); ++it)
{
    sum += *it | 0;
}

/* use method: equal() */
sum = 0;
for (auto it = doc["items"].beginArray(); !it.equal(doc["items"].endArray()); ++it)
{
    sum += *it | 0;
}
```

### 2.24 按位与运算 `&` - 类型判断

#### 2.24.1 基本类型判断 `isType`

**语法**：`jsonT & value`

**功能**：类型判断，使用字面量作为类型指示器

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`scalarT` 或类型常量
- **返回值类型**：`bool`

<!-- example:api_2_24_1_type_check -->
```cpp
yyjson::Document doc = R"({"name": "Alice", "age": 30})"_xyjson;
bool isString = doc / "name" & "";
bool isInt = doc / "age" & 0;
bool isReal = doc / "age" & 0.0;
bool isObject = doc.root() & "{}";
bool isArray = doc.root() & "[]";

/* use method: isType() */
{
    bool isString = doc["name"].isType("");
    bool isInt = doc["age"].isType(0);
    bool isReal = doc["age"].isType(0.0);
    bool isObject = doc.root().isType("{}");
    bool isArray = doc.root().isType("[]");
}
```

> **说明**:
操作符 `&` 与 `isType` 方法也接收类型常量作为参数，详情参考类型常量章节。

### 2.25 按位或运算 `|`

#### 2.25.1 带默认值提取 `getor`

**语法**：`jsonT | defaultValue`

**功能**：从 Json 结点提取值，如果无效或类型不匹配则返回默认值

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`scalarT`（基本类型）
- **返回值类型**：与右侧参数类型相同

带默认值提取操作符 `|` 是 xyjson 推荐的常用操作。基本上能用于 `&` 类型判断的参
数，也能用于 `|` 参数作为默认值提取。如果 `&` 类型判断返回 `true` ，那么 `|`
操作能转换提取 json 值，否则返回右侧的默认参数值。大部分情况下，没必要显式用
`&` 额外做类型判断，只要用 `|` 默认提取即可。

<!-- example:api_2_25_1_get_with_default -->
```cpp
yyjson::Document doc = R"({"name": "Alice", "age": 30, "active": true})"_xyjson;
const char* pszName = doc / "name" | "";
std::string name = doc / "name" | "";
int age = doc / "age" | 0;
bool active = doc / "active" | false;

/* use method: getor() */
{
    const char* pszName = doc["name"].getor("");
    std::string name = doc["name"].getor("");
    int age = doc["age"].getor(0);
    bool active = doc["active"].getor(false);
}
```

注意在上例中 `|""` 返回 C-Style 的字符串 `const char*` ，但可以赋值给
`std::string` ，相当于调用后者的构造函数。也支持 `| std::string()` 直接返回
`std::string` ，虽不如 `|""` 简洁，但意义明了。另外也支持 `| kString` 等类型常
量。

#### 2.25.2 管道函数转换 `pipe`

**语法**：`jsonT | function`

**功能**：对 Json 值应用管道函数进行转换

- **左侧参数类型**：`jsonT`
- **右侧参数类型**：`std::function` 等任意可调用对象
- **返回值类型**：与右侧函数返回值类型相同

右侧函数支持接收一个 jsonT 类型（`Value` 或 `MutableValue`）的参数，也支持基本
类型（数字或字符串），如果是后者，`pipe` 方法相当于先调用 `getor` 方法将左侧
json 转为基本类型（类型不匹配时返回目标类型的零值）再调用右侧函数，否则直接将
左侧 json 传给右侧函数。因此，`pipe` 相当于 `getor` 的超集，`getor` 相当于
`pipe` 的最常用基础版。

<!-- example:api_2_25_2_pipe_function -->
```cpp
yyjson::Document doc = R"({"name": "alice"})"_xyjson;
std::string upper = doc / "name" | [](const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::toupper);
    return r;
};

/* use method: pipe() */
{
    std::string upper = doc["name"].pipe([](const std::string& s) {
        std::string r = s;
        std::transform(r.begin(), r.end(), r.begin(), ::toupper);
        return r;
    });
}
```

### 2.26 赋值操作符 `=`

#### 2.26.1 自定义类的拷贝赋值

封装的 jsonT 类，相当于是 yyjson 底层结点的代理指针，它是值类，允许拷贝赋值。
多个拷贝的 `Value` 值引用相同的底层结点。但不建议拷贝 `MutableValue` 值，因为
通过一个拷贝修改底层结点也会影响另一个拷贝的观察或操作，容易混乱误解。

迭代器 iteratorT 类，也是值类，可以拷贝赋值，但是迭代器内部成员比 jsonT 复杂，
应该尽量避免拷贝赋值。

但是封装的 docT 类，它代理了 yyjson 底层的文档树与内存池，不允许拷贝赋值，只允
许移动赋值。但一般业务中也不建议移动 docT 文档对象。

#### 2.26.2 可写结点赋值 `set`

**语法**：`MutableValue = value`

**功能**：为可写的 json 标量叶结点赋新值，可以改变 json 标量类型，但避免对非空
容器重新赋值。

- **左侧参数类型**：`MutableValue`
- **右侧参数类型**：`scalarT`、`jsonT`、或类型常量
- **返回值类型**：`MutableValue&`

<!-- example:api_2_26_2_value_set -->
```cpp
yyjson::MutableDocument mutDoc(R"({"name": "Alice"})");
mutDoc / "name" = "Bob";
mutDoc["age"] = 30;
mutDoc / "active" = true; // 无效果
//^ 结果：{"name":"Bob","age":30}

/* use method: set() */
{
    yyjson::MutableDocument mutDoc(R"({"name": "Alice"})");
    (mutDoc / "name").set("Bob");
    mutDoc["age"].set(30);
    (mutDoc / "active").set(true);
}
```

注意，不能通过 `/` 为不存在的键赋值，那没有效果。`[]` 可以自动添加不存在的键，
先默认创建并添加 null 类型的 json 结点，再赋值。

### 2.27 复合赋值操作符

重载了一些复合赋值操作符，那是其他操作符与 `=` 的自然衍生组合。

#### 2.27.1 Json 复合赋值提取 `|=`

**语法**：`scalarT |= jsonT`

**功能**：从 Json 结点提取值并赋给原变量

- **左侧参数类型**：`scalarT&`
- **右侧参数类型**：`jsonT`
- **返回值类型**：`scalarT&`

从内部实现上讲，`|=` 操作比 `|` 更简捷，那直接调用 `get` 方法，而 `|` 调用
`getor` 。当类型不匹配时，不影响左参原值。

<!-- example:api_2_27_1_composite_assignment_extract -->
```cpp
yyjson::Document doc = R"({"age": 30})"_xyjson;
int age = 18;
age |= doc / "age";  // age = 30

/* use method: get() */
{
    int age = 18;
    doc["age"].get(age);
}
```

#### 2.27.2 迭代器多步前进 `+=`

**语法**：`iteratorT += n`

**功能**：迭代器多步前进，不会拷贝新迭代器

- **左侧参数类型**：`iteratorT&`
- **右侧参数类型**：`size_t`
- **返回值类型**：`iteratorT&`

<!-- example:api_2_27_2_iterator_multistep_advance -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30, 40]})"_xyjson;
auto iter = doc / "items" % 0;
iter += 3;  // 跳过 3 个元素

/* use method: advance() */
{
    auto iter = doc / "items" % 0;
    iter.advance(3);
}
```

#### 2.27.3 迭代器重定位 `%=`

**语法**：`iteratorT %= target`

**功能**：迭代器重定位，不会拷贝新迭代器

- **左侧参数类型**：`iteratorT&`
- **右侧参数类型**：`size_t` 或 `const char*`
- **返回值类型**：`iteratorT&`

<!-- example:api_2_27_3_iterator_relocate_assign -->
```cpp
yyjson::Document doc = R"({"items": [10, 20, 30, 40, 50]})"_xyjson;
auto iter = doc / "items" % 4;
iter %= 2;  // 重定位到索引 2

/* use method: advance() */
{
    auto iter = doc / "items" % 4;
    iter.advance(2);
}
```

### 2.28 Document 自动转调 Root 的操作符

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

#### 2.28.1 索引操作符 `[]`

**语法**：`docT[key]` 或 `docT[index]`

**功能**：单层索引根结点，作用于可写文档时会自动创建新字段

- **左侧参数类型**：`docT`
- **右侧参数类型**：`stringT`（对象键名）或 `size_t`（数组索引）
- **返回值类型**：`jsonT`（与 docT 对应的 Value 类型）

<!-- example:api_2_28_1_doc_index_operator -->
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

#### 2.28.2 一元正号 `+` 转整数

**语法**：`+docT`

**功能**：或取根结点转整数

- **参数类型**：`docT`
- **返回值类型**：`int`

<!-- example:api_2_28_2_doc_unary_plus -->
```cpp
yyjson::Document doc = R"([10, 20, 30])"_xyjson;
int size = +doc;  // 3

/* use method: root() */
{
    int size = +doc.root(); // 3
}
```

#### 2.28.3 一元负号 `-` 转字符串

**语法**：`-docT`

**功能**：取根结点转字符串

- **参数类型**：`docT`
- **返回值类型**：`std::string`

<!-- example:api_2_28_3_doc_unary_minus -->
```cpp
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::string str = -doc; // {"name":"Alice"}

/* use method: root() */
{
    std::string str = -doc.root();
}
```

#### 2.28.4 除法 `/` 路径查找

**语法**：`docT / key` 或 `docT / path` 或 `docT / index`

**功能**：类似 `[]` 索引的单层访问与线性查找，更方便链式逐级访问

- **左侧参数类型**：`docT`，常规 Json 根结点是对象或数组
- **右侧参数类型**：`stringT`（对象键名）或 `size_t`（数组索引）
- **返回值类型**：`jsonT`（与文档类型对应的结点类型）

<!-- example:api_2_28_4_doc_path_operator -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
    "items": [10, 20, 30]})"_xyjson;

auto name = doc / "user" / "name";
auto age = doc / "/user/age";
auto item = doc / "/items/0";

/* use method: root() */
{
    auto name = doc.root() / "user" / "name";
    auto age = doc.root() / "/user/age";
    auto item = doc.root() / "/items/0";
}
```

#### 2.28.5 取模 `%` 创建迭代器

**语法**：`docT % key` 或 `docT % index`

**功能**：为根结点创建迭代器，可指针起始索引或起始键名

- **左侧参数类型**：`docT`，要求对象或数组类型
- **右侧参数类型**：`const char*`（对象键名）或 `size_t`（数组索引）
- **返回值类型**：`iteratorT`（根结点对应的迭代器类型）

<!-- example:api_2_28_5_doc_iterator_operator -->
```cpp
yyjson::Document array("[10, 20, 30]");
yyjson::Document object(R"({"name": "Alice", "age": 30})");

auto arrIt = array % 0;
auto objIt = object % "";

/* use method: root() */
{
    auto arrIt = array.root() % 0;
    auto objIt = object.root() % "";
}
```

#### 2.28.6 左移 `<<` 标准流输出

**语法**：`std::ostream << docT`

**功能**：整个文档序列化输出

- **左侧参数类型**：`std::ostream&`
- **右侧参数类型**：`docT`
- **返回值类型**：`std::ostream&`

<!-- example:api_2_28_6_doc_stream_output -->
```cpp
yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
std::cout << doc << std::endl;
std::ostringstream oss1;
oss1 << doc;

/* use method: root() */
std::cout << doc.root() << std::endl;
std::ostringstream oss2;
oss2 << doc.root();

// 格式化输出需显式调用 toString(true)
std::cout << doc.root().toString(true) << std::endl;
std::ostringstream oss3;
oss3 << doc.root().toString(true);
```
#### 2.28.7 相等操作符 `==` 

**语法**：`docT == docT`

**功能**：两个文档内容是否相等

- **左侧参数类型**：`docT`
- **右侧参数类型**：`docT`
- **返回值类型**：`bool`

<!-- example:api_2_28_7_doc_equality -->
```cpp
yyjson::Document doc1 = R"({"name": "Alice"})"_xyjson;
yyjson::Document doc2 = R"({"name": "Alice"})"_xyjson;
bool docEqual = doc1 == doc2;

/* use method: root() */
{
    bool rootEqual = doc1.root() == doc2.root();
}
```

注意：比较两个 doc 相等的条件是它们的根结点内容相同。持有两个相同 yyjson 底层
的 doc 指针的 Document 类对象不一样相等，因为它们的根结点可能不一样。Document
的内存池可以创建多个 json 结点，其中任何一个结点都可以被设为根结点，不一定所有
结点都可以从根结点有路径可达。当然这种情况比较罕见，一般业务中不建议重设根结点
。那么 doc 指针相同的也往往意味着根结点指针相同，那么其内容也一定可以快速判断
为相等。

## 3 类型常量应用

xyjson 定义了一系列类型代表值常量，可以用于一些操作符的右侧参数。因为操作数只
能是值，不能是类型，所以定义一些具名的有意义常量，比直接用字面量增加可读性。当
然也需要交换简洁性，可考虑使用 `using` 引入命名空间。

<!-- example:NO_TEST -->
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

注意几点：
- 字面量 `0` 相当于 `kInt` ，如果想表达大整数，推荐使用 `kSint` 与 `kUint` ，
  这也与 yyjson 底层结点内存储整数的命名更一致。
- 字面量 `0.0` 只对应对实数或 `double` 类型的 `kReal` ，而 `kNumber` 是对应
  Json 类型的 Number 概念，它是实数与整数的统称；在 C/C++ 中尽量用更强的类型。
- 字面量 `""` 相当于 `kString` 。但有两个特殊字面 `"[]"` 与 `"{}"` 在一些操作
  中分别表示数组与对象类型。为了更强类型的区分，额外定义了三个空类的常量。

### 3.1 类型判断

<!-- example:api_3_1_type_check_with_constants -->
```cpp
yyjson::Document doc = R"({"name": "Alice", "age": 30})"_xyjson;

// 使用类型常量
{
    using namespace yyjson;
    bool isString = doc / "name" & kString; // true
    bool isInt = doc / "age" & kInt;        // true
    bool isNumber = doc / "age" & kNumber;  // true
    bool isDouble = doc / "age" & kReal;    // false
}

// 使用字面量
{
    bool isString = doc / "name" & "";   // true
    bool isInt = doc / "age" & 0;        // true
    // isNumber 只能用 kNumber 判断
    bool isDouble = doc / "age" & 0.0;   // false
}
```

### 3.2 值提取

在大部分情况下，`getor` 的操作符 `|` 右侧都是某个类型的零值，这适合用类型常量
。如果要提供其他默认值，可用字面量或变量。

<!-- example:api_3_2_value_extract_with_constants -->
```cpp
yyjson::Document doc = R"({"name": "Alice", "age": 30})"_xyjson;

// 使用类型常量
{
    using namespace yyjson;
    const char* pszName = doc / "name" | kString; // Alice
    int iAge = doc / "age" | kInt;      // 30
    double fAage = doc / "age" | kReal; // 0.0
    double nAage = doc / "age" | kNumber; // 30.0
}

// 使用字面量
{
    const char* pszName = doc / "name" | ""; // Alice
    int iAge = doc / "age" | 0;      // 30
    double fAage = doc / "age" | 0.0; // 0.0
    // kNumber 没有对应的字面量，或者可以用 ZeroNumber 构造函数
    double nAage = doc / "age" | ZeroNumber(); // 30.0
}
```

### 3.3 赋值修改

类型常量代表值都是各个类型的零值或空值，所以用于 `=` 右侧时将 json 内容改为对
应类型的零值或空值。

<!-- example:api_3_3_assignment_with_constants -->
```cpp
yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");

// 使用类型常量
{
    using namespace yyjson;
    mutDoc / "name" = kString;
    mutDoc / "age" = kInt;
}

// 使用字面量
{
    mutDoc / "name" = "";
    mutDoc / "age" = 0;
}
```

### 3.4 特殊容器类型 kArray kObject

为 json 容器类型定义了两个常量，`kArray` 表示数组，`kObject` 表示对象。它们相
对其他标量类型常量更复杂些。`Value` 与 `MutableValue` 的数组与对象也有些不同要
注意。

除了 `|` 操作特殊外，在其他一些操作符或方法中，也可以用 `"[]"` 代替 `kArray`，
用 `"{}"` 代替 `kObject`。但这两个特殊字面量是运行期作特殊判断的，`kArray` 与
`kObject` 是编译期决定类型的。所以在性能敏感的地方最好使用类型常量，能略微提升
性能。

#### 3.4.1 容器类型判断

<!-- example:api_3_4_1_container_type_check -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
    "items": [10, 20, 30]})"_xyjson;

// 使用类型常量
{
    using namespace yyjson;
    bool isArray = doc / "items" & kArray;    // true
    bool isObject = doc / "user" & kObject;   // true
}

// 使用字面量
{
    bool isArray = doc / "items" & "[]"; // true
    bool isObject = doc / "user" & "{}";    // true
}
```

#### 3.4.2 容器类型转换

在值提取操作符 `|` 右侧，不能用 `"[]"` 表示数组。因为 `"[]"` 与 `"{}"` 这两个
特殊字符串字面量的类型其实还是字符串，调用 `getor(const char\*)` 的重载，故与
`| ""` 一样表示提取字符串类型的 json 值。`json | "[]"` 操作的意义是，当 json
不是字符串类型时，返回默认值长度为 2 的字符串 `"[]"` 。

但是在 `|` 右侧可以使用 `kArray` 与 `kObject` ，当类型匹配时，它返回 `Value`
或 `MutableValue` 的一个子类专门用于表示数组或对象。

- `Value | kArray` 返回 `ConstArray`
- `Value | kObject` 返回 `ConstObject`
- `MutableValue | kArray` 返回 `MutableArray`
- `MutableValue | kObject` 返回 `MutableObject`

这四个子类分别继承了父类的所有方法，可以当作 `Value` 或 `MutableValue` 使用。
只额外增加两个方法 `begin` 与 `end` 分别表示创建起始与结束迭代器，使之能适配标
准容器迭代器的接口，能够使用范围 `for` 与一些标准库算法。

也就是说，`Value` 类只能用 `beginArray` 与 `endArray` 方法创建数组迭代器，这是
非标准接口。而 `ConstArray` 可以使用 `begin` 与 `end` 标准接口创建数组迭代器，
因为这个类只表示数组。其他子类的意义类似。

在这个操作下，`| kArray` 对应 `array()` 方法，`| kObject` 对应 `object()` 方法
。

<!-- example:api_3_4_2_container_type_conversion -->
```cpp
yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
    "items": [10, 20, 30]})"_xyjson;

auto array = doc / "items" | yyjson::kArray;
auto object = doc / "user" | yyjson::kObject;

// 使用具名方法
{
    auto array = doc["items"].array();
    auto object = doc["user"].object();
}

// 支持标准容器操作
int arraySum = 0;
for (auto value : array) {
    arraySum += value | 0;
}

int objectCount = 0;
for (auto value : object) {
    objectCount++;
}

// 等效显式使用 begin/end 迭代器对
{
    int arraySum = 0;
    for (auto it = array.begin(); it != array.end(); ++it) {
        auto value = *it;
        arraySum += value | 0;
    }
}

{
    int objectCount = 0;
    for (auto it = object.begin(); it != object.end(); ++it) {
        auto value = *it;
        objectCount++;
    }
}
```

#### 3.4.3 变更容器类型

当 `kArray` 或 `kObject` 用于 `=` 右侧时，表示将左侧的可写 json 重设为空数组或
空对象。

<!-- example:api_3_4_3_change_container_type -->
```cpp
yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
mutDoc / "name" = yyjson::kObject;
mutDoc / "age" = yyjson::kArray;
//^ 结果：{"name":{},"age":[]}

// 使用字面量
{
    yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
    mutDoc / "name" = "{}";
    mutDoc / "age" = "[]";
}

// 使用具名方法
{
    yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
    mutDoc["name"].setObject();
    mutDoc["age"].setArray();
}
```

#### 3.4.4 创建空容器

与 `kArray` 或 `kObject` 用于 `=` 右侧表示修改当前左侧参数既有 json 类型不同，
当它们用于 `<<` 操作符右侧时，将自动调用 `MutableDocument::create()` 方法先创
建空数组或空对象结点，再插入到当前操作的左侧参数容器中。

<!-- example:api_3_4_4_create_empty_container -->
```cpp
yyjson::MutableDocument mutDoc("{}");
*mutDoc << "user" << yyjson::kObject;
*mutDoc << "items" << yyjson::kArray;

// 使用字面量
{
    yyjson::MutableDocument mutDoc("{}");
    *mutDoc << "user" << "{}";
    *mutDoc << "items" << "[]";
}

mutDoc / "user" << "name" << "Alice" << "age" << 25;
mutDoc / "items" << 10 << 20 << 30;
```

注意：`kArray` 与 `kObject` 类型常量是设计为给 Value 类的操作符使用的。
Document 类的构造函数中传 `"{}"` 就表示解析 json 字符串，恰好它也是表示空对象
。构造函数不支持 `kObject` 与 `kArray` 参数，但是可写文档在构造之后可以重设
根结点的类型。

<!-- example:api_3_4_4_set_root_container_type -->
```cpp
yyjson::MutableDocument mutDoc; // 默认构造是空对象 "{}"
mutDoc.root() = yyjson::kArray;

mutDoc.root() << 10 << 20 << 30;
//^ 结果：[10,20,30]
```

## 4 获取 yyjson 底层指针

xyjson 提供了一些方法可以从封装类中获取底层 yyjson 结构体指针，可用于想直接调
用 C API 的场景。

### 4.1 使用无参数的 `get` 方法

- `Value::get()` 返回 `yyjson_val*`
- `Document::get()` 返回 `yyjson_doc*`
- `MutableValue::get()` 返回 `yyjson_mut_val*`
- `MutableValue::getDoc()` 返回 `yyjson_mut_doc*`
- `MutableDocument::get()` 返回 `yyjson_mut_doc*`

<!-- example:api_4_1_get_pointer_method -->
```cpp
yyjson::Document doc(R"({"name": "Alice", "age":25})");
yyjson::MutableDocument mutDoc = ~doc;

yyjson_doc* ptrDoc = nullptr;
yyjson_val* ptrVal = nullptr;
ptrDoc = doc.get();             // not null
ptrVal = doc["name"].get();     // not null
ptrVal = (doc / "nokey").get(); // nullptr
ptrVal = (doc["nokey"]).get();  // nullptr

yyjson_mut_doc* ptrMutDoc = nullptr;
yyjson_mut_val* ptrMutVal = nullptr;
ptrMutDoc = mutDoc.get();          // not null
ptrMutVal = mutDoc["name"].get();     // not null
ptrMutDoc = mutDoc["name"].getDoc();  // not null
ptrMutVal = (mutDoc / "nokey").get(); // nullptr
ptrMutVal = (mutDoc["nokey"]).get();  // not null, auto insert
ptrMutVal = (mutDoc / "nokey").get(); // not null
```

### 4.2 使用 `>>` 操作符或 `get` 方法

右侧参数是结构体指针的引用，将修改参数的指针值，返回 `bool` 表示获取成功。

- `Value >> yyjson_val*&` 或 `Value::get(yyjson_val*&)`
- `MutableValue >> yyjson_mut_val*&` 或 `MutableValue::get(yyjson_mut_val*&)`
- `MutableValue >> yyjson_mut_doc*&` 或 `MutableValue::get(yyjson_mut_doc*&)`

左侧参数只支持 Value 类，不支持 Document 类，因为 `>>` 操作符的主要目的是提取
json 叶结点至标量。`MutableValue` 类中也有 `yyjson_mut_doc*` 成员，可以提取。
但不能从 `Value` 类中提取 `yyjson_doc*` 。

<!-- example:api_4_2_extract_pointer_operator -->
```cpp
yyjson::Document doc(R"({"name": "Alice", "age":25})");
yyjson::MutableDocument mutDoc = ~doc;

yyjson_val* ptrVal = nullptr;
yyjson_mut_doc* ptrMutDoc = nullptr;
yyjson_mut_val* ptrMutVal = nullptr;

if (doc / "name" >> ptrVal) { }      // true
if (mutDoc / "name" >> ptrMutVal) {} // true
if (mutDoc / "name" >> ptrMutDoc) {} // true

if (doc / "nokey" >> ptrVal) { }      // false
if (mutDoc / "nokey" >> ptrMutVal) {} // false
if (mutDoc["nokey"] >> ptrMutVal) {}  // true
if (mutDoc / "nokey" >> ptrMutVal) {} // true
```

### 4.3 使用 `c_` 系列函数

命名源于 `std::string` 的 `c_str` 方法获取 C-Style 的字符串 `const char*`，
xyjson 的各个封装类也定义了相应的 `c_` 方法获取底层 C 结构体指针。

- `Value::c_val()` 返回 `yyjson_val*`
- `Document::c_doc()` 返回 `yyjson_doc*`
- `MutableValue::c_val()` 返回 `yyjson_mut_val*`
- `MutableValue::c_doc()` 返回 `yyjson_mut_doc*`
- `MutableDocument::c_doc()` 返回 `yyjson_mut_doc*`
- `iteratorT::c_iter()` 返回各迭代器内部的 C 迭代器结构体指针
- `iteratorT::c_val()` 返回迭代器当前值结点 `yyjson_val*` 或 `yyjson_mut_val*`
- `iteratorT::c_key()` 返回迭代器当前键结点 `yyjson_val*` 或 `yyjson_mut_val*`

其中，数组迭代器没有定义 `c_key` 方法。失效的迭代器 `c_val` 或 `c_key` 方法返
回空指针，但 `c_iter` 返回值始终非空。

<!-- example:api_4_3_c_style_functions -->
```cpp
yyjson::Document doc(R"({"user":{"name": "Alice", "age":25},
    "items":[10, 20, 30]})");
yyjson::MutableDocument mutDoc = ~doc;

yyjson_val* ptrVal = nullptr;
yyjson_doc* ptrDoc = nullptr;
yyjson_mut_doc* ptrMutDoc = nullptr;
yyjson_mut_val* ptrMutVal = nullptr;

// 以下 c_ 方法都返回非空指针
ptrDoc = doc.c_doc();
ptrVal = doc["user"].c_val();
ptrMutDoc = mutDoc.c_doc();
ptrMutDoc = mutDoc["user"].c_doc();
ptrMutVal = mutDoc["user"].c_val();

auto ita = doc / "items" % 0;
auto ito = doc / "user" % "";
auto mita = mutDoc / "items" % 0;
auto mito = mutDoc / "user" % "";

// 以下 c_ 方法也返回非空指针
bool itaIter = (ita.c_iter() != nullptr);
bool itoIter = (ito.c_iter() != nullptr);
bool mitaIter = (mita.c_iter() != nullptr);
bool mitoIter = (mito.c_iter() != nullptr);

bool itaVal = (ita.c_val() != nullptr);
bool itoVal = (ito.c_val() != nullptr);
bool mitaVal = (mita.c_val() != nullptr);
bool mitoVal = (mito.c_val() != nullptr);

bool itoKey = (ito.c_key() != nullptr);
bool mitoKey = (mito.c_key() != nullptr);

//! 数组迭代器没有定义 c_key 方法
//! if (ita.c_key() != nullptr) {}
//! if (mita.c_key() != nullptr) {}
```

## 5 条件编译宏功能控制

xyjson 提供了一些条件编译宏用于在具体项目禁用不必要的功能，默认是所有功能开启
的。这些宏可以在构建系统（如 CMakeLists.txt）中定义，也可以在
`#include "xyjson.h"` 之前手动 `#define` 。建议使用前者，能保证在整个项目中使
用一样的条件编译宏，避免在不同编译单元中包含同一个头文件却实际编译不同的定义而
出现混乱。

注意，只需定义这些宏，而不必定义其值，其值定义为 `1` 或 `0` 都认为已定义，效果
都一样，表达禁用的含义。

### 5.1 禁用可写模型 `XYJSON_DISABLE_MUTABLE`

若定义了该宏，所有可写模型类，包括 MutableDocumet 、MutableValue 及其相应的迭
代器类与容器子类，都不可用。

yyjson 库的只读模型效率比可写模型高很多，如果项目没有可写需求，只需用到读 Json
的功能，可定义该宏。xyjson 的头文件中也有远超一半的代码在处理可写的问题，如果
禁用可写，将大幅降低实际代码量，编译速度也将有所提升。

### 5.2 禁用对象链式插入 `XYJSON_DISABLE_CHAINED_INPUT`

若定义该宏，则不再支持可写对象及其迭代器链式插入，如 `object << key << value`
与 `objectIterator << key << value` 将不能通过编译。

禁用该功能，可使 `MutableValue` 类的大小从 24 字节降为 16 字节（64位操作系统）
即两个指针的大小。由于其他许多操作，如路径操作、迭代器解引用操作等都返回
`MutableValue` 的值，小对象不仅减少内存占用，也能提升运行效率。
