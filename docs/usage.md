# xyjson 使用指南

本指南从库使用者的角度，详述如何利用 xyjson 库在 C++ 项目中执行常见的 JSON 操作。

## 概述

xyjson 基于 [yyjson](https://github.com/ibireme/yyjson) 高性能 JSON 库构建，继承其两种核心模型：

- **只读模型** (`yyjson_doc`, `yyjson_val`) - 高性能解析，零拷贝访问
- **可写模型** (`yyjson_mut_doc`, `yyjson_mut_val`) - 灵活修改，完整构建

JSON 标准分为六大类型：对象、数组、字符串、数字、bool 与 null 。其中对象与数组
是容器类型，其他的是标量类型。在 Json 树上，标量位于叶结点，容器位于中间子树结
点。yyjson 的只读模型与可写模型的本质区别在于容器类型的内存布局上，只读模型是
线性的紧凑布局，可写模型是环形链表的开放布局。

在 C/C++ 语言中，对 Json 数字类型又有更严格的细分类型，主要分为整数与浮点数，
而整数除了最常用的 32 位 `int` ，还有大整数 `uint64_t` 与 `init64_t`。在
yyjson 结点中，用 8 字节来存储各种数字类型。

对于字符串类型，yyjson 结点保存其指针与长度信息，字符串具体内容则拷到独立的内
存池中。但对于具有安全生命周期的字符串（尤其是字符串字面量），yyjson 支持免拷
贝，只在结点中保存指针引用。

然后，xyjson 库在 `yyjson::` 命名空间浅封装了四个对应的类：
- Document: 仅 `yyjson_doc*` 指针
- Value: 仅 `yyjson_val*` 指针
- MutableDocument: 仅 `yyjson_mut_doc*` 指针
- MutableValue: 包括 `yyjson_mut_val*` 与 `yyjson_mut_doc*` 指针方便修改

Document 与 MutableDocument 负责整个 Json 树的内存自动管理，而 Value 与
MutableValue 是对某个 Json 结点的引用，其中根结点可用 root 方法获取。根结点一
般是容器类型，但简单标量也是合法 Json。这些类再通过操作符重载封装了常用操作，
同时提供对应的具名方法供不同编程风格的用户选择。

注：本文的示例省去了必要的头文件包含行 `#include "xyjson.h"` 。

## JSON 模型基本操作

本节讲解只读模型与可写模型都支持的基本操作， Document 一般泛指两个文档类，
Value 泛指两个 Json 结点类。

### Document 整体的读写操作

#### 读入操作 `<<`

Document 类对象可以从字符串直接构造：
```cpp
std::string jsonText = R"({"name": "Alice", "age": 30})";
yyjson::Document doc(jsonText);
```

它将解析输入的字符串参数，在它内部的内存池上构建 DOM 树模型。如入参数不是合法
的 Json 串，将解析失败，`doc` 对象将处理无效状态，可以用 `hasError` 方法判断是
否有解析错误。此外，Document 、Value 及后文介绍的迭代器类都重载了 `operator
bool` ，故也可直接在 `if` 条件中判断各类对象有效性。

```cppp
yyjson::Document doc1("I'm not json");
if (doc1.hasError()) {
    std::cout << "非法 Json\n";
}

// 仅有一个字符串类型的根结点的 Json
yyjson::Document doc2("\"I'm sure json\"");
if (doc2) {
    std::cout << "合法 Json\n";
}
```

也能在创建对象后用 `<<` 操作符或 `read` 方法读入字符串:
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";
if (!doc) {
  std::cerr << "JSON 解析失败" << std::endl;
}
```

当将 Document 类作为其他类的成员时，可能更需求延后读入。但要注意如果 `doc` 已
经是解析过输入 Json，再重新读放时会释放原来的 Json 。若业务上想避免重复初入的
初始化，也可先用 `if` 判断：

```cpp
yyjson::Document doc;
if (!doc) {
  doc << R"({"name": "Alice", "age": 30})"; // 第一次读入
}

if (!doc) {
  doc << R"({"name": "Alice", "age": 35})"; // 不会二次读入
}
```

Document 也支持从文件读取 Json ，但提供文件名参数时，只能用 `readFile` 方法，
不能用 `<<` 操作符，避免与 Json 串本身作为参数的歧义。使用文件指针 `FILE*` 或
`std::iftream` 文件流对象时，支持用 `<<` 操作符。

```cpp
const char* filePath = "/tmp/input.json";
yyjson::Document doc;
docFromFile.readFile(filePath);

FILE* fp = fopen(filePath, "r");
if (fp) {
    doc << fp;
}

std::ifstream ifs(filePath);
if (ifs) {
    doc << ifs;
}
```

最后，Document 类也支持从 yyjson 底层指针 `yyjson_doc*`(`yyjson_mut_doc*`)
。yyjson 库在解析 Json 时支持一些选项，如果你需要用到那些高级功能，可以先用它
的 C API 生成 `yyjson_doc*` ，再传给 Document 类构造。这样，`yyjson_doc*` 的所
有权也转给了 Document 对象，在析构时自动释放，注意不要再手动释放了避免二次
free 的错误。

#### 写出操作 `>>`

输出是输入的逆操作，Document 提供 `wirte` 方法及 `>>` 操作符将整个 Json 树序列化
输出至右侧参数目标。支持的参数与 `<<` 类似：

- 字符串，只支持 `std::string`, 而 C 风格的字符串指针不能当作安全的可写目标；
- 写模式打开的文件指针 `FILE*` ；
- 输出流文件 `std::ofstrem` ；
- 文件名参数，使用 `writeFile` 不能用 `>>` 操作符。

可写的 Document 输出操作更常用些，修改后序列化保存。

```cpp
std::string jsonText = R"({"name": "Alice", "age": 30})";
yyjson::MutableDocument mutDoc(jsonText);

std::string strTarget;
mutDoc >> strTarget;

const char* filePath = "/tmp/output.json"
FILE* fp = fopen(filePath, "w");
if (fp) {
  mutDoc >> fp;
}

std::ofstream ofs(filePath);
if (ofs) {
  mutDoc >> ofs;
}

mutDoc.writeFile(filePath);
```

除了输出文件流，Document 也支持写向任意标准输出流，只不过要换个方向的操作符
`<<` ，标准流写左边，Document 写右边：

```cpp
std::string jsonText = R"({"name": "Alice", "age": 30})";
yyjson::MutableDocument mutDoc(jsonText);
std::cout << mutDoc << std::endl;
```

向标准流输出的通用 `<<` 操作符实际是调用后文介绍的 Value 的 `toString` 方法，
只为提供方便功能，效率上略低 `>>` 操作符或 `write` 方法。另注意 `write` 方法都
是无格式序列化，即紧凑的单行 Json ，`toString` 有可先参数支持常规缩进美化输出。

- **性能提示**：优先选用只读的 Document，只在必要时使用 MutableDocument。
- **错误警示**：Document 与 MutableDocument 禁用拷贝。

### Value 结点的访问

#### 根结点

从概念上讲，一棵 Json 树有一个根结点。Document 类也有个 `root` 方法可以返回一
个 Value 类型的根结点，或者用一元操作符 `*`。不过在 xyjson 中，很多时候不需要
感知根结点的存在，直接操作 Document 就相当于操作其根结点。

```cpp
std::string jsonText = R"({"name": "Alice", "age": 30})";
yyjson::Document doc(jsonText);

// yyjson::Document root = doc.root();
auto root = *doc; 

// 以下三行输出相同的结果
std::cout << root << std::endl;
std::cout << doc << std::endl;
std::cout << *doc << std::endl;
```

#### 索引操作 `[]`

按索引或路径访问属于 Value 类的方法，但也可直接作用于 Document ，自动转为根结
点调用。

Value 支持用常规的 `[]` 索引访问对象或数组：
- Json 数组索引参数是整数，类似 `std::vector`
- Json 对象索引参数是字符串，类似 `std::map`

与标准容器不同的，`Value[]` 返回另一个 `Value` 值，而不是 Value 对象引用。当
Json 数组索引越界或对象值不存在时，返回一个无效的 `Value` 。若返回有效的
`Value` 值，它内部封装的指针指向真实存在的子结点。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "score": [10, 20, 30]})";

auto json = doc["name"];
std::cout << json << std::endl; // 输出: Alice

json = doc["age"];
std::cout << json << std::endl; // 输出: 30

json = doc["sex"];
if (!json) {
    std::cout << "不可说" << std::endl;
}

json = doc["score"][0];
std::cout << json << std::endl; // 输出: 10

json = doc["score"][10];
if (!json) {
  std::cout << "数组越界" << std::endl;
}
```

#### 路径操作 `/`

但更推荐使用路径操作符 `/` 平替索引操作符 `[]`，这几乎是 xyjson 库的精髓与灵感
来源。在链式路径操作中，`/` 比 `[]` 更直观方便。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "score": [10, 20, 30],
    "friend": [{"name": "Bob", "age": 35}, {"name": "Cous", "age": 20}]
})";

