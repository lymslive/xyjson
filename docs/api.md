# xyjson API 参考手册

本手册提供 xyjson 库的完整 API 参考，作为查询手册使用，内容详尽、务实。

## 核心类列表

- **`xyjson::Document`** - 只读 JSON 文档包装器
- **`xyjson::MutableDocument`** - 可写 JSON 文档包装器  
- **`xyjson::Value`** - 只读 JSON 值包装器
- **`xyjson::MutableValue`** - 可写 JSON 值包装器
- **`xyjson::ArrayIterator`** / **`xyjson::MutableArrayIterator`** - 数组迭代器
- **`xyjson::ObjectIterator`** / **`xyjson::MutableObjectIterator`** - 对象迭代器

## 操作符总览表

| 操作符 | 适用类 | 第二参数类型 | 功能描述 | 优先级 | 推荐度 |
|--------|--------|-------------|----------|--------|--------|
| `/` | 所有类 | `const char*`, `int` | 路径访问，支持 JSON Pointer | 5 | ★★★★★ |
| `\|` | Value类 | 基本类型 | 值提取（带默认值） | 12 | ★★★★★ |
| `\|` | Value类 | 函数 | 管道函数自定义转换 | 12 | ★★★★☆ |
| `\|=` | MutableValue | 基本类型 | 复合赋值提取 | 16 | ★★★★☆ |
| `&` | Value类 | 类型代表值 | 类型判断 | 11 | ★★★★☆ |
| `[]` | 所有类 | `const char*`, `int` | 单层索引访问 | 2 | ★★★★★ |
| `=` | MutableValue | 基本类型 | 赋值操作 | 16 | ★★★★★ |
| `<<` | MutableValue | 各种类型 | 智能输入（追加/插入/赋值） | 7 | ★★★★☆ |
| `<<` | Document类 | `std::ostream` | 流输出 | 7 | ★★★★☆ |
| `>>` | Value类 | 引用类型 | 安全值提取 | 7 | ★★★★☆ |
| `*` | Document类 | 无 | 一元解引用，取根结点 | 3 | ★★★★☆ |
| `*` | 所有类 | MutableValue | 二元乘，创建 JSON 结点 | 5 | ★★★☆☆ |
| `+` | Value类 | 无 | 一元正号，转整数 | 3 | ★★★★☆ |
| `+` | Document类 | 无 | 一元正号，取根结点转整数 | 3 | ★★★★☆ |
| `-` | Value类 | 无 | 一元负号，转字符串 | 3 | ★★★★☆ |
| `-` | Document类 | 无 | 一元负号，取根结点转字符串 | 3 | ★★★★☆ |
| `~` | Document类 | 无 | 一元取反，模式转换 | 3 | ★★★☆☆ |
| `%` | Value类 | `int`, `const char*` | 创建迭代器 | 5 | ★★★☆☆ |
| `%=` | 迭代器类 | `int`, `const char*` | 迭代器搜索前进 | 5 | ★★★☆☆ |
| `++` | 迭代器类 | 无 | 迭代器自增 | 3 | ★★★☆☆ |
| `+=` | 迭代器类 | `int` | 迭代器步进 | 16 | ★★★☆☆ |
| `!` | 所有类 | 无 | 错误判断 | 3 | ★★★★☆ |
| `==` | 所有类 | JSON类 | 相等性判断 | 10 | ★★★★☆ |
| `!=` | 所有类 | JSON类 | 不等性判断 | 10 | ★★★★☆ |

> **重要说明**：操作符重载不能改变 C++ 固有的优先级和结合律，使用时需注意操作符的正确组合。

## `/` 路径操作符

### 路径访问操作符

**语法**：`json / path` 或 `doc / path`

**功能**：支持链式路径访问和 JSON Pointer 语法

#### 操作符版本

```cpp
// 链式路径访问
xyjson::Value node = doc / "user" / "profile" / "name";

// JSON Pointer 路径
xyjson::Value node = doc / "/user/profile/name";

// 数组索引访问
xyjson::Value item = doc / "items" / 0;

// 混合使用
xyjson::Value value = doc / "/user/friends" / 0 / "name";
```

#### 对应方法版本

```cpp
// 使用 pathto 方法
xyjson::Value node = doc.pathto("user").pathto("profile").pathto("name");

// JSON Pointer 方法
xyjson::Value node = doc.pathto("/user/profile/name");

// 索引方法
xyjson::Value item = doc.pathto("items").index(0);
```

**功能说明**：
- 支持单层索引：`doc / "key"` 或 `doc / 0`
- 支持链式访问：`doc / "user" / "name"`
- 支持 JSON Pointer：以 `/` 开头的路径使用标准 JSON Pointer 语法
- 路径缓存：中间结果可以缓存以提高性能

## `|` 取值操作符

### 基本类型提取

**语法**：`json | defaultValue`

#### 操作符版本

```cpp
// 基本类型提取
int age = doc / "age" | 0;
std::string name = doc / "name" | "unknown";
bool active = doc / "active" | false;
double price = doc / "price" | 0.0;
```

#### 方法版本

