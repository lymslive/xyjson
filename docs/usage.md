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

### Document 整体的输入输出

#### 输入操作

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

#### 输出操作

输出是输入的逆操作，Document 提供 wirte 方法及 `>>` 操作符将整个 Json 树序列化
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

#### 索引操作 []

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

#### 路径操作 /

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

#### 带默认值的取值操作

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
- **错误警示**：`|` 有严格类型判断，须保存类型适配。

#### 使用变量原来的默认值提取

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

#### 明确判断取值操作是否成功

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

#### 自定义管道函数取值

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

### 类型判断

一般而言，取值操作符 `|` 自带类型感知与类型安全判断，但如果只想判断安全或要求
显式判断类型时，可用 `&` 操作符替换 `|` 操作符，这时可读作且。`json & 0` 判断
json 有效，且与 0 具有一样类型 `int` 。当然也有一系列的 `isXXX` 方法名可用。

```cpp
// 类型判断操作符
bool isString = doc / "name" & "";
bool isNumber = doc / "age" & 0;
bool isObject = doc / "user" & "{}";
bool isArray = doc / "items" & "[]";

// 使用类型常量（推荐）
bool isString = doc / "name" & xyjson::kStr;
bool isObject = doc / "config" & xyjson::kObject;

// 具名类型判断方法
if ((doc / "age").isNumber()) {
    // 是数字类型
}

// 获取类型名称
std::string typeName = (doc / "name").typeName();
```

注意：Json 树上的 `null` 结点也是语法上存在的结点，若 Value 引用这样的 `null`
结点，它也认为是有效的，在 bool 上下文返回 `true` 。可用 `isNull` 方法显式判断
是否 `null` 结点。而无效引用的 Value 用 `isNull` 时返回 `false`。应把 Json 的
`null` 视为与整数、字符串并列的类型，即使它特殊只有一个值，便也不算太特殊，
bool 类型只有两个值。

### 比较操作

Document 类与 Value 类都重载了 `==` 及 `!=` 操作符，执行两棵 Json 树的深度比较：

```cpp
// 深度比较两个文档
if (doc1 == doc2) {
    // 文档内容完全相同
}

// 比较两个 JSON 值，也可能是子树
if ((doc1 / "version") == (doc2 / "version")) {
    // 版本字段相同
}
```

但是 Json 标量结点不能直接与基本类型比较，需要先提取值再比较：

```cpp
//! 错误示例：JSON 值不能直接与基本类型比较
//! if (doc / "version" == "1.0") {  // 编译错误
//! }

// 正确做法：先提取值再比较
std::string version = doc / "version" | "";
if (version == "1.0") {
    // 版本匹配
}

// 可能错误：| "" 返回 const char* 作指针比较
if ((doc / "version" | "") == "1.0") {
    // 版本匹配
}

// 正确：| 返回 std::string 作字符串内容比较
if ((doc / "version" | std::string()) == "1.0") {
    // 版本匹配
}
```

特别注意提取 Json 字符串结点时，返回的具体类型取决于 `|` 右侧的参数，避免
`const char*` 的指针比较，那很可能不是想要的结果。而其他情况下使用 `| ""` 可能
是最高效方便的，先返回 `const char*` 避免拷贝，然后也可按需赋给 `std::string`
创建 C++ 字符串对象。

### 类型转换的一元操作符

Json 标量类型中最常用的就是整数与字符串两种，所以 Value 类提供了两个方法将
Json 结点尽量转为整数与字符串：

- `+`: toNumber 转整数
- `-`: toString 转字符串

可类比于 `!` 将 Json 放在布尔逻辑上下文使用，`+` 与 `-` 就将 Json 分别放在
整数与字符串上下文使用。

#### JSON 转整数
一元操作符 `+` 和 `-` 提供了简洁的类型转换语法：