auto json = doc / "friend" / 0 / "age";
std::cout << json << std::endl; // 输出: 35

// 字段拼写错误
json = doc / "Friend" / 1 / "age";
std::cout << json << std::endl; // 输出: 空
if (!json) {
    std::cout << "路径操作错误" << std::endl;
}
```

- **性能提示**：对于反复的长链路径操作，考虑缓存中间结果。
- **错误警示**：注意 `/` 优先级，避免在其后混用具名方法。

#### JSON Pointer 标准的路径操作

路径操作符 `/` 另一强于索引操作符 `[]` 之处在于可以访问多级路径，要求遵循 JSON
Pointer 标准，必须以 `/` 开头，中间键名本身若含 `/` 或 `~` 特殊字符需要分别转
义为 `~1` 与 `~1`。当然对于实际项目，强烈建议避免键名含特殊字符，仅须注意以
`/` 开头。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "score": [10, 20, 30],
    "friend": [{"name": "Bob", "age": 35}, {"name": "Cous", "age": 20}]
})";

auto json = doc / "/friend/0/age";
std::cout << json << std::endl; // 输出: 35

// 不是 Json Pointer ，与 [] 一样尝试找直接字段名
json = doc / "friend/0/age";
if (!json) {
  std::cout << "路径操作错误" << std::endl;
}
```

当参数不是以 `/` 字符开头时，路径操作符 `/` 与索引操作符 `[]` 意义相同，即使中
间含有 `/` 字符时，也只当作单层的直接键名查找。

- **性能提示**：多级路径适于动态配置，若键名固定，就用分开的链式路径。
- **错误警示**：避免在 Json 键名中使用特殊字符。

### 取值操作

从 Json 标量叶结点提取值到 C++ 相应的基本类型的变量中，是非常常见的操作。Value
类有一系列 `get` 方法重载，支持该操作，但更建议使用操作符。

#### 带默认值的取值操作 `|`

形如 `json | defaultValue` 的操作符用法，是位或符号的重载，就可读作或，它有两
层含义：

- 返回值的类型是右侧参数；
- 如果json 结点无效，返回右侧的默认值。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "ratio": 0.618, "pass": true})";

const char* name = doc / "name" | "";
int age = doc / "age" | 0;
double ratio = doc / "ratio" | 0.0;
bool pass = doc / "pass" | false;
```

以上示例 `|` 右侧是常用基本类型的零值字面量，非常适合用于 `|` 的参数。`=` 左侧
的被赋值变量的类型不一定要与 `|` 右侧参数完全一致，只要满足 C++ 类型转换规则即
可。例如：

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "ratio": 0.618, "pass": true})";

// | 先返回 const char* ，= 再转为 std::string
std::string name = doc / "name" | "";

std::string nameDefault("unknown");
std::string name2 = doc / "name" | nameDefault;
std::string name3 = doc / "name" | std::string();

// | 先返回 int ，= 再提升为 uint_64
uint64_t age = doc / "age" | 0;
```

如果 json 结点中存储的整数真的超过 21 亿，只将 `=` 号左右的接收变量声明为
`uint_64t` 是没用右，`|` 右侧参数也得是 `uint_64t` 才行。因为 `0` 字面量是
`int` ，`json | 0` 返回 `int` ，会先被截断。

- **性能提示**：字符串结点优先用 `|""` 取值，必要时再构造 `std::string`。
- **错误警示**：`|` 有严格类型判断，须保证类型适配。

#### 使用变量原来的默认值提取 `|=`

也支持 `|=` 复合操作，`result |= json` 相当于 `result = json | result` ，表示
只有当 json 类型类型符合 `result` 这样的基本类型时才读取并赋值。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

int age = 18;
age = doc / "age" | age; // 30
age |= doc / "age";
age |= doc / "Age"; // 值不变，Age 键名有误

std::string name = "unknown";
name = doc / "name" | name; // 可能多一次字符串拷贝
name |= doc / "name";
name |= doc / "Name"; // 键名有误，无影响，完全不涉及字符串的拷贝与赋值
```

操作符 `|=` 的返回类型是左侧参数的自身引用，与 `|` 相比可以减少一次等值拷贝。
故虽然主操作对象不在左侧似乎有点别扭，但有些情况还是有用的。

#### 明确判断取值操作是否成功 `>>`

如果希望显式知道读取是否成功，可用 `json >> target` 操作，它返回 true 时会更新
target 的值。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

std::string name;
if (doc / "name" >> name) {
    std::cout << name << std::endl; // 输出：Alice
}
```

一般情况下选个合适的默认值，用 `|` 取值最方便。可以根据业务在取值后对值有效性
作判断。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

std::string name = doc / "name" | "";
if (name.empty()) {
    std::cout << "无效名字" << std::endl; //! 不会执行到这行
    return;
}
```

#### 自定义管道函数取值 `|`

取值操作符 `|` 还有个扩展的高级用法。右侧可接一个自定义函数，该函数接收一个
Value 参数，由它决定如何处理这个 json 结点，然后返回一个值。例如作自定义转换：

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

// 取值转大写
std::string name = doc / "name" | [](yyjson::Value json) {
    std::string result = json | "";
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
};

std::cout << name << std::endl; // 输出：ALICE
```

在这种用法下，`|` 可读为管道，或过滤。右侧是基本类型的情况也可以视为管道函数的
特例，只因太常用，xyjson 库内置了简化用法。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

// int age = doc / "age" | 0;
int age = doc / "age" | [](yyjson::Value json) {
    return json.getor(0);
};

std::cout << name << std::endl; // 输出：ALICE
```

一个更复杂的对象读取示例：

```cpp
struct User {
    std::string name;
    int age;
};

yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

// 使用自定义函数
User user = doc.root() | [](yyjson::Value json) -> User {
    User result;
    result.name = json / "name" | "";
    result.age = json / "age" | 0;
    return result;
};

// 等效的写法
User user2;
user2.name = doc / "name" | "";
user2.age = doc / "age" | 0;
```

仅从这个示例看，似乎用 lambda 函数还更复杂了，不过管道函数的扩展意义在于可以调
用任意已有可复用函数。

另外注意，Document 类不支持 `|` 操作，因为它的原始设计是提取叶结点的值。

#### 底层指针访问

xyjson 旨在封装 yyjson 的常用功能，并未封装所有功能，对于有些不常用的高级功能，
提供了获取底层指针的方法，允许直接使用 yyjson C API 。

`Value` 类的不带参数的 `get` 方法获取 `yyjson_val` 指针，也可以使用带一个引用
参数的 `get` 重载方法，也就支持 `|` `|=` 与 `>>` 这几个取值操作符。

```cpp
Document doc("{}");