```cpp
// 方法版本
int age = doc / "age".getor(0);
std::string name = doc / "name".getor("unknown");
bool active = doc / "active".getor(false);
double price = doc / "price".getor(0.0);
```

### 管道函数转换

#### 操作符版本

```cpp
// 自定义转换
std::string uppercase = doc / "name" | [](const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
} | "DEFAULT";

// 链式管道
std::string result = doc / "data" 
    | [](const xyjson::Value& v) { return v.toString(); }
    | [](const std::string& s) { return "Result: " + s; }
    | "Default";
```

#### 方法版本

```cpp
// 方法版本
std::string result = doc / "name".pipe([](const std::string& s) {
    return s + "_processed";
}).getor("default");
```

## `|=` 复合赋值提取

### 操作符版本

```cpp
xyjson::MutableValue node = mutDoc / "config";
int timeout = 30;
node |= timeout;  // 如果存在则提取，否则保持原值
```

### 方法版本

```cpp
xyjson::MutableValue node = mutDoc / "config";
int timeout = 30;
node.get(timeout);  // 返回 bool 表示是否成功提取
```

## `&` 类型判断操作符

### 操作符版本

```cpp
// 基本类型判断
bool isString = doc / "name" & "";
bool isNumber = doc / "age" & 0;
bool isBool = doc / "active" & false;

// 复杂类型判断
bool isObject = doc / "user" & "{}";
bool isArray = doc / "items" & "[]";

// 使用类型常量
bool isString = doc / "name" & xyjson::kStr;
bool isNumber = doc / "age" & xyjson::kInt;
```

### 方法版本

```cpp
// 使用 isType 方法
bool isString = doc / "name".isType("");
bool isNumber = doc / "age".isType(0);
bool isObject = doc / "user".isType("{}");

// 具体类型方法
bool isString = doc / "name".isString();
bool isNumber = doc / "age".isNumber();
bool isObject = doc / "user".isObject();
```

## `[]` 单层索引操作符

### 操作符版本

```cpp
// 对象键访问
xyjson::Value name = doc["user"]["name"];

// 数组索引访问
xyjson::Value first = doc["items"][0];

// 可写模型的自动插入
mutDoc["new_field"] = "value";  // 自动创建新字段
```

### 方法版本

```cpp
// 使用 index 方法
xyjson::Value name = doc.index("user").index("name");
xyjson::Value first = doc.index("items").index(0);

// 可写模型的设置
mutDoc.index("new_field").set("value");
```

## `=` 赋值操作符

### 操作符版本

```cpp
// 基本类型赋值
mutDoc / "name" = "Alice";
mutDoc / "age" = 30;
mutDoc / "active" = true;

// 复杂赋值
mutDoc / "user" = "{}";
mutDoc / "user" / "name" = "Bob";
```

### 方法版本

```cpp
// 使用 set 方法
mutDoc / "name".set("Alice");
mutDoc / "age".set(30);
mutDoc / "active".set(true);
```

## `<<` 智能输入操作符

### 操作符版本

```cpp
// 数组追加
mutDoc / "items" << 1 << "two" << 3.14;

// 对象插入键值对
mutDoc.root() << "key1" << "value1" << "key2" << 42;

// 标量赋值
mutDoc / "scalar" << "new_value";
```

### 方法版本

```cpp
// 使用 input 方法
mutDoc / "items".input(1).input("two").input(3.14);
mutDoc.root().input("key1").input("value1");
```

## `>>` 安全值提取操作符

### 操作符版本

```cpp
int age;
if (doc / "age" >> age) {
    // 成功提取
}

std::string name;
if (doc / "name" >> name) {
    // 成功提取
}

// 底层指针提取
yyjson_val* nodePtr;
if (doc / "data" >> nodePtr) {
    // 使用原生 yyjson API
}
```

### 方法版本

```cpp
// 使用 get 方法
int age;
if (doc / "age".get(age)) {
    // 成功提取
}
```

## `*` 解引用操作符

### 一元解引用（取根结点）

#### 操作符版本

```cpp
xyjson::Value root = *doc;
xyjson::MutableValue mutRoot = *mutDoc;
```

#### 方法版本

```cpp
xyjson::Value root = doc.root();
xyjson::MutableValue mutRoot = mutDoc.root();
```

### 二元乘（创建 JSON 结点）

#### 操作符版本

```cpp
xyjson::MutableValue newNode = *mutDoc * "new_value";
xyjson::MutableValue newArray = *mutDoc * "[]";
```

#### 方法版本

```cpp
xyjson::MutableValue newNode = mutDoc.create("new_value");
xyjson::MutableValue newArray = mutDoc.create("[]");
```

## `+` 一元正号操作符

### 操作符版本

```cpp
// Value 类：转换为数字
int size = +(doc / "items");  // 获取数组/对象大小

// Document 类：取根结点转数字  
int rootSize = +doc;  // 相当于 +(doc.root())
```

### 方法版本

```cpp
// 使用 toNumber 方法
int size = doc / "items".toNumber();
int rootSize = doc.root().toNumber();
```