```cpp
// + 操作符：转换为数字
int size = +(doc / "items");        // 数组/对象返回元素个数
int number = +(doc / "value");      // 数字返回原值，字符串尝试转换

// - 操作符：转换为字符串  
std::string str = -(doc / "name");  // 返回字符串表示

// * 操作符：获取根结点
yyjson::Value root = *doc;

// ~ 操作符：模式转换
yyjson::MutableDocument mutableCopy = ~doc;  // 只读转可写
yyjson::Document readonlyCopy = ~mutDoc;     // 可写转只读
```

#### JSON 转字符串

Value 类另有一个转字符串的方法 `toString` ，返回 json 的字符串表示，在大多情况
下与 write 的输出是相同的，只有当 json 是字符串类型时，`toString` 默认返回的是
不带序列化引号包围的纯值。在传入可选参数 `true` 时，`toString` 返回格式化的
Json 串，也与 `write` 无格式序列化不一样。

```cpp
// 转字符串
std::string jsonString = doc.root().toString();

// 输出至字符串
doc >> jsonString;

// 流输出
std::cout << doc << std::endl;

// 真序列化输出，支持仅输出一部分子树
std::cout << doc["key"].toString(true) << std::endl;

// 写入文件
doc.writeFile("output.json");
```

#### Document 只读可写互转

### 管道函数自定义处理

管道函数允许对 JSON 值进行自定义转换处理：

```cpp
// 简单的字符串处理
std::string uppercase = doc / "name" | [](const yyjson::Value& v) {
    std::string result = v | "";
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
};

// 复杂对象转换
struct UserProfile {
    std::string name;
    int age;
    std::vector<std::string> skills;
};

UserProfile profile = doc / "user" | [](const yyjson::Value& v) -> UserProfile {
    UserProfile result;
    result.name = v / "name" | "";
    result.age = v / "age" | 0;
    
    // 处理技能数组
    auto skills = v / "skills";
    if (skills) {
        for (auto iter = skills % 0; iter; ++iter) {
            result.skills.push_back(iter->value | "");
        }
    }
    
    return result;
};

```

### 底层指针访问

对于需要直接使用 yyjson C API 的高级场景，可以获取底层指针：

```cpp
// 获取只读结点指针
yyjson_val* nodePtr;
if (doc / "data" >> nodePtr) {
    // 使用 yyjson C API 直接操作
    yyjson_type type = yyjson_get_type(nodePtr);
}

// 获取可写文档指针
yyjson_mut_doc* docPtr;
if (mutDoc >> docPtr) {
    // 使用完整的 yyjson 可写 API
}
```
## 可写 JSON 模型操作

可写 JSON 模型（MutableDocument 和 MutableValue）提供了创建和修改 JSON 数据的能力。与只读模型相比，可写模型在内存布局上采用环形链表结构，支持动态添加和修改结点。

### 创建与初始化可写文档

创建可写文档时，可以指定初始 JSON 内容，或者从空对象/数组开始：

```cpp
// 创建空对象文档
yyjson::MutableDocument mutDoc("{}");

// 创建带初始内容的文档
yyjson::MutableDocument mutDoc2("{\"title\": \"Configuration\"}");

// 创建空数组文档
yyjson::MutableDocument mutDoc3("[]");
```

这与只读的 Document 创建基本是一样的，只有一点区别，无参的默认构造函数也会创建
一个空对象的根结点，与传入 `"{}"` 参数是一样的。这是为了方便用户有个直接可以开
始写的根结点对象。虽然 yyjson 支持在之后重新设定根结点，但一般不建议这样做。

### 字段操作的正确方法

在可写模型中，操作符 `[]` 和 `/` 有不同的语义和行为：

- **`[]` 操作符**：支持自动插入新字段，会检查键是否存在，不存在时自动创建
- **`/` 操作符**：只能访问已存在的路径，如果路径不存在会返回无效值