yyjson_val* p  = doc.root().get();
yyjson_val* p1 = doc.root() | (yyjson_val*)nullptr;
yyjson_val* p2 = nullptr;
p2 |= doc.root();
yyjson_val* p3 = nullptr;
if (doc.root() >> p3) {
    // 使用 yyjson C API 直接操作
}
```

`MutableValue` 除了能获取 `yyjson_mut_val` 指针，还能获取 `yyson_mut_doc` 指针
，因为后者经常是可写 Json 操作的必要参数。

```cpp
MutDocument doc("{}");
yyjson_mut_val* p1 = nullptr;
yyjson_mut_doc* p2 = nullptr;
p1 |= doc.root();
p2 |= doc.root();
```

### Json 结点类型判断 `&`

一般而言，取值操作 `|` 及其他许多操作都自带类型感知与类型安全判断。但如果只想
判断类型或要求显式判断类型时，也有独立的方法实现。

#### 方法与操作符

Value 类提供了以下各种方法用于获取类型：
- getType: 获取表示类型的 yyjson 常量
- typeName: 获取类型的字符串表示，仅推荐在调试或打印日志时使用
- isXXX: 系列方法，如 isInt isString 等
- isType: 系列重载方法，接收一个表示类型的参数，如 isType(0), isType("") 等

然后依托 `isType` 方法实现了 `&` 操作符重载。一般能放在 `|` 操作符右侧的参数，
都可改为 `&` ，这时可读作且。`json & 0` 判断 json 有效，且与 0 具有一样类型
`int` 。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "score": [10, 20, 30]})";

bool isString = doc["name"].isString();
bool isNumber = doc["age"].isInt();
bool isArray  = doc["score"].isArray();

isString = doc / "name" & "";
isNumber = doc / "age" & 0;
isArray = doc / "score" & "[]";

// 使用类型常量（推荐）
bool isString = doc / "name" & xyjson::kString;
bool isObject = doc / "config" & xyjson::kObject;

// 具名类型判断方法
if ((doc / "age").isNumber()) {
    // 是数字类型
}

// 获取类型名称
std::string typeName = (doc / "name").typeName();
```

#### 类型代表值

在上一示例中看到，特殊字符字面量 `"[]"` 与 `"{}"` 用作 `&` 操作符的参数可以用
于判断数组类型与对象类型。如果不习惯使用这样的字面量魔数，xyjson 库也提供了各
类型的代表值常量（空值或零值），可以用于 `&` 的类型判断，与 `|` 的值提取。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "ratio": 0.618, "score": [10, 20, 30]})";

if(doc / "name" & kString) {
    std::cout << (doc / "name" | kString) << std::endl;
}
if(doc / "age" & kInt) {
    std::cout << (doc / "age" | kInt) << std::endl;
}
if(doc / "ratio" & kFloat) {
    std::cout << (doc / "ratio" | kReal) << std::endl;
}

if(doc / "score" & kArray) {
    // kArray 与 kObject 用于 | 参数
    std::cout << (doc / "score").toString() << std::endl;
}
```

使用 `kArray` 与 `kObject` 常量判断比用 `"[]"` 与 `"{}"` 常量判断略快些，省去
`::strcmp` 比较调用。

#### 数字类型细分

按 Json 标准的类型划分，Number 是不分整数与浮点数。xyjson(yyjson) 库既支持强类
型区分整数与浮点数，也支持通用数字类型，常量 `kNumber` 可用于类型判断 `&`与
值提取 `|` 操作符。沿上例：

```cpp
// 整数与浮点数都属于 Number
bool isNumber = doc / "age" & kNumber; // 结果：true
isNumber = doc / "ration" & kNmuber;   // 结果：true
auto age = doc / "age" | kNumber;      // 结果: 30.0
```

注意 `| kNumber` 返回类型是 `double` 。在 C++ 中，除非特殊场景，尽量区分整数与
浮点数；在 xyjson 中就该区分使用 `kInt(0)` 与 `kReal(0.0)` 。

事实上，整数在 yyjson 也进一步分为无符号（正整数）与有符号（负整数），分别用
`uint64_t` 与 `int64_t` 存储，api 命名上分别以 `uint` 与 `sint` 表示。在
xyjson 定义的常量符号分别是 `kInt`, `kUint` 与 `kSint` 。

尤其要注意的是，正整数与负整数的分类是互斥的，一个 json 数字当然不会既是正整数
又是负整数。四字节范围内的正整数与负整数都可用 `int` 存储，所以大部分情况下使
用 `int` 就可以了。一个容易困惑的是小正整数，它只属于 `int` 与 `uint64_t` ，但
不属于 `int64_t`。

```cpp
yyjson::Document doc;
doc << R"({"uint": "1", "sint": -1, "int": 100})";

bool check;
check = doc / "uint" & int(0);  // 结果: true
check = doc / "uint" & uint64_t(0);  // 结果: true
check = doc / "uint" &  int64_t(0);  // 结果: false

check = doc / "sint" & int(0);  // 结果: true
check = doc / "sint" & uint64_t(0);  // 结果: false
check = doc / "sint" &  int64_t(0);  // 结果: true

// 用类型常量判断可能更直观些
check = doc / "uint" & kInt;   // 结果: true
check = doc / "uint" & kUint;  // 结果: true
check = doc / "uint" & kSint;  // 结果: false

check = doc / "sint" & kInt;   // 结果: true
check = doc / "sint" & kUint;  // 结果: false
check = doc / "sint" & kSint;  // 结果: true

// 取值失败，"int" 结点不是负整数
int64_t value = 0;
value |= doc / "int";
std::cout << value << std::endl; // 输出：0

// 取值成功，字面量 `0` 属于 `int` 类型，与 json 结点匹配
std::cout << (doc / "int" | 0) << std::endl; // 输出：100
```

- **性能提示**：尽量使用最常用的 `int` ，不要使用太通用的 Number 。
- **错误警示**：在使用 64 位大整数时，最好先判断实际的整数类型。

#### 字符串类型

从 Json 字符串结点中读取后能存入到两种常用的 C++ 字符串类型中：
- C 风格字符串 `const char*`，只保存指针仍指向原 Json 的字符串
- 标准库字符串 `std::string`，拷贝字符串到独立副本

这两种类型的变量都可以放到 `|` 或 `&` 的后面，用于读取（默认值）或类型判断。
由于 `std::string` 可以从 `const char*` 构造或赋值，所以读取字符串最简单的写法
就是 `| ""` ，再由 `=` 左侧的变量决定最终使用什么字符串类型。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

const char* pszName = doc / "name" | "";
std::string strName = doc / "name" | ""; // 或 | std::string()

if (doc / "name" & kString)
{
    pszName = doc / "name" | kString;
}
```

此外，还有个特殊类型 `EmptyString` 及其常量 `kString` ，只能用于 `|` 与 `&` 等
重载操作符后面作为一种类型标识，而作为普通变量是没有意义的。这个标识类型的提出，
是由于字面 `""` 与 `"{}"` ，`"[]"` 在用于 `&` 的参数时，调用的是同一个方法，接
收的参数类型是 `const char*` ，这就需要对后面两种特殊字面作额外的比较判断是否
对象或数组。

#### 特殊 Json 类型 Null

xyjson 也提供了 `kNull` 常量（其实也就是 `nullptr`）与 `isNull` 方法来判断一个
`Value` 是否 Null 结点。但要注意，Null 结点也是有效结点，无效结点却不是 Null
结点，也不是任何其他类型的结点。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "sex": null})";