## `-` 一元负号操作符

### 操作符版本

```cpp
// Value 类：转换为字符串
std::string str = -(doc / "name");

// Document 类：取根结点转字符串
std::string rootStr = -doc;  // 相当于 -(doc.root())
```

### 方法版本

```cpp
// 使用 toString 方法
std::string str = doc / "name".toString();
std::string rootStr = doc.root().toString();
```

## `~` 模式转换操作符

### 操作符版本

```cpp
// 只读转可写
xyjson::MutableDocument mutCopy = ~doc;

// 可写转只读  
xyjson::Document readOnlyCopy = ~mutDoc;
```

### 方法版本

```cpp
// 使用 mutate/freeze 方法
xyjson::MutableDocument mutCopy = doc.mutate();
xyjson::Document readOnlyCopy = mutDoc.freeze();
```

## `%` 迭代器创建操作符

### 操作符版本

```cpp
// 数组迭代器
for (auto iter = doc / "items" % 0; iter; ++iter) {
    // 处理数组元素
}

// 对象迭代器
for (auto iter = doc / "user" % ""; iter; ++iter) {
    // 处理对象键值对
}
```

### 方法版本

```cpp
// 使用 arrayIter/objectIter 方法
for (auto iter = doc / "items".arrayIter(0); iter; iter.next()) {
    // 处理数组元素
}
```

## `%=` 迭代器搜索操作符

### 操作符版本

```cpp
auto iter = doc / "users" % 0;

// 搜索特定索引
iter %= 5;  // 搜索索引为5的元素

// 搜索特定键
iter %= "name";  // 搜索键为"name"的元素
```

### 方法版本

```cpp
// 使用 seek 方法
auto iter = doc / "users".arrayIter(0);
iter.seek(5);  // 搜索到索引5
```

## `++` 迭代器自增操作符

### 操作符版本

```cpp
auto iter = doc / "items" % 0;
++iter;  // 前进到下一个元素
```

### 方法版本

```cpp
auto iter = doc / "items".arrayIter(0);
iter.next();  // 前进到下一个元素
```

## `+=` 迭代器步进操作符

### 操作符版本

```cpp
auto iter = doc / "items" % 0;
iter += 3;  // 跳过3个元素
```

### 方法版本

```cpp
auto iter = doc / "items".arrayIter(0);
iter.advance(3);  // 跳过3个元素
```

## `!` 错误判断操作符

### 操作符版本

```cpp
if (!doc) {
    // 文档无效
}

if (!(doc / "nonexistent")) {
    // 路径不存在
}
```

### 方法版本

```cpp
if (doc.hasError()) {
    // 文档有错误
}

if (!doc / "path".isValid()) {
    // 结点无效
}
```

## `==` / `!=` 相等性判断操作符

### 操作符版本

```cpp
// 文档比较
if (doc1 == doc2) {
    // 内容相同
}

// 值比较
if (doc / "version" == "1.0") {
    // 版本匹配
}

// 不等性判断
if (doc / "status" != "active") {
    // 状态不匹配
}
```

### 方法版本

```cpp
// 使用 equal 方法
if (doc1.equal(doc2)) {
    // 内容相同
}

if (doc / "version".equal("1.0")) {
    // 版本匹配
}
```

## 类型常量定义

xyjson 定义了一系列类型代表值常量，用于类型判断和值提取：

```cpp
namespace xyjson {
    // 基本类型常量
    constexpr auto kInt = 0;
    constexpr auto kStr = "";
    constexpr auto kBool = false;
    constexpr auto kDouble = 0.0;
    
    // 复杂类型常量
    constexpr auto kObject = "{}";
    constexpr auto kArray = "[]";
    
    // 底层指针类型
    constexpr auto kNode = nullptr;
    constexpr auto kMutNode = nullptr;
    constexpr auto kMutDoc = nullptr;
}
```

### 使用示例

```cpp
// 类型判断
bool isInt = doc / "value" & xyjson::kInt;
bool isObject = doc / "data" & xyjson::kObject;

// 值提取
int value = doc / "number" | xyjson::kInt;
std::string text = doc / "text" | xyjson::kStr;

// 底层指针提取
yyjson_val* node = doc / "node" | xyjson::kNode;
```

## 操作符命名常量

为了方便理解和文档编写，xyjson 定义了操作符的命名常量：

```cpp
namespace xyjson {
    // 操作符常量
    constexpr auto okPath = "/";
    constexpr auto okExtract = "|";
    constexpr auto okPipe = "|";
    constexpr auto okIndex = "[]";
    constexpr auto okAssign = "=";
    constexpr auto okInput = "<<";
    constexpr auto okSafeGet = ">>";
    constexpr auto okDeref = "*";
    constexpr auto okToNumber = "+";
    constexpr auto okToString = "-";
    constexpr auto okConvert = "~";
    constexpr auto okIter = "%";
    constexpr auto okNot = "!";
    constexpr auto okEqual = "==";
    constexpr auto okNotEqual = "!=";
}
```

这些常量在当前代码中主要用于文档目的，帮助理解操作符的语义。