```cpp
// 使用 [] 操作符创建新字段（推荐）
mutDoc["name"] = "Alice";    // 自动创建并设置 name 字段
mutDoc["age"] = 30;           // 自动创建并设置 age 字段

// 使用 / 操作符修改已存在的字段
mutDoc / "name" = "Bob";      // 修改已存在的 name 字段

//! 错误示例：尝试用 / 操作符创建新字段，没有任何效果
mutDoc / "new_field" = "value"; 

// 正确做法：先用 [] 创建，再用 / 修改
mutDoc["new_field"] = "initial value";   // 创建
mutDoc / "new_field" = "updated value";  // 修改
```

以上代码，第一次执行 `mutDoc / "new_field" = "value"` 时，虽没有语法编译错误，
但没有实际效果，也不会抛异常或错误，只是完全没有影响到 `mutDoc` 。事实上，这个
时间点前后 `mutDoc / "new_field"` 返回一个无效状态的 `MutableValue` ，它封装一
个空指针结点，对它不管是读值或赋值都没有效果。

而 `mutDoc["new_field"]` 操作是先在 `mutDoc` 中插入一个 null 类型的 json 结点，
返回一个指向了该 null 结点的 MutableValue ，随后就可以为它赋值了，改成一个字符
串类型。然后用 `/` 操作符也能索引到该结点了。另注意即使 `[]` 在 `=` 右侧也会自
动插入不存在的结点，所以要尽量避免这个不期望的副作用。

还有一个区别，`/` 与 `[]` 操作符的优先级不同，如果习惯使用具名方法，用 `[]` 取
字段更简洁些，而 `/` 需要额外加 `()` 强制优先级。例如：

```cpp
std::cout << mutDoc["name"].toString();
std::cout << (mutDoc / "name").toString();
//! 编绎错误，"name" 没有 toString() 方法
//! std::cout << mutDoc / "name" . toString();
```

### 构建复杂数据结构

可写模型支持构建嵌套的复杂数据结构，以下是构建用户配置信息的示例：

```cpp
// 创建用户信息对象
mutDoc["user"] = "{}";
mutDoc / "user" / "name" = "Alice";
mutDoc / "user" / "age" = 25;
mutDoc / "user" / "active" = true;

// 添加爱好数组
mutDoc["user"]["hobbies"] = "[]";
mutDoc / "user" / "hobbies" << "reading" << "music" << "programming";

// 添加联系信息对象
mutDoc["user"]["contact"] = "{}";
mutDoc / "user" / "contact" / "email" = "alice@example.com";
mutDoc / "user" / "contact" / "phone" = "+1234567890";

std::cout << mutDoc;
```

以上代码输出单行紧凑格式的 json 串：

```
{"user":{"name":"Alice","age":25,"active":true,"hobbies":["reading","music","programming"],"contact":{"email":"alice@example.com","phone":"+1234567890"}}}
```

### 使用 `<<` 操作符批量操作

智能输入 `<<` 操作符提供了一种高效的批量数据添加方式，特别适合数组和对象的快速构建：

```cpp
MutableDocument mutDoc;

// 批量添加数组元素
mutDoc["numbers"] = "[]";
mutDoc / "numbers" << 1 << 2 << 3 << 4 << 5;

// 批量设置对象属性，键、值需成对出现
mutDoc["config"] = "{}";
mutDoc / "config" << "timeout" << 30 << "retries" << 3 << "debug" << true;

// 混合类型添加
mutDoc["mixed"] = "[]";
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
写法支持动态从各种（基本类型的）变量构造 Json 。

Json 数组的元素允许任意其他 Json 类型，不要求同质，所以混合输入各种基本类型该
没有疑义。但是链式连续的 `<<` 如何输入对象的呢？MutableValue 类为支持这种写法
作了一定牺牲，额外加了一个成员表示尚未成对的临时悬空键，当再次用 `<<` 输入一个
值时，才一起将该键值对插入底层的 Json 对象中。所以输入对象时，要求 `<<` 成对出
现（且奇数位只能是能作为键的字符串类型），避免永久悬空键或键值错位。

相对于 Json 数组与对象这种可自动扩容的容器，如果把标量也视为固定容量为 1 的容
器，那么用 `<<` 输入就不是添加元素，而是改变容器中那个唯一的元素，也就是赋值。

### 构建多级嵌套结构

对于复杂的数据建模需求，可写模型也支持构建深层次的嵌套结构：

```cpp
MutableDocument mutDoc;