auto sex = doc / "sex";
if (sex) {
    bool tf1 = sex.isNull(); // 结果：true
    bool tf2 = sex & kNull;  // 结果：true
}

// 查找错误字段，无效 Value
sex = doc / "Sex";
if (!sex) {
    bool tf1 = sex.isNull(); // 结果：false
    bool tf2 = sex & kNull;  // 结果：false
}
```

- **错误警示**：不要用 `isNull()` 判断 `Value` 无效，直接使用 `!` 布尔上下文。

### 比较操作

#### 等值比较 `==`

Document 类与 Value 类都重载了 `==` 及 `!=` 操作符，执行两棵 Json 树的深度比较：

```cpp
std::string jsonText = R"({"name": "Alice", "age": 30})";
yyjson::Document doc1(jsonText);
yyjson::Document doc2(jsonText);

if (doc1 == doc2) {
    std::cout << "相等" << std::endl;
}

if ((doc1 / "name") == (doc2 / "name")) {
    std::cout << "相等" << std::endl;
}
```

Json 标量结点也可以直接与基本类型比较：

```cpp
std::string jsonText = R"({"name": "Alice", "age": 30})";
yyjson::Document doc1(jsonText);

if ((doc1 / "name") == "Alice") {
    std::cout << "相等" << std::endl;
}

if ((doc1 / "age") == 30) {
    std::cout << "相等" << std::endl;
}
```

#### 有序比较 `<`

也重载了 `<` 用于比较两个同类型的 `Value` 或 `MutableValue` ，但交叉比较
`Value` 与 `Mutable` 的大小（或相等）没有定义。有序比较不如等值比较常用，但在
有需要时 `<` 定义了确定性的排序，其规则大致是：

- 先按 json 类型排序：null 最小，object 最大
- 标量类型，主要是数字与字符串，按其内容或值比较
- 容器类型，数组与对象，先按容器大小比较，再按内部指针比较（避免递归性能问题）

由 `<` 也衍生出其他几个比较操作 `>` , `<=` 与 `>=` 。在实际业务项目中，若先确
知两个 json 的类型相同，可选使用简化操作。

但是 `<` 不能用于 json 与基本类型比较，因为考虑到类型不同时难以逻辑自洽。

### 类型转换的一元操作符

Json 标量类型中最常用的就是整数与字符串两种，所以 Value 类提供了两个方法将
Json 结点尽量转为整数与字符串：

- `+`: toNumber 转整数
- `-`: toString 转字符串

可类比于 `!` 将 Json 放在布尔逻辑上下文使用，`+` 与 `-` 就将 Json 分别放在
整数与字符串上下文使用。

#### JSON 转整数 `+`

相对于  `| kInt` 有严格的类型检查，操作符 `+` 或 `toInteger` 方法则有相当宽松
的规则将任意 json 结点转为一个整数 `int`：

- 整数类型，最直接，但大整数可能会被截断；
- 实数类型，取整；
- 字符串类型，按 `atoi` 转换方法将字符串前缀转整数；
- 容器类型，数组或对象，取容器大小；
- 特殊类型，`null` 与 `false` 转 `0`，`ture` 转 `1`；
- 无效值，`Value` 本身无效也返回 `0`。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "ratio": "61.8", "score": [10, 20, 30]})";

int result = 0;
result = +(doc / "name");  // 结果：0
result = +(doc / "age");   // 结果：30
result = +(doc / "ratio"); // 结果：61
result = +(doc / "score"); // 结果：3

result = (doc / "score").toInteger(); // 结果：3
```

`toInteger` 的意义在于有些 json 数字可能会被有意或无意地加上引号当成字符串传输
（比如从前端输入框收集的数据），这样的话 `| 0` 就会因类型不匹配取值失败。选择
`+` 正号作为强转整数的符号是因为将其整数前仍保持原义。

还有个相关的方法 `toNumber` 方法，它只能将各种数字类型的 Json 值统一转为
`double` ，其他类型如字符串、数组、对象都无法转换，返回零值 `0.0` 。该方法与
操作符 `| kNumber` 效果相同。

#### JSON 转字符串 `-`

Value 类另有一个转字符串的方法 `toString` ，返回 json 的字符串表示，在大多情况
下与 write 的输出是相同的，只有当 json 是字符串类型时，`toString` 默认返回的是
不带序列化引号包围的纯值。在传入可选参数 `true` 时，`toString` 返回格式化的
Json 串，也与 `write` 无格式序列化不一样。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

std::string result; // toString 的返回类型
result = -(doc / "name");  // 结果：name
result = -(doc / "age");   // 结果：30
result = -doc.root();      // 结果：{"name":"Alice","age":30}

result = (doc / "name").toString(true); // 结果："name"
result = doc.root().toString(true);
std::cout << result << std::endl;
```

最后一条语句的结果是：
```
{
    "name": "Alice",
    "age": 30
}
```

#### Document 只读可写互转 `~`

操作符 `~` 可以将 `Document` 转为 `MutableDocument` 或反向转换，相应方法名是
`mutate` 和 `freeze` 。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

yyjson::MutableDocument mutDoc = ~doc;
auto doc2 = ~mutDoc;
bool result = (doc2 == doc); // 结果：true
```

只读的 Document 效率更高，一般的建议是先将输入 Json 解析为 `Document`，在有需
要修改时再转为 `MutableDocument` 。

#### 字符串字面量直接转 Document `_xyjson`

字面量运算符不是传统的操作符，它是在 C++11 开始引入的，通过在字面量加后缀来自
定义某种操作。在 `yyjson::literals` 命名空间定义了 `_xyjson` 操作符，可以从字
面量构造 `Document` 类对象。其用法如下：

```cpp
using namespace yyjson::literals;
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
auto name = doc / "name" | "";
auto age = doc / "age" | 0;

std::cout << name << "=" << age << std::endl; // 输出：Alice=30
```

从之前的示例可以看到，通过操作符重载，在代码中除了需要用法 `Document` 类构造外，
就几乎不必显式用到其他类与方法名。现在再通过 `_xyjson` 后缀操作符，连起点的
`Document` 类名都可以不出现在代码中了。当然了，字面量操作符可能只在测试中更常
见且简洁，而实际的项目中很少出现固定字面量的 json 串。

## 可写 JSON 模型操作

可写 JSON 模型（MutableDocument 和 MutableValue）也支持上一节的所有操作，
本节着重讲解它额外支持的动态修改和添加 Json 数据结点的功能。

### 创建与初始化可写文档

创建可写文档时，可以指定初始 JSON 内容，或者从空对象/数组开始：

```cpp
// 创建空对象文档
yyjson::MutableDocument mutDoc("{}");

// 创建带初始内容的文档
yyjson::MutableDocument mutDoc2(R"({"name": "Alice", "age": 30})");

// 创建空数组文档
yyjson::MutableDocument mutDoc3("[]");
```

这与只读的 Document 创建基本是一样的，只有一点区别，无参的默认构造函数也会创建
一个空对象的根结点，与传入 `"{}"` 参数是一样的。这是为了方便用户有个直接可以开
始写的根结点对象。虽然 yyjson 支持在之后重新设定根结点，但一般不建议这样做。

### 赋值操作修改已有结点 `=`

`MutableValue` 支持操作符 `=` 或 `set` 方法修改已有 Json 结点的值。

```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"({"name": "Alice", "age": 30})";

mutDoc / "age" = 18;
mutDoc / "name" = "bob";
std::string strName = mutDoc / "name" | ""; // 结果：bob
strName[0] = 'B';
mutDoc / "name" = strName;
std::cout << mutDoc / "name" << std::endl;  // 输出：Bob
std::cout << mutDoc << std::endl;  // 输出：{"name":"Bob","age":18}
```

#### 修改整数类型

前文提到，yyjson 在解析只读文档时，只会将负整数保存为 `int64_t`，非负整数保存为
`uint64_t`。但在可写文档中，可以显式修改设定整数类型：

```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"({"name": "Alice", "age": 30})";

auto age = mutDoc / "age";
if(age & 0);           // kInt:  true
if(age & uint64_t(0)); // kUint: true
if(age & int64_t(0));  // kSint: false

// 显式将 age 修改为使用 int64_t 存储整数
age = int64_t(3000);
if(age & 0);           // kInt:  true
if(age & uint64_t(0)); // kUint: false
if(age & int64_t(0));  // kSint: true

// 重新序列化与反序列化
std::string json = mutDoc.toString();
yyjson::Document doc(json);
if (doc / "age" & int64_t())); // kSint: false
```

然而要注意的是，将 json 序列化后，这种细节类型信息就丢失了。重新读入后，非负整
数仍然会保存为 `uint64_t`，而不是 `int64_t` 。

#### 使用类型常量赋值

那些可用于 `&` 作类型判断的常量符号，也可用于 `=` 赋值，将目标结点改为各类型的
默认值，也就是零值或空值。

```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"({"name":"Alice", "age":"30", "sex":"null", "score":100, "friend":"Bob"})";

mutDoc / "name" = "";     // kString
mutDoc / "age" = 0;       // kInt
mutDoc / "sex" = nullptr; // kNull
mutDoc / "score" = "{}";  // kObject
mutDoc / "friend" = "[]"; // kArray

std::cout << mutDoc << std::endl;
// 输出：{"name":"","age":0,"sex":null,"score":{},"friend":[]}
```

可以修改 Json 标量结点的类型，但根据 yyjson 的建议，不要为非空数组或对象重新赋
值为标量，这会导致容器内大量子结点变得不可访问，很可能是非预期的。但是将标量先
改为空数组或空对象，一般是安全且有用的，以备后面扩展 Json 结构。

#### Value 类型本身的赋值

请注意 `=` 操作符还承担了标准的类对象赋值拷贝功能。`Value` 与 `MutableValue`
是值类型的类，支持拷贝与同类赋值（Document 不可拷贝只能移动）。同类赋值只是拷
贝底层 Json 结点的指针引用，赋值为基本类型则会修改改所引用结点的存储值，所以只
支持 `MutableValue` 不支持 `Value` 。

```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"({"name": "Alice", "age": 30})";

auto age = mutDoc / "age";
auto age2 = age;
age = 20;

// 以下三行都输出：20
std::cout << age << std::endl;
std::cout << age2 << std::endl;
std::cout << mutDoc / "age" << std::endl;
```

### 索引操作自动插入结点 `[]`

在可写模型中，操作符 `[]` 和 `/` 有不同的语义和行为：

- **`[]` 操作符**：支持自动插入新字段，会检查键是否存在，不存在时自动创建
- **`/` 操作符**：只能访问已存在的路径，如果路径不存在会返回无效值

```cpp
yyjson::MutableDocument mutDoc;

mutDoc["name"] = "Alice";     // 自动创建并设置 name 字段
mutDoc["age"] = 30;           // 自动创建并设置 age 字段

mutDoc / "name" = "Bob";      // 修改已存在的 name 字段

//! 错误用法：尝试用 / 操作符创建新字段，没有任何效果
mutDoc / "sex" = "female"; 
if (mutDoc / "sex"); // false

// 正确做法：先用 [] 创建，再用 / 或 [] 修改
mutDoc["sex"] = "female";   // 创建
mutDoc / "sex" = "Female";  // 修改
mutDoc["sex"] = "Male";     // 修改
if (mutDoc / "sex"); // true
```

以上代码，第一次执行 `mutDoc / "sex" = "female"` 时，虽没有语法编译错误，
但没有实际效果，也不会抛异常或错误，只是完全没有影响到 `mutDoc` 。事实上，这个
时间点前后 `mutDoc / "sex"` 返回一个无效状态的 `MutableValue` ，它封装一
个空指针结点，对它不管是读值或赋值都没有效果。

而 `mutDoc["sex"]` 操作是先在 `mutDoc` 中插入一个 null 类型的 json 结点，
返回一个指向了该 null 结点的 MutableValue ，随后就可以为它赋值了，改成一个字符
串类型。然后用 `/` 操作符也能索引到该结点了。另注意即使 `[]` 在 `=` 右侧也会自
动插入不存在的结点，所以要尽量避免这个不期望的副作用。

索引操作自动增加结点的行为，仅限 Json 对象，数组是不行的。类似 `[10000]` 索引
大整数仍可能越界无效，并不会自动为数组扩容。所以 `[]` 的语义在 `MutableValue`
中比较接近标准容器 `std::map` 与 `std::vector` ，一般只建议在加字段时使用，其
他场景使用只读的路径操作符 `/` 。

`[]` 与 `/` 操作符还有优先级的不同，如果习惯使用具名方法，用 `[]` 取字段更简洁
些，而 `/` 需要额外加 `()` 改变优先级。例如：

```cpp
std::cout << mutDoc["name"].toString();
std::cout << (mutDoc / "name").toString();
//! 编绎错误，"name" 没有 toString() 方法
//! std::cout << mutDoc / "name" . toString();
```

### 输入操作增加新结点 `<<`

输入操作符 `<<` 可以向 Json 容器，数组与对象，添加新结点。这是有类型判断的智能
输入，甚至也支持向标量结点输入，即赋值。

#### 给数组添加结点

相应的具名方法是 `append` ，可以添加各种基本类型的值或其常量代表值，以及
`Value` 与 `MutableValue` 。

```cpp
MutableDocument mutDoc;

mutDoc["num1"] = "[]";
mutDoc["num1"] = kArray;

mutDoc["num1"].append(1).append(2).append(3);
mutDoc / "num2" << 1 << 2 << 3;

std::cout << mutDoc / "num1" << std::endl; // 输出：[1,2,3]
std::cout << mutDoc / "num2" << std::endl; // 输出：[1,2,3]

(mutDoc["mix"] = kArray) << false << 5.5 << 666 << "[]" << "{}" << "end";
std::cout << mutDoc / "mix" << std::endl; // 输出：[false,5.5,666,[],{},"end"]
```

可见 Json 数组内可以添加任意其他 Json 类型的值，虽然在实践项目中一般会规范要求
数组元素应该相同类型。

#### 给对象添加结点

Json 对象容器的元素是键值对，包含两个值，这与数组有点不一样。`add` 方法要求两
个参数，仍可用操作符 `<<` 但要求必须连续使用两次先后输入键与值。

```cpp
yyjson::MutableDocument mutDoc;
auto root = *mutDoc;

root.add("name", "Alice").add("age", 35);
root << "sex" << "Female" << "login" << false;

// 如果担心长链看不清键值对组合，建议一行写一对
root << "name" << "Alice";
root << "age" << "25";

std::cout << mutDoc << std::endl;
```

在该示例中，`<<` 不能直接作用于 `mutDoc` ，那将是调用 `MutableDocument` 的
`read` 方法读入并解析 Json 的，用在这里会出现编译错误。所以必须先取根结点以正
确调用 `MutableValue` 的 `input` 方法。最后输出的序列化内容是：

```json
{"name":"Alice","age":35,"sex":"Female","login":false,"name":Alice,"age":25}
```

可见在 Json 对象中 `<<` 操作符也像在数组那样在末尾添加，并且没有去重，允许重复
键。yyjson 也有 api 支持先查重再添加新键，但效率比较低，所以 xyjson 的操作符采
用不查重的 api 。Json 对象键去重的保证工作转交实际的客户代码。yyjson 反序列化
解析这种有重复键的 Json 串，也会保留重复键，而且各键还保留源串顺序；xyjson 的
`/` 与 `[]` 访问重复键时的取前面的键。至于其他 json 库遇到重复键会保留哪个键是
未定义的。