// 构建用户列表结构
mutDoc["users"] = "[]";

// 添加第一个用户
mutDoc / "users" << "{}";
mutDoc / "users" / 0 << "name" << "Alice" << "age" << 25;

// 为用户添加详细信息
mutDoc["users"][0]["profile"] = "{}";
mutDoc / "users" / 0 / "profile" / "department" = "Engineering";
mutDoc / "users" / 0 / "profile" / "level" = "Senior";

// 添加技能数组
mutDoc["users"][0]["skills"] = "[]";
mutDoc / "users" / 0 / "skills" << "C++" << "Python" << "JavaScript";

// 添加第二个用户
mutDoc / "users" << "{}";
mutDoc / "users" / 1 << "name" << "Bob" << "age" << 30;
```

### 预创建可写结点

以上用 `<<` 往 Json 容器添加新结点时，其实分两步：
1. 用 MutableDocument 在它所管理的内存池中创建结点，
2. 将新结点真正添加到已有的容器结点中。

插入操作符 `<<` 直接期望的参数是 MutableValue ，在上文示例中传入基本类型的情况
下，它会自动先调用 MutableDocument 的 create 方法创建一个 MutableValue ，也可
以用 `*` 操作符（二元乘法）。

```cpp
MutableDocument mutDoc;

MutableValue name = mutDoc * "Alice"; // mutDoc.create("Alice")
MutableValue age = mutDoc * 25;       // mutDoc.create(25)

mutDoc["akey"] = "[]";
mutDoc / "akey" << name << age;
mutDoc["okey"] = "{}";
mutDoc / "okey" << name << age;

std::cout << mutDoc;
// 输出：{"akey":["Alice",25],"okey":{"Alice":25}}
```

如果觉得 `<<` 链式输入 Json 对象与数组写法一样容易混淆，那么还可以用 `*` 将键
值对的两个 MutableValue 先绑定起来再插入对象：

```cpp
MutableDocument mutDoc;

MutableValue name = mutDoc * "Alice"; // mutDoc.create("Alice")
MutableValue age = mutDoc * 25;       // mutDoc.create(25)

mutDoc["okey"] = "{}";
mutDoc / "okey" << (name * age);
mutDoc / "okey" << name * age; // * 优化级比 << 高，括号是可选的

std::cout << mutDoc;
// 输出：{"okey":{"Alice":25},"okey":{"Alice":25}}
```

注意以上示例还说明 `<<` 插入对象也与插入数组一样，不负责检测重复键的。

如果再把 MutableValue 的创建与键值对绑定同时写在一个表达式上，就形如：

```cpp
mutDoc / "okey" << (mutDoc * "Alice") * (mutDoc * 25);
mutDoc / "okey" << "Alice" * (mutDoc * 25);
mutDoc / "okey" << (mutDoc * 25) * "Alice";
mutDoc / "okey" << mutDoc * 25 * "Alice";
mutDoc / "okey" << mutDoc.create(25).tag("Alice");
```

以上几行都是等效的写法，最后一行表明操作符本质上所调用的方法。tag 方法创建一个
类型为 KeyValue 的值，打包提供给 `<<` 操作符。

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

### 数据持久化

修改完成后，可以将可写文档保存到文件或转换为字符串：

```cpp
// 保存到文件
mutDoc.writeFile("user_data.json");

// 转换为字符串
std::string jsonString = mutDoc.toString();

// 带格式化的输出
std::string prettyJson = mutDoc.toString(true);
std::cout << "生成的 JSON: " << std::endl << prettyJson << std::endl;
```

这与只读的 Document 持久化是一样的用法。

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