`MutableValue` 类为支持 Json 对象这种写法作了一定牺牲，额外加了一个成员表示尚
未成对的临时悬空键，当再次用 `<<` 输入一个值时，才一起将该键值对插入底层的
Json 对象中。所以输入对象时，要求 `<<` 成对出现（且奇数位只能是能作为键的字符
串类型），避免永久悬空键或键值错位。

#### 给标量重新赋值

相对于 Json 数组与对象这种可自动扩容的容器，如果把标量也视为固定容量为 1 的容
器，那么用 `<<` 输入就不是添加元素，而是改变容器中那个唯一的元素，也就是赋值。
这种情况下 `<<` 与 `=` 操作符是相同的效果。

```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"({"name": "Alice", "age": 30})";

mutDoc / "age" = 18;
mutDoc / "name" = "Bob";
std::cout << mutDoc << std::endl;  // 输出：{"name":"Bob","age":18}
```

- **性能提示**：使用 `<<` 给对象添加新字段比 `[]` 更有效率。
- **错误警示**：使用 `<<` 前判断 Json 类型结点。

#### 从头构建复杂 Json

有了 `<<` 操作符，就可以动态构建具有复杂层次结构的 Json 了。例如：

```cpp
MutableDocument mutDoc;

// 链式添加数组元素
mutDoc["numbers"] = "[]"; // mutDoc.root() << "numbers" << kArray;
mutDoc / "numbers" << 1 << 2 << 3 << 4 << 5;

// 链式设置对象属性，键、值需成对出现
mutDoc["config"] = "{}"; // mutDoc.root() << "config" << kObject;
mutDoc / "config" << "timeout" << 30 << "retries" << 3 << "debug" << true;

// 混合类型数组
mutDoc["mixed"] = "[]"; // mutDoc.root() << "mixed" << kArray;
mutDoc / "mixed" << 42 << "text" << 3.14 << false;

// 向标量结点输入，相当于赋值 `=`
mutDoc / "config" / "timeout" << 40;
```

以上代码构建的 Json 等效于：

```cpp
MutableDocument mutDoc;

mutDoc << R"({
    "numbers": [1, 2, 3, 4, 5],
    "config": {
        "timeout": 40,
        "retries": 3,
        "debug": true
    },
    "mixed": [42, "text", 3.14, false]
})";
```

初看这示例，似乎下面的写法更直观，但它实质上只是解析静态的字符串字面量，上面的
写法支持动态从各种（基本类型的）变量构造 Json 。这个示例也反映了 `<<` 操作符在
几个类上的语义相关性，对于 Document 是整体的输入，对于 `MutableValue` 是局部的
修改输入。

### 预建结点与键值对绑定 `*`

以上用 `<<` 往 Json 数组添加新结点时，其实分两步：
1. 在 `MutableDocument` 所管理的内存池中创建结点，
2. 将新结点真正添加到已有的容器结点中。

在插入对象时，第 2 步又需额外做个状态检查，如果没有悬挂键，就先创建键结点，否
则创建值结点，将键值对一起插入底层对象，然后清空悬挂键。

为了避免 `<<` 操作对象时插入键值对非原子性与悬挂键的问题，也提供了另一种操作方案。
分 3 步：
1. 先分别创建键结点与值结点，方法名 `MutableDocument::create`
2. 将键结点与值结点绑定为一个中间类型，方法名 `MuatbleValue::tag`
3. 将中间类型表示的键值对一次调用插入到对象中，方法名 `MuatbleValue::add`

这个中间类型名不关键，就叫 `KeyValue` ，只包含两个指针的轻量值类型。前两步方法
可用二元操作符 `*` 代表，第三步就是上节已介绍过的 `<<` 。由于 `*` 操作符优先级
比 `<<` 高，可以方便地写在一个表达式中。

这三步法也可稍作简化：
1. 只从基本类型创建值结点；
2. 将值结点与字符串类型表示的键名绑定；
3. 将键值对一次性插入对象中。

按此思路插入对象的示例如下：

```cpp
MutableDocument mutDoc;

MutableValue name = mutDoc * "Alice"; // mutDoc.create("Alice")
MutableValue age = mutDoc * 25;       // mutDoc.create(25)
auto kv = name * age;                 // age.tag(name)

auto root = *mutDoc;
root << kv;
std::cout << mutDoc << std::endl; // 输出：{"Alice":25}

// 这些操作可写成一行表达式，以下每一行都是等效的
root << (mutDoc * "Alice") * (mutDoc * 25); // 键结点 * 值结点
root << "Alice" * (mutDoc * 25); // 键名 * 值结点，括号是必须的
root << (mutDoc * 25) * "Alice"; // 值结点 * 键名
root << mutDoc * 25 * "Alice";   // 值结点 * 键名，省去括号
root << mutDoc.create(25).tag("Alice"); // root.add("Alice", 25)
```

看到最后，可能会觉得采用具名方法 `add` 更简洁，不过具名方法也有其他注意事项，
后文再讨论。

至于为什么选用乘号 `*` 来表达结点创建与绑定的操作，因为它与路径查找的除号 `/`
原是互逆关系，一个结点要在 Json 树中通过 `/` 找到，那么它在创建时就该用 `*` 了。
既然二元乘 `*` 是创建结点，那么一元 `*` 作用于 Document 取其根结点也相似了。

使用两个 `*` 连乘创建的键值对不能用 `<<` 添加到数组中，用一个 `*` 创建的结点理
论上可以添加到数组中，但是会重复拷贝结点，添加到数组不必舍近求远做这额外工作。
因此，也不建议将 `*` 产生的临时结果保存，更不要将绑定的键值对重复添加。

### 拷贝已有结点

操作符 `<<` 右侧参数可以也是 `Value` 或 `MutableValue`，表示从已有结点拷贝插入。

```cpp
MutableDocument mutDoc("["Alice",25]");

auto age = mutDoc / 1;
mutDoc.root << "Bob" << age; // 拷贝原结点
std::cout << mutDoc << std::endl; // 输出：["Alice",25,"Bob",25]
```

另一个更常见的情景是需要将另一个 Document 的 Json 树（或其某个子树）插入到到另
个文档中，因两个文档的内存池不同，也需要拷贝。

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

auto doc2 = ~doc;
doc2 / "name" = "Bob";
doc2 / "age" = 25;

yyjson::MutableDocument mutDoc("[]");
mutDoc.root() << doc << doc2; // 或 << doc.root() << doc2.root()
std::cout << mutDoc << std::endl;
// 输出：[{"name":"Alice","age":30},{"name":"Bob","age":25}]
```

将上个示例的两个源文档改为插入目标文档的对象中：

```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

auto doc2 = ~doc;
doc2 / "name" = "Bob";
doc2 / "age" = 25;

yyjson::MutableDocument mutDoc("{}");
// 将 doc 与 doc2 复制到 mutDoc 的内存池中
mutDoc.root() << "first" * (mutDoc * doc);
mutDoc.root() << "second" * (mutDoc * doc2);
std::cout << mutDoc << std::endl;
// 输出：{"first":{"name":"Alice","age":30},"second":{"name":"Bob","age":25}}
```

### 字符串引用

上节最后提到的 `mutDoc.create(25).tag("Alice")` 与 `mutDoc * 25 * "Alice"` 其
实只是结果一样，但实现不完全一样，改用 `tagRef("Alice")` 才完全一样。

这其中的区别在于从字符串创建 Json 结点（对象的键也是一个结点）是否需要拷贝。
`tag` 方法为安全起见，默认拷贝，`tagRef` 显式表明可安全引用。而操作符 `*` 通过
模板匹配，能自动识别字符串字面量使用引用，如果是 `std::string` 类型的变量，就
会采用拷贝。但 `tagRef` 方法的灵活性在于，如果你确认 `std::string` 的生命周期
足够长，也可以(取其 `c_str`)传给 `tagRef` 按引用方式创建结点。

用 `MutableDocument` 的 `create` 方法创建字符串类型的结点时，也遵循同样的原则，
默认拷贝，但可用 `createRef` 显式引用，而 `*` 能自动引用字符串字面量。在很多实
践项目中，Json 对象的键可能是固定，那就推荐用字符串字面量引用（`tagRef`），而
值多半是动态的，更常用拷贝（`create`）。只是在本文的示例中，为简洁起见，很多出
字符串值也使用字面量了。

```cpp
// 字符串引用示例
MutableDocument mutDoc;

// 字面量自动引用（零拷贝，推荐用于固定键名）
mutDoc["app_name"] = "MyApp";  // 自动使用引用
mutDoc["version"] = "1.0.0";    // 字面量自动引用

// 动态字符串处理
std::string dynamicKey = "dynamic_key";
std::string dynamicValue = "This is a dynamic value";

// 推荐方式：拷贝字符串（安全，适用于动态数据）
mutDoc[dynamicKey] = dynamicValue;  // 自动拷贝字符串内容

// 高级用法：显式引用（确认生命周期足够长）
std::string longLivedValue = "Long-lived value";
//! mutDoc["long_lived"] = longLivedValue.c_str();  // 显式引用
mutDoc["long_lived"].setRef(longLivedValue.c_str());  // 显式引用

// 使用 create 和 createRef 方法
MutableValue keyNode = mutDoc.createRef("fixed_key");     // 引用字面量
MutableValue valueNode = mutDoc.create(dynamicValue);     // 拷贝动态值

// 使用 * 操作符（自动识别字面量引用）
mutDoc["config"] = "{}";
mutDoc / "config" << (mutDoc * "timeout") * (mutDoc * 30);  // 自动引用

// 使用 tagRef 方法
MutableValue kvPair = mutDoc.create("25").tagRef("age");    // 键引用，值拷贝
mutDoc / "config" << kvPair;

// 验证结果
std::cout << mutDoc.toString(true) << std::endl;
```

## 迭代器使用

迭代器提供了一种高效遍历 JSON 容器（数组和对象）的方式。xyjson 提供了四种迭代
器类型：ArrayIterator、MutableArrayIterator、ObjectIterator、
MutableObjectIterator。一般不必刻意记迭代器类型，它们分别由 Value 或
MutableValue 创建，用 `auto` 接收就行。

### 迭代器创建与基本遍历

迭代器通过 `%` 操作符创建，右侧参数是整数时创建数组迭代器，字符串时创建对象
迭代器：

```cpp
// 数组迭代器 - 从索引 0 开始
for (auto iter = doc / "items" % 0; iter; ++iter) {
    std::cout << "索引: " << iter->key 
              << ", 值: " << (iter->value | "") << std::endl;
}

// 对象迭代器 - 空字符串开始表示从对象开头迭代
for (auto iter = doc / "user" % ""; iter; ++iter) {
    std::cout << "键: " << iter->key 
              << ", 值: " << (iter->value | "") << std::endl;
}
```

创建迭代器时，也可以指定一个初始位置：

```cpp
// 数组迭代器 - 表示从索引 3 开始迭代访问
for (auto iter = doc / "items" % 3; iter; ++iter) {
    std::cout << "索引: " << iter->key 
        << ", 值: " << (iter->value | "") << std::endl;
}

// 对象迭代器 - 表示从 "start_key" 键开始迭代访问
for (auto iter = doc / "user" % "start_key"; iter; ++iter) {
    std::cout << "键: " << iter->key 
        << ", 值: " << (iter->value | "") << std::endl;
}
```

简单类比一下，`doc / "items" / 0` 与 `doc / "items" % 0` 都指向同一个 Json 结
点，只不过 `%` 多一层间接性，以交互后续迭代访问的性能。`json % "key"` 也与
`json / "key"` 指向同一个结点。不过注意空路径对于两个操作符的指向结果不同，
`json % ""` 指向 `json` 的第一个子结点，而 `json / ""` 指向 `json` 本身。

### 无效迭代器与错误处理

创建迭代器类型要符合原结点本身的类型，如果错误地通过对象结点创建数组迭代器，
或通过数组结点迭代器创建对象迭代器，那将直接返回一个无效的迭代器。从空数组或空
对象创建正确类型的迭代器也是一开始就失效了。有效迭代器在到达数组末尾或对象末尾
越界后也失效。

无效迭代器在 `if` 或 `for` 的条件判断中返回 `false` ，因为各迭代器都实现了
`oprator bool` 的重载。

```cpp
// 迭代器有效性检查示例
Document doc("{\"users\": [{\"name\": \"Alice\", \"age\": 25}, {\"name\": \"Bob\", \"age\": 30}]}");

// 1. 正确创建迭代器
auto validArrayIter = doc / "users" % 0;  // 数组迭代器
if (validArrayIter) {
    std::cout << "数组迭代器有效" << std::endl;
}

// 2. 无效迭代器创建尝试
auto invalidObjectIter = doc / "users" % "";  // 错误：对数组使用对象迭代器
if (!invalidObjectIter) {
    std::cout << "对象迭代器无效（正确：数组不应使用对象迭代器）" << std::endl;
}

// 3. 空数组的迭代器
Document emptyArrayDoc("[]");
auto emptyArrayIter = emptyArrayDoc.root() % 0;
if (!emptyArrayIter) {
    std::cout << "空数组迭代器无效" << std::endl;
}

// 4. 空对象的迭代器  
Document emptyObjectDoc("{}");
auto emptyObjectIter = emptyObjectDoc.root() % "";
if (!emptyObjectIter) {
    std::cout << "空对象迭代器无效" << std::endl;
}

// 5. 迭代器边界检查
Document arrayDoc("[1, 2, 3]");
auto iter = arrayDoc.root() % 0;
for (int i = 0; iter; ++iter, ++i) {
    std::cout << "元素 " << i << ": " << (iter->value | 0) << std::endl;
}
if (!iter) {
    std::cout << "迭代器已到达数组末尾" << std::endl;
}
```

### 迭代器解引用

如果把迭代器想象为指针，那么迭代器解引用 `*` 就该获取它所指向的对象。但是对于
Json 对象迭代器有点特殊，它指向的是键值对，而不只是一个结点。所以在 xyjson 的
迭代器设计中，保存了当前键值对，一个简单内部类型。解引用 `*` 与指针 `->` 操作
符重载就指向这个内部类型。

对象迭代器当前键值对结构：
- `key`: `const char*` 类型
- `value`: Json 结点 `Value` 或 `MutableValue` 类型

为了实现的一致性，数组迭代器的解引用也是当前键值对类型，只是它的 `key` 是
`size_t` 整数类型。这使得数组迭代器在迭代中也能获当前所处的位置信息。

```cpp
// 迭代器解引用示例
Document doc("{\"users\": [{\"name\": \"Alice\", \"age\": 25}, {\"name\": \"Bob\", \"age\": 30}], \"config\": {\"timeout\": 30, \"retries\": 3}}");

// 1. 数组迭代器解引用
std::cout << "=== 数组迭代器示例 ===" << std::endl;
auto arrayIter = doc / "users" % 0;
for (; arrayIter; ++arrayIter) {
    auto& kvPair = *arrayIter;  // 解引用获取键值对
    std::cout << "索引: " << kvPair.key << ", 值类型: " << kvPair.value.typeName() << std::endl;
    
    // 访问具体字段
    std::string name = kvPair.value / "name" | "";
    int age = kvPair.value / "age" | 0;
    std::cout << "  - 用户: " << name << ", 年龄: " << age << std::endl;
}

// 2. 对象迭代器解引用
std::cout << "\n=== 对象迭代器示例 ===" << std::endl;
auto objectIter = doc / "config" % "";
for (; objectIter; ++objectIter) {
    auto& kvPair = *objectIter;  // 解引用获取键值对
    std::cout << "键: " << kvPair.key << ", 值类型: " << kvPair.value.typeName();
    
    // 根据类型处理值
    if (kvPair.value.isNumber()) {
        int value = kvPair.value | 0;
        std::cout << ", 值: " << value << std::endl;
    } else {
        std::string value = kvPair.value | "";
        std::cout << ", 值: " << value << std::endl;
    }
}

// 3. 指针操作符 -> 使用
std::cout << "\n=== 指针操作符示例 ===" << std::endl;
auto iter = doc / "config" % "";
if (iter) {
    // 使用 -> 操作符访问成员
    std::cout << "第一个配置项 - 键: " << iter->key << ", 值: " << (iter->value | "") << std::endl;
    
    // 移动到下一个
    ++iter;
    if (iter) {
        std::cout << "第二个配置项 - 键: " << iter->key << ", 值: " << (iter->value | "") << std::endl;
    }
}
```

### 迭代器移动操作

迭代器支持多种移动方式，包括顺序移动和随机访问：

```cpp
// 创建迭代器
auto iter = doc / "items" % 0;

// 顺序移动
++iter;      // 前进到下一个元素
--iter;      // 后退到上一个元素（如果支持）

// 随机访问
iter += 3;   // 向前移动 3 个元素
iter -= 2;   // 向后移动 2 个元素
```

### 迭代器的重定位

我们已知迭代器是由 Json 结点类用 `%` 操作符创建的，而迭代器本身也支持 `%` ，它
也创建一个重新定位的新迭代器。同时支持 `%=` 复合操作符，它不会创建新迭代器，而
是自身重定位。

```cpp
// 迭代器重定位示例
Document doc("{\"items\": [10, 20, 30, 40, 50], \"config\": {\"a\": 1, \"b\": 2, \"c\": 3, \"d\": 4, \"e\": 5}}");

// 1. 迭代器 % 操作符重定位
std::cout << "=== 迭代器重定位示例 ===" << std::endl;

// 数组迭代器重定位
auto arrayIter = doc / "items" % 0;
std::cout << "初始位置: 索引 " << arrayIter->key << ", 值: " << (arrayIter->value | 0) << std::endl;

// 重定位到索引 2
arrayIter = arrayIter % 2;
if (arrayIter) {
    std::cout << "重定位到索引 2: 值: " << (arrayIter->value | 0) << std::endl;
}

// 重定位到末尾之后（无效）
arrayIter = arrayIter % 10;
if (!arrayIter) {
    std::cout << "重定位到索引 10: 迭代器无效" << std::endl;
}

// 2. 对象迭代器重定位
auto objectIter = doc / "config" % "";
std::cout << "\n=== 对象迭代器重定位 ===" << std::endl;

// 重定位到键 "c"
objectIter = objectIter % "c";
if (objectIter) {
    std::cout << "重定位到键 'c': 值: " << (objectIter->value | 0) << std::endl;
}

// 重定位到不存在的键（无效）
objectIter = objectIter % "nonexistent";
if (!objectIter) {
    std::cout << "重定位到不存在的键: 迭代器无效" << std::endl;
}

// 3. %= 复合操作符示例
std::cout << "\n=== 复合操作符 %= 示例 ===" << std::endl;

// 数组迭代器
arrayIter = doc / "items" % 0;
std::cout << "初始位置: " << (arrayIter->value | 0) << std::endl;

// 使用 %= 重定位到索引 3
arrayIter %= 3;
if (arrayIter) {
    std::cout << "使用 %= 重定位到索引 3: " << (arrayIter->value | 0) << std::endl;
}

// 对象迭代器
objectIter = doc / "config" % "";
std::cout << "初始键: " << objectIter->key << std::endl;

// 使用 %= 重定位到键 "d"
objectIter %= "d";
if (objectIter) {
    std::cout << "使用 %= 重定位到键 'd': 值: " << (objectIter->value | 0) << std::endl;
}

// 4. 可写迭代器重定位
std::cout << "\n=== 可写迭代器重定位示例 ===" << std::endl;
MutableDocument mutDoc("{\"data\": [100, 200, 300, 400, 500]}");

auto mutIter = mutDoc / "data" % 0;
std::cout << "可写迭代器初始位置: " << (mutIter->value | 0) << std::endl;

// 重定位并修改
mutIter %= 2;
if (mutIter) {
    mutIter->value = 999;
    std::cout << "重定位到索引 2 并修改为: " << (mutIter->value | 0) << std::endl;
}

std::cout << "修改后的完整文档: " << mutDoc.toString() << std::endl;
```

### 可写迭代器的修改能力

MutableArrayIterator 和 MutableObjectIterator 支持修改当前元素的值：

```cpp
// 创建可写迭代器
for (auto iter = mutDoc / "items" % 0; iter; ++iter) {
    // 修改当前元素的值
    iter->value = "modified value";
}
```

## 常见错误与陷阱

在使用 xyjson 过程中，一些常见的错误模式需要特别注意。

### Document 类拷贝限制

Document 类设计为不可拷贝，只能移动或转换：

```cpp
yyjson::Document doc1("{\"data\": \"value\"}");

//! 错误：尝试拷贝 Document
//! yyjson::Document doc2 = doc1;  // 编译错误

// 正确做法：使用移动语义
yyjson::Document doc2 = std::move(doc1);

// 正确做法：使用模式转换
yyjson::MutableDocument mutCopy = ~doc2;
```

### 路径操作符语义误解

`/` 和 `[]` 操作符有不同的创建行为：

```cpp
yyjson::MutableDocument mutDoc("{}");

//! 错误：用 / 操作符创建新字段
//! mutDoc / "new_field" = "value";

// 正确：用 [] 操作符创建新字段
mutDoc["new_field"] = "value";
mutDoc / "new_field" = "updated";  // 修改已存在字段
```

### 字符串字面量与 const char* 不同

xyjson 的操作符能识别字符串字面量进行优化，但是 `const char*` 类型的字符串不行。
尤其要注意 `const char[N]` 数组在函数传参时常会退化为 `const char*` ，以及
`std::string::c_str()` 方法返回的 `const char*` 不能被自动优化。但可以用具名
方法显式表明引用。

```cpp
MutableDocument mutDoc;

mutDoc["key"] = "value"; // 引用
mutDoc["key"] = std::string("value"); // 拷贝

std::string strValue = "long string value";
mutDoc["key"] = strValue.c_str(); // 拷贝
mutDoc["key"].setRef(strValue.c_str()); // 引用
```

### 管道函数链式操作限制

管道函数不支持链式操作：

```cpp
//! 错误：尝试链式管道操作
//! auto result = value | f1 | f2 | f3;  // 编译错误

// 正确：在一个 lambda 中完成所有处理
auto result = value | [](auto v) {
    return f3(f2(f1(v)));
};
```

其实如果管道函数类型也是 Value 或 MutableValue ，那就可以继续管道。如果返回其
他类型，能否继续接管道取决于返回类型（及后面类型）的实现。

## 小结

### 性能优化建议

- **中间结点保存**：对于频繁访问的字段，保存变量，避免重复路径找
- **字符串优化**：优先使用字符串字面量以获得引用优化
- **使用迭代器**：在适合迭代器的场景就不要循环路径操作
