# xyjson 使用指南

本指南从库使用者的角度，详述如何利用 xyjson 库在 C++ 项目中执行常见的 JSON 操作。

## 1 概述

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

`Document` 与 `MutableDocument` 负责整个 Json 树的内存自动管理，相当于一个智能
指针。而 `Value` 与 `MutableValue` 是对某个 Json 结点的引用，相当于一种代理模
式。这些类再通过操作符重载封装了常用操作，同时提供对应的具名方法供不同编程风格
的用户选择。

注：本文的示例假设已经包含头文件 `#include "xyjson.h"` ，及引入命名空间
`using namespace yyjson`。

## 2 JSON 模型基本操作

本节讲解只读模型与可写模型都支持的基本操作， Document 一般泛指两个文档类，
Value 泛指两个 Json 结点类。

### 2.1 Document 整体的读写操作

#### 2.1.1 读入操作 `<<`

Document 类对象可以从字符串直接构造：
<!-- example:usage_2_1_1_read_json -->
```cpp
std::string jsonText = R"({"name": "Alice", "age": 30})";
yyjson::Document doc(jsonText);
```

它将解析输入的字符串参数，在它内部的内存池上构建 DOM 树模型。如入参数不是合法
的 Json 串，将解析失败，`doc` 对象将处理无效状态，可以用 `hasError` 方法判断是
否有解析错误。此外，Document 、Value 及后文介绍的迭代器类都重载了 `operator
bool` ，故也可直接在 `if` 条件中判断各类对象有效性。

<!-- example:usage_2_1_1_error_check -->
```cpp
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
<!-- example:usage_2_1_1_read_operator -->
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

<!-- example:usage_2_1_1_lazy_read -->
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

<!-- example:usage_2_1_1_read_file -->
```cpp
const char* filePath = "/tmp/input.json";
yyjson::Document doc;
doc.readFile(filePath);

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

#### 2.1.2 写出操作 `>>`

输出是输入的逆操作，Document 提供 `wirte` 方法及 `>>` 操作符将整个 Json 树序列化
输出至右侧参数目标。支持的参数与 `<<` 类似：

- 字符串，只支持 `std::string`, 而 C 风格的字符串指针不能当作安全的可写目标；
- 写模式打开的文件指针 `FILE*` ；
- 输出流文件 `std::ofstrem` ；
- 文件名参数，使用 `writeFile` 不能用 `>>` 操作符。

可写的 Document 输出操作更常用些，修改后序列化保存。

<!-- example:usage_2_1_2_write_json -->
```cpp
std::string jsonText = R"({"name": "Alice", "age": 30})";
yyjson::MutableDocument mutDoc(jsonText);

std::string strTarget;
mutDoc >> strTarget;

const char* filePath = "/tmp/output.json";
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

<!-- example:usage_2_1_2_stdout -->
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

### 2.2 Value 结点的访问

#### 2.2.1 根结点

从概念上讲，一棵 Json 树有一个根结点。Document 类也有个 `root` 方法可以返回一
个 Value 类型的根结点，或者用一元操作符 `*`。不过在 xyjson 中，很多时候不需要
感知根结点的存在，直接操作 Document 就相当于操作其根结点。

<!-- example:usage_2_2_1_root_access -->
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

#### 2.2.2 索引操作 `[]`

按索引或路径访问属于 Value 类的方法，但也可直接作用于 Document ，自动转为根结
点调用。

Value 支持用常规的 `[]` 索引访问对象或数组：
- Json 数组索引参数是整数，类似 `std::vector`
- Json 对象索引参数是字符串，类似 `std::map`

与标准容器不同的，`Value[]` 返回另一个 `Value` 值，而不是 Value 对象引用。当
Json 数组索引越界或对象值不存在时，返回一个无效的 `Value` 。若返回有效的
`Value` 值，它内部封装的指针指向真实存在的子结点。

<!-- example:usage_2_2_2_index_access -->
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

#### 2.2.3 路径操作 `/`

但更推荐使用路径操作符 `/` 平替索引操作符 `[]`，这几乎是 xyjson 库的精髓与灵感
来源。在链式路径操作中，`/` 比 `[]` 更直观方便。

<!-- example:usage_2_2_3_path_access -->
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

#### 2.2.4 JSON Pointer 标准的路径操作

路径操作符 `/` 另一强于索引操作符 `[]` 之处在于可以访问多级路径，要求遵循 JSON
Pointer 标准，必须以 `/` 开头，中间键名本身若含 `/` 或 `~` 特殊字符需要分别转
义为 `~1` 与 `~1`。当然对于实际项目，强烈建议避免键名含特殊字符，仅须注意以
`/` 开头。

<!-- example:usage_2_2_4_json_pointer -->
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

### 2.3 取值操作

从 Json 标量叶结点提取值到 C++ 相应的基本类型的变量中，是非常常见的操作。Value
类有一系列 `get` 方法重载，支持该操作，但更建议使用操作符。

#### 2.3.1 带默认值的取值操作 `|`

形如 `json | defaultValue` 的操作符用法，是位或符号的重载，就可读作或，它有两
层含义：

- 返回值的类型是右侧参数；
- 如果json 结点无效，返回右侧的默认值。

<!-- example:usage_2_3_1_extract_value -->
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

<!-- example:usage_2_3_1_convert_value -->
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

#### 2.3.2 使用变量原来的默认值提取 `|=`

也支持 `|=` 复合操作，`result |= json` 相当于 `result = json | result` ，表示
只有当 json 类型类型符合 `result` 这样的基本类型时才读取并赋值。

<!-- example:usage_2_3_2_or_assign -->
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

#### 2.3.3 明确判断取值操作是否成功 `>>`

如果希望显式知道读取是否成功，可用 `json >> target` 操作，它返回 true 时会更新
target 的值。

<!-- example:usage_2_3_3_explicit_check -->
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

<!-- example:usage_2_3_3_post_check -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

std::string name = doc / "name" | "";
if (name.empty()) {
    std::cout << "无效名字" << std::endl; //! 不会执行到这行
    return;
}
```

#### 2.3.4 自定义管道函数取值 `|`

取值操作符 `|` 还有个扩展的高级用法。右侧可接一个自定义函数，该函数接收一个
Value 参数，由它决定如何处理这个 json 结点，然后返回一个值。例如作自定义转换：

<!-- example:usage_2_3_4_pipe_function -->
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

管道函数也可以直接定义为接收基本类型的参数，如此 xyjson 会先从 json 结点中提取
出符合参数类型的基本值再传给自定义函数，如果 json 类型不匹配，默认传的是基本类
型的零值或空值。上例的等效简化写法是：

<!-- example:usage_2_3_4_pipe_function_basic -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

// 取值转大写的简化写法
std::string name = doc / "name" | [](const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
};

std::cout << name << std::endl; // 输出：ALICE
```

在这种用法下，`|` 可读为管道，或过滤。右侧是基本类型的情况也可以视为管道函数的
特例，只因太常用，xyjson 库内置了简化用法。

<!-- example:usage_2_3_4_pipe_function_int -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

// int age = doc / "age" | 0;
int age = doc / "age" | [](yyjson::Value json) {
    return json.getor(0);
};

std::cout << age << std::endl; // 输出：30
```

一个更复杂的对象读取示例：

<!-- example:usage_2_3_4_complex_object -->
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

#### 2.3.5 底层指针访问

xyjson 旨在封装 yyjson 的常用功能，并未封装所有功能，对于有些不常用的高级功能，
提供了获取底层指针的方法，允许直接使用 yyjson C API 。

`Value` 类的不带参数的 `get` 方法获取 `yyjson_val` 指针，也可以使用带一个引用
参数的 `get` 重载方法，也就支持 `|` `|=` 与 `>>` 这几个取值操作符。

<!-- example:usage_2_3_5_underlying_ptr -->
```cpp
yyjson::Document doc("{}");

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

<!-- example:usage_2_3_5_underlying_mutptr -->
```cpp
yyjson::MutableDocument doc("{}");
yyjson_mut_val* p1 = nullptr;
yyjson_mut_doc* p2 = nullptr;
p1 |= doc.root();
p2 |= doc.root();
```

### 2.4 Json 结点类型判断 `&`

一般而言，取值操作 `|` 及其他许多操作都自带类型感知与类型安全判断。但如果只想
判断类型或要求显式判断类型时，也有独立的方法实现。

#### 2.4.1 方法与操作符

Value 类提供了以下各种方法用于获取类型：
- getType: 获取表示类型的 yyjson 常量
- typeName: 获取类型的字符串表示，仅推荐在调试或打印日志时使用
- isXXX: 系列方法，如 isInt isString 等
- isType: 系列重载方法，接收一个表示类型的参数，如 isType(0), isType("") 等

然后依托 `isType` 方法实现了 `&` 操作符重载。一般能放在 `|` 操作符右侧的参数，
都可改为 `&` ，这时可读作且。`json & 0` 判断 json 有效，且与 0 具有一样类型
`int` 。

<!-- example:usage_2_4_1_type_check -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "score": [10, 20, 30]})";

bool isString = doc["name"].isString();
bool isNumber = doc["age"].isInt();
bool isArray  = doc["score"].isArray();
bool isObject = doc["config"].isObject();


isString = doc / "name" & "";
isNumber = doc / "age" & 0;
isArray = doc / "score" & "[]";


// 使用类型常量（推荐）
isString = doc / "name" & kString;
isObject = doc / "config" & kObject;

// 具名类型判断方法
if ((doc / "age").isNumber()) {
    // 是数字类型
}

// 获取类型名称
std::string typeName = (doc / "name").typeName();
```

#### 2.4.2 类型代表值

在上一示例中看到，特殊字符字面量 `"[]"` 与 `"{}"` 用作 `&` 操作符的参数可以用
于判断数组类型与对象类型。如果不习惯使用这样的字面量魔数，xyjson 库也提供了各
类型的代表值常量（空值或零值），可以用于 `&` 的类型判断，与 `|` 的值提取。

<!-- example:usage_2_4_2_type_represent -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30, "ratio": 0.618, "score": [10, 20, 30]})";

if (doc / "name" & kString) {
    std::cout << (doc / "name" | kString) << std::endl; // 输出：Alice
}
if (doc / "age" & kInt) {
    std::cout << (doc / "age" | kInt) << std::endl; // 输出：30
}
if (doc / "ratio" & kReal) {
    std::cout << (doc / "ratio" | kReal) << std::endl; // 输出：0.618
}

if (doc / "score" & kArray) {
    // kArray 与 kObject 也可用于 | 参数，返回表示数组/对象的特殊子类
    std::cout << (doc / "score" | kArray).toString() << std::endl;
    //^ 输出：[10,20,30]
}
```

使用 `kArray` 与 `kObject` 常量判断比用 `"[]"` 与 `"{}"` 常量判断略快些，省去
`::strcmp` 比较调用。

#### 2.4.3 数字类型细分

按 Json 标准的类型划分，Number 是不分整数与浮点数。xyjson(yyjson) 库既支持强类
型区分整数与浮点数，也支持通用数字类型，常量 `kNumber` 可用于类型判断 `&`与
值提取 `|` 操作符。沿上例：

<!-- example:usage_2_4_3_number_type -->
```cpp
yyjson::Document doc;
doc << R"({"age": 30, "ratio": 0.618})";

// 整数与浮点数都属于 Number
bool isNumber = doc / "age" & kNumber; // 结果：true
isNumber = doc / "ratio" & kNumber;    // 结果：true
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

<!-- example:usage_2_4_3_integer_types -->
```cpp
yyjson::Document doc;
doc << R"({"uint": 1, "sint": -1, "int": 100})";

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

#### 2.4.4 字符串类型

从 Json 字符串结点中读取后能存入到两种常用的 C++ 字符串类型中：
- C 风格字符串 `const char\*`，只保存指针仍指向原 Json 的字符串
- 标准库字符串 `std::string`，拷贝字符串到独立副本

这两种类型的变量都可以放到 `|` 或 `&` 的后面，用于读取（默认值）或类型判断。
由于 `std::string` 可以从 `const char\*` 构造或赋值，所以读取字符串最简单的写法
就是 `| ""` ，再由 `=` 左侧的变量决定最终使用什么字符串类型。

<!-- example:usage_2_4_4_string_type -->
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
收的参数类型是 `const char \*` ，这就需要对后面两种特殊字面作额外的比较判断是否
对象或数组。

#### 2.4.5 特殊 Json 类型 Null

xyjson 也提供了 `kNull` 常量（其实也就是 `nullptr`）与 `isNull` 方法来判断一个
`Value` 是否 Null 结点。但要注意，Null 结点也是有效结点，无效结点却不是 Null
结点，也不是任何其他类型的结点。

<!-- example:usage_2_4_5_null_type -->
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

### 2.5 比较操作

#### 2.5.1 等值比较 `==`

Document 类与 Value 类都重载了 `==` 及 `!=` 操作符，执行两棵 Json 树的深度比较：

<!-- example:usage_2_5_1_equal_compare -->
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

<!-- example:usage_2_5_1_scalar_compare -->
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

#### 2.5.2 有序比较 `<`

也重载了 `<` 用于比较两个同类型的 `Value` 或 `MutableValue` ，但交叉比较
`Value` 与 `Mutable` 的大小（或相等）没有定义。有序比较不如等值比较常用，但在
有需要时 `<` 定义了确定性的排序，其规则大致是：

- 先按 json 类型排序：null 最小，object 最大
- 标量类型，主要是数字与字符串，按其内容或值比较
- 容器类型，数组与对象，先按容器大小比较，再按内部指针比较（避免递归性能问题）

由 `<` 也衍生出其他几个比较操作 `>` , `<=` 与 `>=` 。在实际业务项目中，若先确
知两个 json 的类型相同，可选使用简化操作。

但是 `<` 不能用于 json 与基本类型比较，因为考虑到类型不同时难以逻辑自洽。

### 2.6 类型转换的一元操作符

Json 标量类型中最常用的就是整数与字符串两种，所以 Value 类提供了两个方法将
Json 结点尽量转为整数与字符串：

- `+`: toNumber 转整数
- `-`: toString 转字符串

可类比于 `!` 将 Json 放在布尔逻辑上下文使用，`+` 与 `-` 就将 Json 分别放在
整数与字符串上下文使用。

#### 2.6.1 JSON 转整数 `+`

相对于  `| kInt` 有严格的类型检查，操作符 `+` 或 `toInteger` 方法则有相当宽松
的规则将任意 json 结点转为一个整数 `int`：

- 整数类型，最直接，但大整数可能会被截断；
- 实数类型，取整；
- 字符串类型，按 `atoi` 转换方法将字符串前缀转整数；
- 容器类型，数组或对象，取容器大小；
- 特殊类型，`null` 与 `false` 转 `0`，`ture` 转 `1`；
- 无效值，`Value` 本身无效也返回 `0`。

<!-- example:usage_2_6_1_to_integer -->
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

#### 2.6.2 JSON 转字符串 `-`

Value 类另有一个转字符串的方法 `toString` ，返回 json 的字符串表示，在大多情况
下与 write 的输出是相同的，只有当 json 是字符串类型时，`toString` 默认返回的是
不带序列化引号包围的纯值。在传入可选参数 `true` 时，`toString` 返回格式化的
Json 串，也与 `write` 无格式序列化不一样。

<!-- example:usage_2_6_2_to_string -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

std::string result; // toString 的返回类型
result = -(doc / "name");  // 结果：Alice
result = -(doc / "age");   // 结果：30
result = -doc.root();      // 结果：{"name":"Alice","age":30}

result = (doc / "name").toString(true); // 结果："Alice"
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

#### 2.6.3 Document 只读可写互转 `~`

操作符 `~` 可以将 `Document` 转为 `MutableDocument` 或反向转换，相应方法名是
`mutate` 和 `freeze` 。

<!-- example:usage_2_6_3_doc_convert -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

yyjson::MutableDocument mutDoc = ~doc;
auto doc2 = ~mutDoc;
bool result = (doc2 == doc); // 结果：true
```

只读的 Document 效率更高，一般的建议是先将输入 Json 解析为 `Document`，在有需
要修改时再转为 `MutableDocument` 。

#### 2.6.4 字符串字面量直接转 Document `_xyjson`

字面量运算符不是传统的操作符，它是在 C++11 开始引入的，通过在字面量加后缀来自
定义某种操作。在 `yyjson::literals` 命名空间定义了 `_xyjson` 操作符，可以从字
面量构造 `Document` 类对象。其用法如下：

<!-- example:usage_2_6_4_literal_operator -->
```cpp
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
auto name = doc / "name" | "";
auto age = doc / "age" | 0;

std::cout << name << "=" << age << std::endl; // 输出：Alice=30
```

从之前的示例可以看到，通过操作符重载，在代码中除了需要用法 `Document` 类构造外，
就几乎不必显式用到其他类与方法名。现在再通过 `_xyjson` 后缀操作符，连起点的
`Document` 类名都可以不出现在代码中了。当然了，字面量操作符可能只在测试中更常
见且简洁，而实际的项目中很少出现固定字面量的 json 串。

## 3 可写 JSON 模型操作

可写 JSON 模型（MutableDocument 和 MutableValue）也支持上一节的所有操作，
本节着重讲解它额外支持的动态修改和添加 Json 数据结点的功能。

### 3.1 创建与初始化可写文档

创建可写文档时，可以指定初始 JSON 内容，或者从空对象/数组开始：

<!-- example:usage_3_1_create_mutable -->
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

### 3.2 赋值操作修改已有结点 `=`

`MutableValue` 支持操作符 `=` 或 `set` 方法修改已有 Json 结点的值。

<!-- example:usage_3_2_basic_assign -->
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

#### 3.2.1 修改整数类型

前文提到，yyjson 在解析只读文档时，只会将负整数保存为 `int64_t`，非负整数保存为
`uint64_t`。但在可写文档中，可以显式修改设定整数类型：

<!-- example:usage_3_2_1_integer_type -->
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
std::string json = mutDoc.root().toString();
yyjson::Document doc(json);
if (doc / "age" & int64_t()); // kSint: false
```

然而要注意的是，将 json 序列化后，这种细节类型信息就丢失了。重新读入后，非负整
数仍然会保存为 `uint64_t`，而不是 `int64_t` 。

#### 3.2.2 使用类型常量赋值

那些可用于 `&` 作类型判断的常量符号，也可用于 `=` 赋值，将目标结点改为各类型的
默认值，也就是零值或空值。

<!-- example:usage_3_2_2_type_constant_assign -->
```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"({"name":"Alice", "age":"30", "sex":"null", "score":100, "friend":"Bob"})";

mutDoc / "name" = "";     // kString
mutDoc / "age" = 0;       // kInt
mutDoc / "sex" = nullptr; // kNull
mutDoc / "score" = "{}";  // kObject
mutDoc / "friend" = "[]"; // kArray

std::cout << mutDoc << std::endl;
//^ 输出：{"name":"","age":0,"sex":null,"score":{},"friend":[]}
```

可以修改 Json 标量结点的类型，但根据 yyjson 的建议，不要为非空数组或对象重新赋
值为标量，这会导致容器内大量子结点变得不可访问，很可能是非预期的。但是将标量先
改为空数组或空对象，一般是安全且有用的，以备后面扩展 Json 结构。

#### 3.2.3 Value 类型本身的赋值

请注意 `=` 操作符还承担了标准的类对象赋值拷贝功能。`Value` 与 `MutableValue`
是值类型的类，支持拷贝与同类赋值（Document 不可拷贝只能移动）。同类赋值只是拷
贝底层 Json 结点的指针引用。赋值为其他基本类型则会修改改所引用结点的存储值，所
以只支持 `MutableValue` 不支持 `Value` 。

<!-- example:usage_3_2_3_value_assign -->
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

### 3.3 索引操作自动插入结点 `[]`

在可写模型中，操作符 `[]` 和 `/` 有不同的语义和行为：

- **`[]` 操作符**：支持自动插入新字段，会检查键是否存在，不存在时自动创建
- **`/` 操作符**：只能访问已存在的路径，如果路径不存在会返回无效值

<!-- example:usage_3_3_auto_insert -->
```cpp
yyjson::MutableDocument mutDoc;

mutDoc["name"] = "Alice";     // 自动创建并设置 name 字段
mutDoc["age"] = 30;           // 自动创建并设置 age 字段

mutDoc / "name" = "Bob";      // 修改已存在的 name 字段

//! 错误用法：尝试用 / 操作符创建新字段，没有任何效果
mutDoc / "sex" = "female";
if (mutDoc / "sex"); // false

// 正确用法：先用 [] 创建，再用 / 或 [] 修改
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

### 3.4 输入操作增加新结点 `<<`

输入操作符 `<<` 可以向 Json 容器，数组与对象，添加新结点，对应的方法名是
`push` 。只能在容器末尾添加，后文借助迭代器再介绍在中间位置插入结点。

#### 3.4.1 给数组添加结点

相应的具名方法是 `append` ，可以添加各种基本类型的值或其常量代表值，以及
`Value` 与 `MutableValue` 。

<!-- example:usage_3_4_1_add_array -->
```cpp
MutableDocument mutDoc;

mutDoc["num1"] = "[]";
mutDoc["num2"] = kArray;

mutDoc["num1"].append(1).append(2).append(3);
mutDoc / "num2" << 1 << 2 << 3;

std::cout << mutDoc / "num1" << std::endl; // 输出：[1,2,3]
std::cout << mutDoc / "num2" << std::endl; // 输出：[1,2,3]

(mutDoc["mix"] = kArray) << false << 5.5 << 666 << "[]" << "{}" << "end";
std::cout << mutDoc / "mix" << std::endl; // 输出：[false,5.5,666,[],{},"end"]
```

可见 Json 数组内可以添加任意其他 Json 类型的值，虽然在实践项目中一般会规范要求
数组元素应该相同类型。

#### 3.4.2 给对象添加结点

Json 对象容器的元素是键值对，包含两个值，这与数组有点不一样。`add` 方法要求两
个参数，仍可用操作符 `<<` 但要求必须连续使用两次先后输入键与值。

<!-- example:usage_3_4_2_add_object -->
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
{"name":"Alice","age":35,"sex":"Female","login":false,"name":"Alice","age":"25"}
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

- **性能提示**：使用 `<<` 给对象添加新字段比 `[]` 更有效率。
- **错误警示**：使用 `<<` 前判断 Json 类型结点。

#### 3.4.3 从头构建复杂 Json

有了 `<<` 操作符，就可以动态构建具有复杂层次结构的 Json 了。例如：

<!-- example:usage_3_4_3_build_object -->
```cpp
yyjson::MutableDocument mutDoc;

// 链式添加数组元素
mutDoc["numbers"] = "[]"; // mutDoc.root() << "numbers" << kArray;
mutDoc / "numbers" << 1 << 2 << 3 << 4 << 5;

// 链式设置对象属性，键、值需成对出现
mutDoc["config"] = "{}"; // mutDoc.root() << "config" << kObject;
mutDoc / "config" << "timeout" << 30 << "retries" << 3 << "debug" << true;

// 混合类型数组
mutDoc["mixed"] = "[]"; // mutDoc.root() << "mixed" << kArray;
mutDoc / "mixed" << 42 << "text" << 3.14 << false;

mutDoc / "config" / "timeout" = 40;
```

以上代码构建的 Json 等效于：

<!-- example:usage_3_4_3_build_static -->
```cpp
yyjson::MutableDocument mutDoc;

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

### 3.5 创建结点、移动与复制

以上用 `<<` 往 Json 数组添加新结点时，其实分两步：
1. 在 `MutableDocument` 所管理的内存池中创建结点，
2. 将新结点真正添加到已有的容器结点中。

在插入对象时，第 2 步又需额外做个状态检查，如果没有悬挂键，就先创建键结点，否
则创建值结点，将键值对一起插入底层对象，然后清空悬挂键。

#### 3.5.1 键值对绑定

为了避免 `<<` 操作对象时插入键值对非原子性与悬挂键的问题，也提供了另一种操作方
案。分 3 步：
1. 先分别创建键结点与值结点，方法名 `MutableDocument::create`
2. 将键结点与值结点绑定为一个中间类型，方法名 `MuatbleValue::tag`
3. 将中间类型表示的键值对一次调用插入到对象中，方法名 `MuatbleValue::add`

这个中间类型名不关键，就叫 `KeyValue` ，只包含两个指针的轻量值类型。前两步方法
可用二元操作符 `\*` 代替，第三步就是上节已介绍过的 `<<` 。由于 `\*` 操作符优先
级比 `<<` 高，可以方便地写在一个表达式中。

这三步法也可稍作简化：
1. 只从基本类型创建值结点；
2. 将值结点与字符串类型表示的键名绑定；
3. 将键值对一次性插入对象中。

按此思路插入对象的示例如下：

<!-- example:usage_3_5_1_keyvalue_binding -->
```cpp
yyjson::MutableDocument mutDoc;

yyjson::MutableValue name = mutDoc * "Alice"; // mutDoc.create("Alice")
yyjson::MutableValue age = mutDoc * 25;       // mutDoc.create(25)
auto kv = std::move(name) * std::move(age);
//^ 等效: std::move(age).tag(std::move(name));

auto root = *mutDoc;
root << std::move(kv);
std::cout << mutDoc << std::endl; // 输出：{"Alice":25}

// 这些操作可写成一行表达式，以下每一行都是等效的
root << (mutDoc * "Alice") * (mutDoc * 25); // 键结点 * 值结点
root << "Alice" * (mutDoc * 25); // 键名 * 值结点，括号是必须的
root << (mutDoc * 25) * "Alice"; // 值结点 * 键名
root << mutDoc * 25 * "Alice";   // 值结点 * 键名，省去括号
root << mutDoc.create(25).tag("Alice"); // root.add("Alice", 25)
```

注意键值对绑定是移动语义，表示将要打包插入到目标对象中。如果不是移动语义，就表
示允许将这个键值对重复插入，那就有问题。因此一般用在 `\*` 表达式中，不要保存中
间变量成为左值，保存左值后要再利用还得显式用 `std::move` 转右值。

至于为什么选用乘号 `\*` 来表达结点创建与绑定的操作，因为它与路径查找的除号 `/`
原是互逆关系，一个结点要在 Json 树中通过 `/` 找到，那么它在创建时就该用 `\*`
了。既然二元乘 `\*` 是创建结点，那么一元 `\*` 作用于 Document 取其根结点也相似
了。

#### 3.5.2 移动独立结点

使用两个 `\*` 连乘创建的键值对不能用 `<<` 添加到数组中，用一个 `\*` 创建的结点
可以用 `<<` 添加到数组（与对象）中。注意左值时是复制结点，右值则以移动的方式插
入目标容器。对于新创建的独立结点，一般是期望以移动式插入到 Json 树的某个路径下
，除非有意保留模板多处插入相同的结点。

<!-- example:usage_3_5_2_move_node -->
```cpp
// 创建根为数组的 Json
yyjson::MutableDocument mutDoc("[]");

// 创建独立于 Json 树根的对象，填充信息
auto user = mutDoc * "{}";
user << "name" << "Alice" << "age" << 30;

// 将对象移入到 Json 根数组中
mutDoc.root() << std::move(user);
if (!user) {
    std::cout << "moved" << std::endl;
}

// 创建另一个对象，可复用 user 变量名，再移入
user = mutDoc * "{}";
user << "name" << "Bob" << "age" << 25;
mutDoc.root() << std::move(user);

std::cout << mutDoc << std::endl;
//^ 输出：[{"name":"Alice","age":30},{"name":"Bob","age":25}]

// 再创建一个数组
auto favor = mutDoc * "[]";
favor << "book" << "movie" << "music";

// 将这个新数组复制两份插入到前两个（已挂到树上的） user 对象
mutDoc / 0 << "favor" << favor;
mutDoc / 1 << "favor" << favor;

std::cout << mutDoc << std::endl;
//^ 输出：[{"name":"Alice","age":30,"favor":["book","movie","music"]},{"name":"Bob","age":25,"favor":["book","movie","music"]}]

if (favor) {
    std::cout << favor << std::endl;
    //^ 仍有效，输出：["book","movie","music"]
}
```

像这样自下而上先操作新建的独立结点，内容准备好后再挂到树上的方法，比自上而下先
在树上添加空数组或空对象占位结点，再用 `/` 路径操作符重新取出来操作的方法，效
率要高一些，因为避免了重复的路径查找。

### 拷贝已有结点

但是对于已经挂在树上的结点，不能用 `std::move` 移动插入到另一个地方，那可能发
生逻辑错误。所以已有结点只能拷贝才能安全插入。

<!-- example:usage_3_6_copy_node -->
```cpp
yyjson::MutableDocument mutDoc(R"(["Alice",25])");

auto age = mutDoc / 1;       // 引用已有结点
mutDoc.root() << "Bob" << age; // 拷贝原结点，不要用 std::move(age)
std::cout << mutDoc << std::endl; // 输出：["Alice",25,"Bob",25]

// 也可以先显式用 * 或 create 创建新独立结点再移动插入
auto newNode = mutDoc * age;
// 第二个 << 后面直接用 mutDoc * age 代替也可以，临时值也是右值移动语义
mutDoc.root() << "Candy" << std::move(newNode);
std::cout << mutDoc << std::endl; // 输出：["Alice",25,"Bob",25,"Candy",25]
```

另一个更常见的情景是需要将另一个 Document 的 Json 树（或其某个子树）插入到到另
个文档中，因两个文档的内存池不同，也需要拷贝。

<!-- example:usage_3_6_copy_document -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

auto doc2 = ~doc;
doc2 / "name" = "Bob";
doc2 / "age" = 25;

yyjson::MutableDocument mutDoc("[]");
mutDoc.root() << doc << doc2; // 或 << doc.root() << doc2.root()
std::cout << mutDoc << std::endl;
//^ 输出：[{"name":"Alice","age":30},{"name":"Bob","age":25}]
```

将上个示例的两个源文档改为插入目标文档的对象中：

<!-- example:usage_3_6_copy_to_object -->
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
//^ 输出：{"first":{"name":"Alice","age":30},"second":{"name":"Bob","age":25}}
```

- **错误警示**：路径 `/` 返回的临时值，不要放在 `<<` 后面。

### 3.6 字符串引用

yyjson 为每个 Json 文档树管理的内存池分为两部分，一是大小一致的 Json 结点，二
是不定长的字符串。当创建字符串结点时，一般需要将字符串拷进内存池，但是也可以在
引用安全的时候不拷贝。基于此，xyjson 的一些操作能自动识别字符串字面量，进行字
面量引用优化，因为那是有静态生命周期的字符串，引用肯定安全。

<!-- example:usage_3_6_string_ref -->
```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"({"name": "Alice", "age": 30})";
auto root = *mutDoc;

root / "age" = "25";     // 改结点类型为字符串，但引用字面量
std::string strName = "Alice.Green";
root / "name" = strName; // 或 strName.c_str()，都会拷贝

// 自动插入的键名，不能获得字面量优化
root["friend"] = "[]"; // 特殊字面量，空数组
// 添加数组元素，可以引用字面量
root / "friend" << "Bob" << "Candy";

// 插入对象的键名与字符串值，都能获得字面量优化
root << "telephone" << "{}";
root / "telephone" << "Home" << "1234567" << "Office" << "7654321";

std::cout << mutDoc << std::endl;
```

在实际项目中，固定的 json 键名往往可以用字面量，一些约定的简短标志内容如
`"OK"` 、`"Fail"` 也有时用字面量。当然有些项目规范不建议直接在代码中写字面量，
而是用宏或常量间接表示，当定义常量时要注意类型如果是 `const char\*` 类型就无法
获得自动优化，应该定义为 `const char[]` ，宏替换与手写字面量却是完全一样的。

<!-- example:usage_3_6_string_literal_const -->
```cpp
#define OK "OK"
constexpr const char* kSucc = "Succ";
constexpr const char kFail[] = "Fail";

yyjson::MutableDocument mutDoc("[]");
mutDoc.root() << OK << kSucc << kFail;
std::cout << mutDoc << std::endl; // 输出：["OK","Succ","Fail"]

if (mutDoc[0] | "" == OK) {
    std::cout << "ref" << std::endl;
}
if (mutDoc[1] | "" != kSucc) {
    std::cout << "copy" << std::endl;
}
if (mutDoc[2] | "" == kFail) {
    std::cout << "ref" << std::endl;
}
```

对于非字符串字面量，如果用户能负责保证来源字符串生命周期足够长，提供了一个封装
类 `yyjson::StringRef` 显式转为可引用字符串，然后由此创建的 Json 结点就像字符
串字面量优化一样不会发生拷贝。

<!-- example:usage_3_6_stringref -->
```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"({"name": "Alice", "age": 30})";
auto root = *mutDoc;

std::string strName = "Alice.Green";
auto refName = yyjson::StringRef(strName.c_str(), strName.size());
root / "name" = refName;

if (root / "name" | "" == strName.c_str()) {
    std::cout << "ref" << std::endl;
}
```

## 4 迭代器使用

Json 数组与对象是一种容器，那就应该可以用迭代器来高效访问。 xyjson 提供了四种
迭代器类型，只读与写模型分别有对应的数组与对象迭代器：

- `ArrayIterator` 只读数组迭代器；
- `MutableArrayIterator`只读对象迭代器；
- `ObjectIterator` 可写数组迭代器；
- `MutableObjectIterator`可写对象迭代器。

一般不必刻意记迭代器类型，它们分别由 Value 或 MutableValue 创建，用 `auto` 接
收就行。

为了正确理解与使用迭代器，最好先了解一下 yyjson 的内存结构。

### 4.1 容器结构与迭代器模型

在 yyjson 中，数组与对象在内存的存储方式是非常相似的。数组是每个结点的线性排列
，对象是每两个结点（键值对）的线性排序。但是只读模型与可写模型有很大区别，
只读模型是一种致密的、跨越式的排列方式，可写模型是一种环形链表的排列方式。

一种简单的结构表示法如下：
```
只读 array  | V0 | V1 | V2 ... | Vn
只读 object | K0 | V0 | K1 | V1 ... | Kn | Vn

可写 array --> Vn --> V0 --> V1 --> V2 ... --> Vn
可写 object --> Kn -> Vn --> K0 -> V0 --> K1 -> V1 ... --> Kn -> Vn
```

假设有如下一个 json ：
```json
["Alice", {"name":"Bob", "age":25}, "Candy"]
```

当解析为 yyjson 的只读 Document 模型时，它将严格按照源 Json 串的顺序在内存池中
逐个创建 json 结点如下：

- node[0]: 数组 [], 根结点，含 3 个直接子结点，7 个子孙结点
- node[1]: 字符串 Alice
- node[2]: 对象 {}，含 4 个子结点，两组键值对
- node[3]: 字符串 name
- node[4]: 字符串 Bob
- node[5]: 字符串 age
- node[6]: 正整数 25
- node[7]: 字符串 Candy

每个结点都是整齐堆砌的 16 字节，字符串内容放在单独内存池区域，在结点区只存个指
针。数组与对象本身也占据一个结点位置，其内容记录的是紧随其后子结点数量。所以这
个 Json 被解析为 8 个结点，1 个根结点数组加上其所辖 7 个子孙结点，其中第 2 个
元素是对象，包含 4 个子结点，总长度是 5 个结点。这也相当于一棵 Json 树的一维拓
扑结构。

只读模型的意思，不仅解析完后不可修改，即使在解析过程中，也是单程写入的，已经解
析好的结点不可再修改或移动，除了容器结点记录子结点数的信息必须在容器结束时才能
回填更新。这是 yyjson 库高效的底层原理。

为根数组创建迭代器时，迭代器记录了如下信息：
- 当前索引计数，初始化为 0，
- 最大索引计数，即数组长度 3
- 当前结点指针，就是被迭代的数组结点指针 +1，也就是 node[1] 的内存地址。

为其中第二元素的对象创建迭代器时，迭代器则记录如下 信息：
- 当前索引计数，初始化为 0，
- 最大索引计数，即数组长度 2
- 当前结点指针，就是被迭代的对象结点指针 +1，也就是 node[3] 的内存地址。

然后迭代器的前进，也即经典 `++` 操作，也就好理解了，数组迭代器前移两个逻辑结点
，对象迭代器每次前移两个逻辑结点。

这里所说按逻辑结点移动，是因为与内存物理结点未必一一对应。比如根结点数组从
`root[0]` 前移到 `root[1]` ，那是只要偏移一个内存结点的指针，但从 `root[1]` 前
移到 `root[2]` 就是偏移 5 个内存结点的指针了，因为 `root[1]` 不是标量，它代表
的对象总长 5 个结点。

也由于这个原因，yyjson 的迭代器只保证前向迭代器级别，不保证随机迭代器。也就是
说类似 `array[n]` 的索引操作不能像 C/C++ 原生数组那样具有 O(1) 的常数复杂度，只
能从头开始重复 `n` 次的 `++` 操作达到 `array[n]` 的目的，是 O(N) 的线性复杂度
。

同时 yyjson 的迭代器也不支持 `--` 后退操作。仍以上例而言，虽然 `root[1]` 知道
自己跨越 5 个结点的长度，可以前移到 `root[2]` ，但是当指针已经定位到 `root[2]`
时，它却不知道该后退几个结点才能回到 `root[1]` 。

可写模型又更复杂些。为了支持灵活修改，每个 json 结点就不是紧密堆砌了，而是多加
了一个 `next` 指针，指向下一个兄弟结点。容器的所有子结点的 `next` 指针首尾相接
，组成环形单向链表。对象的每个键值对结点也依次连接，一个键必定连接一个值结点，
然后连接下一个键结点。

而容器结点本身的内容（不是 `next` 指针，而是处于原有的 16 字节中），又持有一个
头指针，指向它的子结点环链中的尾结点，即数组的最后一个值结点，或对象的最后一个
键结点。空容器的头指针，显然是 `nullptr` ，相当于空字符串的内容指针可以是
`nullptr` 。

以本节上文的 json 为例，若转为可写模型，其结构示意简图标记如下：

```
node[0] (root)
     |----------------------
     v                     |
node[8] -> node[1] -> node[2] ({})
(Candy)    (Alice)         |
                           v
                      node[5] -> node[6] (age:25)
                           ^          |
                           |          v
                      node[4] <- node[3] (name:Bob)
```

在 yyjson 中，为可写数组或对象设计的原生迭代器的当前索引与最大索引意义与只读迭
代器一样，当前指针初始化也是容器的头指针，也即环链中的尾结点，此外它还多存了一
个 `prev` 指针，后续迭代时保存前一个结点。可写数组迭代器每次前进，就沿着结点
`next` 指针移动一次，可写对象迭代器就沿 `next` 移动两次。每次调用 C API 的
`next()` 方法时返回下一个将迭代的结点。

在 xyjson 封装为 C++ 迭代器类时，`next()` 方法对应 `++` 操作，它不返回结点值，
所以与真正取结点值的解引用操作 `\*` 是分离使用的。为适应 C++ 标准迭代器的惯用法
，迭代器创建初始化时内部保存的当前指针指向第一个结点（非空时），`prev` 指向尾
结点（或容器本身的头指针）。在迭代中保证当前指针指向当前真正要处理的结点，注意
这个语义与原生迭代器有一个结点位差。

与只读迭代器级别一样，可写迭代器也只能一步步前进，不能随机前进，也不能后退。假
设容器大小是 `n` ，那迭代器最大索引也是 `n` 。当迭代器前进 `n` 次后，按环链特
性，当前指针它又回到了初始的位置，但是当前索引也累积到了 `n` ，与最大索引相同
，故此判断认为迭代器已经越界，算失效。但是恰好移到最后越界的迭代器位置，允许插
入，相当于在容器末尾追加元素。环链结构使得它在最前面插入或最后面追加元素都是
O(1) 复杂度，其他位置则是 O(N) 复杂度。删除最后面的元素也是 O(N)，因为需要先移
到最后（但未越界）的位置。

### 4.2 迭代器创建与基本遍历

迭代器通过 `%` 操作符创建，右侧参数是整数时创建数组迭代器，字符串时创建对象
迭代器：

<!-- example:usage_4_2_iter_create -->
```cpp
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
auto mutDoc = ~doc;

// 对象迭代器
for (auto iter = doc % ""; iter; ++iter) { }
for (auto iter = mutDoc % ""; iter; ++iter) { }

doc << R"(["name", "Alice", "age", 30])";
mutDoc = ~doc;

// 数组迭代器
for (auto iter = doc % 0; iter; ++iter) { }
for (auto iter = mutDoc % 0; iter; ++iter) { }
```

可以将 `%` 视为路径操作 `/` 的变种，因为迭代器的主要作用是为重复路径操作提效用
的。创建迭代器的具名方法是 `iterator`根据参数类型重载决定创建哪种迭代器类型，
整数参数表示创建数组迭代器，字符串参数表示创建对象迭代器。操作符 `%` 也可以直
接用于 Document ，相当于从其根结点创建迭代器，但是具名方法只能显式先调用
`root` 取根结点。

创建迭代器的参数不仅类型有区分，值也可以表示初始位置。虽然最常用 `0` 或空字符串
`""` 表示从头开始迭代，但也可以传其他参数值表示从中间某个位置开始迭代。

<!-- example:usage_4_2_iter_with_startpos -->
```cpp
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;

// 对象迭代器，从第二个键值对 "age" 开始迭代
// doc.root().iterator("age")
for (auto iter = doc % "age"; iter; ++iter) { }

doc << R"(["name", "Alice", "age", 30])";

// 数组迭代器，从第三个索引开始迭代（第一个的索引是 0）
// doc.root().iterator(2)
for (auto iter = doc % 2; iter; ++iter) { }
```

简单类比一下，`doc / "age"` 与 `doc % "age"` 都指向同一个 Json 结点，只不过
`%` 多一层间接性，以交换后续迭代访问的性能。不过注意空路径 `json / ""` 指向
`json` 本身，而不是其第一个子结点。

如果不喜欢用 `0` 与 `""` 这样的魔数，也可以用类型代表值，`%` 后接 `kArray` 表
示创建数组迭代器，`kObject` 表示创建对象迭代器。当然无法再指定初始位置，就按默
认的从头开始迭代，这也是最常用的。

<!-- example:usage_4_2_iter_type_constants -->
```cpp
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;

// 对象迭代器 doc.root().iterator(kObject)
for (auto iter = doc % kObject; iter; ++iter) { }

// 数组迭代器 doc.root().iterator(kArray)
doc << R"(["name", "Alice", "age", 30])";
for (auto iter = doc % kArray; iter; ++iter) { }
```

还有一种类似标准库的创建迭代器方法，成对的 begin 与 end 。但是由于 Json 有两种
类型的迭代，不能直接用 begin 与 end 命名，于是有两种变种：

<!-- example:usage_4_2_iter_begin_end -->
```cpp
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;

// 对象迭代器
for (auto it = doc.root().beginObject(); it != doc.root().endObject(); ++it) { }

// 数组迭代器
doc << R"(["name", "Alice", "age", 30])";
for (auto it = doc.root().beginArray(); it != doc.root().endArray(); ++it) { }
```

### 4.3 无效迭代器与错误处理

创建迭代器类型要符合原结点本身的类型，如果错误地通过对象结点创建数组迭代器，
或通过数组结点迭代器创建对象迭代器，那将直接返回一个无效的迭代器。从空数组或空
对象创建正确类型的迭代器也是一开始就失效了。有效迭代器在到达数组末尾或对象末尾
越界后也失效。

无效迭代器在 `if` 或 `for` 的条件判断中返回 `false` ，因为各迭代器都实现了
`oprator bool` 的重载。

<!-- example:usage_4_3_iter_validity -->
```cpp
// 迭代器有效性检查示例
yyjson::Document doc;
doc << R"({"users": [{"name":"Alice", "age":25}, {"name":"Bob", "age":30}]})";

// 1. 正确创建迭代器
auto validArrayIter = doc / "users" % 0;
if (validArrayIter) {
    std::cout << "OK" << std::endl;
}

// 2. 错误类型地创建迭代器
auto invalidObjectIter = doc / "users" % "";
if (!invalidObjectIter) {
    std::cout << "Error" << std::endl;
}

// 3. 空数组的迭代器
yyjson::Document emptyArrayDoc("[]");
auto emptyArrayIter = emptyArrayDoc.root() % 0;
if (!emptyArrayIter) {
    std::cout << "Empty" << std::endl;
}

// 4. 空对象的迭代器
yyjson::Document emptyObjectDoc("{}");
auto emptyObjectIter = emptyObjectDoc.root() % "";
if (!emptyObjectIter) {
    std::cout << "Empty" << std::endl;
}

// 5. 迭代器边界检查
yyjson::Document arrayDoc("[1, 2, 3]");
auto iter = arrayDoc.root() % 0;
for (int i = 0; iter; ++iter, ++i) { }

// 迭代器已到达末尾失效
if (!iter) {
    std::cout << "End" << std::endl;
}
```

### 4.4 迭代器解引用

迭代器的基本操作是解引用 `\*`，像指针一样获取它所指向的对象。在 xyjon 中，可以
将 Value （代理类）视为 Json 结点，那么由它创建的迭代器，解引用后获取的对象应
该能代表它的子结点，也是 Value 类。

数组迭代器易理解，解引用就是每个子结点元素：

<!-- example:usage_4_4_array_iter_deref -->
```cpp
yyjson::Document doc;
doc << R"(["name", "Alice", "age", 30])";

for (auto it = doc % 0; it; ++it) {
    if (*it & "") { // 解引用操作符 *it 优先级高，不必加括号
        std::cout << (*it | "") << ","; // | 优先级比 << 低，要加括号
    }
    else if (it->isInt()) { // 调用方法，用 -> 更方便，否则 (*it).isInt()
        std::cout << (*it | 0) << ",";
    }
}
std::cout << std::endl; // 输出：name,Alice,age,30,
```

对象从概念上讲是键值对组合，但 xyjson 设计的对象迭代器，为了与数组迭代器解引用
接口一致，它的解引用也是返回一个 Value 类，表示的是键值对中的值结点。


<!-- example:usage_4_4_object_iter_deref -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

for (auto it = doc % ""; it; ++it) {
    if (*it & "") {
        std::cout << (*it | "") << ",";
    }
    else if (it->isInt()) {
        std::cout << (*it | 0) << ",";
    }
}
std::cout << std::endl; // 输出：Alice,30,
```

至于键值对中的键结点，可用对象迭代器的 `key` 方法获取，相应的也有 `value` 方法
，其效果就是与解引用 `\*` 操作相同：


<!-- example:usage_4_4_iter_key_value -->
```cpp
yyjson::Document doc;
doc << R"({"name": "Alice", "age": 30})";

for (auto it = doc % ""; it; ++it) {
    if (*it & "") {
        std::cout << (it.key() | "") << "," << (it.value() | "") << ",";
    }
    else if (it->isInt()) {
        std::cout << (it.key() | "") << "," << (it.value() | 0) << ",";
    }
}
std::cout << std::endl; // 输出：name,Alice,age,30,
```

对象迭代器还有另一个方法 `name` 方法，返回 `const char \*` 字符串类型的键名，
指向存在 json 结点内的字符串，只要 json 树有效，这个指针也就有效。在上例中，如
果用 `name()` 方法代替 `key()` ，就不必加 `|""` 取值操作了。

数组迭代器也有 `value` 方法与解引用等效。它没有键的概念，所以 `key` 与 `name`
方法无效，但它另有一个索引的概念，可用 `index` 返回，是 `size_t` 类型。

为了与一元操作 `\*` 解引用等效 `value` 方法，也定义了一元操作 `~` 等效 `key`
方法。另外为与 json 的一元 `+` 与 `-` 分别转整数与字符串的功能对应，`+` 迭代器
取索引 `index` 方法，`-` 取键名 `name` 方法。当然这可能易与迭代器常见的 `++`
与 `--` 视觉混淆，可选慎用。

另外有几个 `c` 前缀的方法，用于取底层的指针类型：
- `c_val` 取底层值结点指针 `yyjson_val\*` 或 `yyjson_mut_val\*`
- `c_key` 取底层键结点指针 `yyjson_val\*` 或 `yyjson_mut_val\*`
- `c_iter` 取底层迭代指针，四种迭代器类型各不相同

### 4.5 迭代器移动

迭代器支持最显而易见的 `++` 操作，前进一步，在前面的示例中已经不假思索地使用了
。具名方法 `next` 对应前缀 `++`，后缀 `++` 操作也支持，但不如前缀常用，故没有
直接对应的具名方法（当然拷贝的迭代器也调用 `next` 方法）。

由于 yyjson 底层的数据结构，迭代器只支持前向迭代，不支持 `--` 后退操作，也不能
真正支持随机移动。支持的二元 `+` 与 `+=` 操作其实由 `++` 前进 `N` 次实现的，复
杂度是 `O(N)`。

<!-- example:usage_4_5_iter_move_plus -->
```cpp
yyjson::Document doc;
doc << R"([1,2,3,4,5,6])";

// 从第二个元素开始迭代，每次进两步
for (auto it = doc % 1; it; it +=2) {
    std::cout << (*it | 0) << ",";
}
std::cout << std::endl; // 输出：2,4,6,
```

对象也是线性存储的，所以对象迭代器 `+ n` 操作也有意义，相当于根据 Json 源字符
串的顺序将逐步 `n` 步。另外还支持 `% "key"` 与 `%= "key"` 表示重新定位到 `key`
的位置，事实上也支持整数索引参数的重新定位。

<!-- example:usage_4_5_iter_move_plus_object -->
```cpp
yyjson::Document doc;
doc << R"({"one":1, "two":2, "three":3, "four":4, "five":5, "six":6})";

// 从第二个元素开始迭代，每次进两步
for (auto it = doc % "two"; it; it += 2) {
    std::cout << (*it | 0) << ",";
}
std::cout << std::endl; // 输出：2,4,6,

// 不用循环，已知每个键名，向前搜索
auto it = doc % "two";
std::cout << (*it | 0) << ",";
it %= "four";
std::cout << (*it | 0) << ",";
it %= "six";
std::cout << (*it | 0) << ",";
it %= "eight";
if (!it); // 找不到键名，迭代器无效了
std::cout << std::endl; // 输出：2,4,6,
```

符合常规语义，后缀 `++` 、二元 `+` 与 `%` 都产生新迭代器，而前缀 `++` 、`+=`
与 `%=` 不会创建新迭代器，只改变自身状态。另外由于迭代器类比 Value 类还大一些
，故创建新迭代器的操作效率会慢一些。

### 4.6 对象迭代器的静态快速搜索

在 yyjson 中，利用对象迭代器还支持一种特殊的搜索方法，xyjson 将其封装为对象迭
代器的 `seek` 方法，或 `/` 操作符。它用于按编译期已确定的固定顺序访问对象的一
系列键，若访问顺序与源 Json 串相同，就能达到很高的效率。这条件虽有些苛刻，但在
规范的工程实践中却很有意义。

假设有如下这样一个 Json 结构来表示一个用户的信息：

```json
{"name":"Alice", "sex":false, "age":25, "height":163.5, "weight":53.3}
```

实际中可能还有许多字段，远不止 5 个。其中性别一般认为只有两种，所以用 bool 类
型来表达了，譬如 `true` 表示男，`false` 表示女。

对于这样的 json 对象，每个字段都有不同类型与物理意义，在大部分业务处理中，显然
不能用简单的迭代器循环统一处理，只能依次读取每个字段分别处理。最容易想到的处理
方法可能是：

<!-- example:usage_4_6_object_iter_seek_traditional -->
```cpp
auto doc = R"({"name":"Alice", "sex":false, "age":25, "height":163.5, "weight":53.3})"_xyjson;

std::string name = doc / "name" | "";
bool sex = doc / "sex" | false;
int age = doc / "age" | 0;
double height = doc / "height" | 0.0;
double weight = doc / "weight" | 0.0;
```

但这在字段数量很多时并不高效，因为 Value 类的 `/` 操作每次都要从第一个
字段开始线性查找。改用迭代器的 `/` 操作就可以这样写：


<!-- example:usage_4_6_object_iter_seek_fast -->
```cpp
auto doc = R"({"name":"Alice", "sex":false, "age":25, "height":163.5, "weight":53.3})"_xyjson;

auto it = doc % "";
std::string name = it / "name" | ""; // it.seek("name") | ""
bool sex = it / "sex" | false;
int age = it / "age" | 0;
double height = it / "height" | 0.0;
double weight = it / "weight" | 0.0; // it.seek("weight") | "

// 等效于如下写法
it = doc % "";
name = *it | "";    ++it;
sex = *it | false;  ++it;
age = *it | 0;      ++it;
height = *it | 0.0; ++it;
weight = *it | 0.0; ++it;
// seek 方法查找成功后，迭代器位置停在它下一步
```

这样在访问全部 5 个字段时只扫描了 5 次，平均 1 次，O(1) 复杂度。而前面用
`Value` 类的 `/` 路径操作则共扫描了 (1+2+3+4+5) 次，访问所有字段的复杂度总共
O(N^2) 平均 O(N) 。

在这个特殊的例子中，访问键序与 Json 实际的键序完全相同，不多也不少，才相当于几
次 `++` 迭代器操作。但对象迭代器的 `/` 操作是有容错的，分为如下三种情况：

- Json 中有多余的字段，则在某次 `/` 访问时跳过即可，影响最小，全访问时额外的扫
  描次数仅等于额外的字段数量；
- Json 缺省字段，`/` 操作试图访问不存在的键，这需要全扫描一遍才能知道键不存在，
  但这也只影响这一次的查找，全扫描后发现不存在时迭代器会回到原位置，故是局部影
  响；
- Json 键顺序是乱序的，这就可能使多次 `/` 查找操作降级为线性扫描，影响最大，将
  回退为平均每次查找 O(N) 的复杂度。

所以，对象迭代器的 seek 方法或 `/` 操作，每次查找字段最好情况是 O(1)，最坏情况
是 O(N) 。但是在实践中，通过项目规范，如果大家都按照协议文档的字段顺序进行序列
化与反序列化（也可能由于业务逻辑省略写或省略读某些字段），那基本就能达到最好情
况，全访问每个字段时每个字段也只会被探测一次。

### 4.7 可写迭代器修改结点

很显然，利用可写迭代器解引用后的 `MutableValue` 可以修改当前元素。

<!-- example:usage_4_7_iter_modify -->
```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"({"name": "Alice", "age": 30})";

auto iter = mutDoc % "age"; // item = mutDoc / "age"
*iter = 25;                 // item = 25

std::cout << mutDoc << std::endl; // 输出：{"name":"Alice","age":25}
```

在上例中，用 `/` 符号代替 `%` 直接取结点也一样达成修改目标。但是当需要在循环中
使用 `/` 路径查找时，迭代器就更有效率了。

<!-- example:usage_4_7_iter_modify_batch -->
```cpp
yyjson::MutableDocument mutDoc;
mutDoc << R"([1,2,3,4,5,6])";

// 将每个元素增大为 2 倍
// 路径操作 / 需要每次从头查找
for (auto i = 0; i < +mutDoc; ++i) {
    mutDoc / i = (mutDoc / i | 0) * 2;
}
std::cout << mutDoc << std::endl; // 输出：[2,4,6,8,10,12]

// 使用迭代器批量修改
for (auto it = mutDoc % 0; it; ++it) {
    *it = (*it | 0) * 2;
}
std::cout << mutDoc << std::endl; // 输出：[4,8,12,16,20,24]
```

### 4.8 可写迭代器插入结点

在前文讲到，`MutableValue` 支持在末尾增加结点，如果想在数组或对象中间某个位置
插入，则可以先利用迭代器定位到所在位置，再用 `insert` 方法插入，操作符也是
`<<` 。例如：

<!-- example:usage_4_8_iter_insert -->
```cpp
auto doc = R"([1, 3, 4, 5])"_xyjson;
auto mutDoc = ~doc;

// 迭代器定位到 [1] 元素 3，在其前面插入 2，迭代器指向新插入元素
auto it = mutDoc % 1;
it.insert(2);
std::cout << (*it | 0) << std::endl; // 输出：2
std::cout << mutDoc << std::endl; // 输出：[1,2,3,4,5]

// 用 << 在 [3] 元素 4 前链式插入，迭代器位置不变
it = mutDoc % 3;
std::cout << (*it | 0) << std::endl; // 输出：4
it << 3.1 << 3.14 << 3.142;
std::cout << (*it | 0) << std::endl; // 输出：4
std::cout << mutDoc << std::endl; // 输出：[1,2,3,3.1,3.14,3.142,4,5]
```

使用方法 `insert` 与操作符 `<<` 有个细微差别。`insert` 与大多标准容器的插入行
为一致，插入后迭代器指向新插入元素。但是 `<<` 为支持链式插入的有序性，相对于
`insert` 插入会前进一步，由于 `insert` 是在迭代器当前位置前面插入的，`<<` 再前
进一步的话就回到当前位置了。

如果迭代器位于初始位置即指向 `[0]` 号结点，插入时就表示在容器最前面插入。如果
将迭代器移到最末尾的 `end` 无效状态（指向最后一个结点再进一步），也支持插入，
表示在容器的末尾追加，如果用 `insert` 方法，迭代器恢复有效，如果用 `<<` 插入，
迭代器保持无效。

可写对象迭代器的插入操作方法类似，不过 `insert` 方法要求两个参数，`<<` 操作符
要求一个 `KeyValue` 键值对参数，也支持将键、值结点依次用 `<<` 插入（需要在迭代
器内部状态保存一个临键结点的代价）。

<!-- example:usage_4_8_iter_insert_object -->
```cpp
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
auto mutDoc = ~doc;

// 当前 it 指向 age，在其前面插入 sex
auto it = mutDoc % "age";
it.insert("sex", false);
std::cout << -it << "=" << *it << std::endl; // 输出：sex=false
std::cout << mutDoc << std::endl;
//^ 输出：{"name":"Alice","sex":false,"age":30}

// 当前 it 指向 sex ，在其前面再插入 sex2:49
it << "sex2" * (mutDoc * 49);
std::cout << mutDoc << std::endl;
//^ 输出：{"name":"Alice","sex2":49,"sex":false,"age":30}

// 当前 it 仍指向 sex ，在前面再插入一个字符串类型的 sex
it << "sex3" << "spec";
std::cout << mutDoc << std::endl;
//^ 输出：{"name":"Alice","sex2":49,"sex3":"spec","sex":false,"age":30}
```

操作符 `<<` 为支持链接调用，返回值是迭代器本身引用。但 `insert` 方法不可链式调
用，其返回值是 `bool` 表示插入是否成功。数组迭代器插入几乎不会失败，除非内存不
足等不可解决的异常。对象迭代器插入，在键结点不是字符串不可作为键时会失败。
yyjson 对象允许重名键，所以即使插入已有键，也不会失败。

### 4.9 可写迭代器删除结点

通过迭代器定点删除，是插入的逆操作，方法名 `remove`，操作符 `>>` 。不过是用方
法名还是操作符，删除后迭代器都前进一步指向下一个元素。如果原来容器只有一个元素
，删空后的迭代器自然将处于无效状态，如果原来容器就为空，删除无效果，迭代器也保
持无效状态。

可写数组迭代器的 `remove` 方法返回 `MutableValue` ，表示原来被删除的结点，可写
对象迭代器的 `remove` 方法返回 `KeyValue` ，表示被删除的键值对。如果在无效状态
的迭代器上删除，返回值显然也是无效的。操作 `>>` 支持链式操作，返回迭代器引用本
身，右侧参数 `MutableValue` 或 `KeyValue` 保存被删除的结点。例如：

<!-- example:usage_4_9_iter_delete -->
```cpp
auto doc = R"([1,2,3,3.1,3.14,3.142,4,5])"_xyjson;
auto mutDoc = ~doc;

auto it = mutDoc % 3;
auto rm = it.remove();
std::cout << (rm | 0.0) << std::endl; // 输出：3.1

yyjson::MutableValue rm1, rm2;
it >> rm1 >> rm2;
std::cout << (rm1 | 0.0) << std::endl; // 输出：3.14
std::cout << (rm2 | 0.0) << std::endl; // 输出：3.142
std::cout << mutDoc << std::endl; // 输出：[1,2,3,4,5]
```

在用可写对象迭代器操作符 `>>` 作删除时，不能像链式插入那样依次输出键与值，只能
一次性删除保存到 `KeyValue` 临时值中。

<!-- example:usage_4_9_iter_delete_object -->
```cpp
auto doc = R"({"name":"Alice","sex2":49,"sex3":"spec","sex":false,"age":30})"_xyjson;
auto mutDoc = ~doc;

auto it = mutDoc % "sex2";
auto rm2 = it.remove();

// 在 it 处删除 sex2 后，自动移到下一个结点 sex3
yyjson::KeyValue rm3;
it >> rm3;

// 将删除的 sex2 sex3 字段重新插入到末尾
it.end(true);
it << std::move(rm2) << std::move(rm3);

std::cout << mutDoc << std::endl;
//^ 输出：{"name":"Alice","sex":false,"age":30,"sex2":49,"sex3":"spec"}
```

迭代器的 `remove` 方法或 `>>` 操作，可以删除最后一个元素，但不能在最末尾的
`end` 状态删除，因为那已经是失效状态，没有当前元素可删除的概念。另外提一点，
`MutableValue` 类在表示数组或对象时，也支持 `pop` 方法或 `>>` 操作符，用于删除
最后一个元素，但它也是借用迭代器实现的，但要注意这不是 O(1) 操作，而是 O(N) 操
作，因为必须先将迭代器移动到最后一个元素也即 `end` 前一个位置。迭代器移到
`end` 状态有 O(1) 的快速操作，所以在最后插入也是 O(1) 操作，比在最后删除更常用
更有用。

### 4.10 迭代器标准化接口

按 C++ 标准库的一般惯例，容器应该提供 `begin` 与 `end` 方法生成一对迭代器，据
此可适配标准库算法，以及支持使用范围 `for` 的语法糖。

如果把 Json 当作容器，比如 xyjson 封装的 `Value` 类，却有一个小问题，它在运行
时才能知道所表达的是数组还是对象，所以没法直接提供统一的 `begin` 与 `end` 方法
。只能分别提供 `beginArray` 与 `endArray`，或者 `beginObject` 与 `endObject`
方法，这就与标准库接口不适配了。

为了解决这个问题，从 `Value` 与 `MutableValue` 再分别派生两个子类，专门用于表
示数组或对象，然后在子类中就可以定义 `begin` 与 `end` 方法，适配标准容器。

- ConstArray: 只读数组，由 Value.array() 方法或 `| kArray` 转换；
- ConstObject: 只读对象，由 Value.object() 方法或 `| kObject` 转换；
- MutableArray: 可写数组，由 MutableValue.array() 方法或 `| kArray` 转换；
- MutableObject: 可写对象，由 MutableValue.object() 方法或 `| kObject` 转换；

这四个子类的类名，其实不并过多关注，一般也不会直接构造，而是从父类中调用方法向
下转换类型，当然运行时类型不匹配返回的是无效值。其主要用途是借此类型概念适配标
准用法，例如：

<!-- example:usage_4_10_standard_interface -->
```cpp
auto doc = R"([1, 2, 3, 4, 5])"_xyjson;

auto array = doc.root().array(); // doc.root() | kArray;
int sum = 0;
for (auto item : array) {
    sum += (item | 0);
}
std::cout << sum << std::endl; // 输出：15

// 或者直接使用标准库算法
sum = std::accumulate(array.begin(), array.end(), 0,
    [](int acc, const yyjson::Value& val) {
    return acc + (val | 0);
});
std::cout << sum << std::endl; // 输出：15
```

## 5 性能优化辨析与指南

在前文的教程中，也曾多处点出可以增加效率与性能的推荐用法，这里再者补充一些阐述
。

### 5.1 C++ 封装需要开启编译优化

在生产环境使用 xyjson 像其他大多 C++ 库一样，应该编译 Release 版本，开启至少
`-O2` 优化。C++ 的很多零成本抽象是建立在编译器优化的基本之上的，有优化与没优化
的 C++ 库的效率可能有相当大的差距。这不像 C 库，没那么多花招，Debug 版本也很快
，Release 版本的性能提升可能不像 C++ 那么明显。

据本项目的性能测试显示，在开启 `-O2` 优化下，使用 xyjson 的操作符写法，与对应
的原生 yyjson C API 用法相比，性能浮动在 5% 以内。

### 5.2 条件编译宏禁用不必要功能

在前文曾较详细介绍 yyjson 只读模型与可写模型的数据结构，可知只读模型在路径查找
操作上比可写模型要快得多。所以在项目实践中根据业务需求，尽可能优先使用只读模型
，只在有必要的时候转为可写模式。例如读入的 Json 只在满足某些条件时才需要修改，
而且有较大概率是不需要修改的，那就可先按只读模型处理。

如果在项目中能确定只有读 Json 的需求，没有修改 Json 的需求，或者说 yyjson 可写
模型的链表结构不适合某个具体项目，或单纯不想用 yyjson 的可写模式，那 xyjson 库
也提供了一个 `XYJSON_DISABLE_MUTABLE` 条件编译宏，可以完全禁用可写模式的
`MutableValue` 与 `MutableDocument` 类，这也能减少 `xyjson.h` 头文件一半以上的
源代码，提升编译速率。

在开启可写模型的支持下，还有另一个条件编译宏 `XYJSON_DISABLE_CHAINED_INPUT` 用
于控制 `MutableValue` 对象及其迭代器是否支持 `<< key << value` 这样的分开两次
插入键与值的键式操作。要实现支持该功能是有代价的，它会将 `MutableValue` 的大小
由两个指针大小增大到三个指针大小，增加了 1/3 的内存占用。更重要的是两个指针或
16 字节大小的类对象很可能是编译器优化的临界点，16 字节大小的类对象有可能利用寄
存器传参，所以这也对性能影响较大。如果性能敏感应用，或不喜欢这样使用（避免与链
式插入数组混淆），可以禁用该功能。

这两个条件编译宏可以在构建脚本中定义，也可以在源代码中 `#include "xyjson.h"`
之前手动定义。例如：

<!-- example:NO_TEST -->
```cpp
#define XYJSON_DISABLE_CHAINED_INPUT
#include "xyjson.h"
```

不要试图给这种条件编译宏定义 `0` 或 `1` ，不区分值，只区分有没有定义。

### 5.3 迭代器的加速与开销

我们常说，迭代器模式比普通循环快，那是基于一般的理论分析，所谓 `O(N)` 等时间复
杂度记号，那是在 `N` 足够大时必定成立的。但在实际的不同项目中，`N` 的规模到底
有多大却是不同的。在 `N` 比较小时，`O(N)` 未必比 `O(1)` 慢，甚至还可能更快，因
为实现 `O(1)` 的数据结构与算法往往更复杂，在小规模数据量下没有优化。

具体而言，在 xyjson 中，封装的迭代器类比 Value 类大得多，所以在对象的键数量不
多时，直接用 `/` 路径操作取字段可能反而比使用迭代器更快些。这个规模的临界值，
可能也与具体项目的应用场景不尽相同，真正性能敏感的应该最好作有针对性的具体测试
研究。据我所作的性能测试，在几十个元素大小的规模下，迭代器还没有体现性能优势，
仅供参考。

但是在小规模 `N` 的场景中，使用迭代器当然也不至于使性能恶化太多，所以仍然推荐
使用迭代器。一来迭代本身是种比较标准、比较优雅的设计模式，二来不清楚项目将来要
处理的 json 会不会变得更大，数组或对象容器的元素数量规模突然变大。

## 6 常见错误与陷阱

在使用 xyjson 过程中，一些常见的错误模式需要特别注意。

### 6.1 理解核心类的拷贝行为

两个 Document 类相当于独占型智能指针，不能拷贝，只能移动；两个 Value 类是代理
指针，可以拷贝，但拷贝的只是对底层结点的引用。迭代器是值语义，虽能拷贝，但体积
反而比前两者大，如无必要别拷贝。

<!-- example:usage_6_1_copy_behavior -->
```cpp
yyjson::Document doc1("{\"data\": \"value\"}");

//! 错误：尝试拷贝 Document
//! yyjson::Document doc2 = doc1;  // 编译错误

// 合法：使用移动语义
yyjson::Document doc2 = std::move(doc1);

// 合法：使用类型转换，拷贝
yyjson::MutableDocument mutCopy = ~doc2;
```

但一般也不建议移动 Document，建议尽量按引用传参调用子函数，或者作为另一个大类
的成员，在方法内共享，避免传参。当 Document 离开作用域后，从它用 `/` 路径操作
获取的 Value 类型值，都不可再使用，相当于野指针了。

对于只读 Value ，复制引用问题不大。但是对于 MutableValue，如果发生复制引用，要
注意通过一个引用修改了 Json 结点，其修改也会反映到另一个引用变量中。例如：

<!-- example:usage_6_1_mutable_value_ref -->
```cpp
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
auto mutDoc = ~doc;

auto ageNode = mutDoc / "age";
auto copyNode = ageNode;  // 实际复制的是对结点的引用

ageNode = 25;
std::cout << (copyNode | 0) << std::endl; // 输出：25

// 要真正复制结点，得调用 mutDoc 的 create 方法，或 * 操作符
// 新结点是游离在 json 树外的独立结点，除非挂载到树上的某个容器中
auto cloneNode = mutDoc * ageNode;
```

所以不建议长期持有 MutableValue 类型的值，避免跟踪与理解困难。

### 6.2 注意判断值与迭代器的有效性

使用 xyjson 不会主动抛异常，但是每个类包括迭代器都有 `isValid` 方法用于判断其
有效性，或者直接在 bool 上下文判断。在严肃的工程项目中，应该习惯性地判断每个值
的有效性，相当于指针判空。

有效性判断并不一定意味着要打断方便的链式操作，比如 `/` 路径操作取深层结点。只
要在中途某个 `/` 操作返回无效值（如键不存在），那无效值就会一直传染到最后，不
会主动报错抛异常，顶多浪费一些对无效值的 `/` 操作。只需要对最后的结果作有效性
判断。如果取叶结点的主要目的只是用 `|` 读取值，并且接受合理的默认值，那甚至也
可不对 `|` 左边的参数作有效性判断。

但是，当发现代码没有按预期工作，需要调试时，很可能就需要打断链式操作，逐步检查
每个中间值是否有效。所以在严肃项目中，还是推荐习惯性检查值的有效状态，除非性能
热点部分，不想在重复判断上浪费操作。

### 6.3 索引操作符只建议放 = 左边

由于 `[]` 操作符在索引对象字段时，会自动插入不存在的字段，建议只放在 `=` 左边
确实想增加字段的情况，不要放在右侧无意间插入新字段。

<!-- example:usage_6_3_index_operator -->
```cpp
yyjson::MutableDocument mutDoc("{}");

//! 错误：用 / 操作符不会创建新字段
//! mutDoc / "new_field" = "value";

// 正确：用 [] 操作符创建新字段
mutDoc["new_field"] = "value";
mutDoc / "new_field" = "updated";  // 修改已存在字段

//! 错误：会自动添加 "no_field" 字段，类型为 null
int i = mutDoc["non_field"] | 0;
```
### 6.4 避免对可写容器同时操作两个迭代器

可写数组或对象的迭代器，支持对容器作插入与删除操作，在这些操作中能维持这个迭代
器的有效性状态。但如果同时持有两个可写迭代器，通过一个迭代器作增删操作，另一个
迭代器就无法感知这种修改，那就会有问题。例如：

<!-- example:usage_6_4_iterator_safety -->
```cpp
auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
auto mutDoc = ~doc;

// 通过一个迭代器 wit 在对象中插入了一个键
// 另一个迭代器 rit 在读取时的结果难以预料
auto rit = mutDoc % "";
auto wit = mutDoc % "age";
wit << "sex" << false;
```

其实这就是只允许一个可写引用的原则。不过对于只读容器的迭代器，并发读没有问题。

### 6.5 管道函数链式操作限制

管道函数一般不支持链式操作：

<!-- example:NO_TEST -->
```cpp
//! 错误：尝试链式管道操作
//! auto result = value | f1 | f2 | f3;  // 编译错误

// 正确：在一个 lambda 中完成所有处理
auto result = value | [](auto v) {
    return f3(f2(f1(v)));
};
```

其实如果管道函数返回类型也是 Value 或 MutableValue ，那就可以继续管道。如果返
回其他类型，能否继续接管道取决于返回类型（及后面类型）的实现。

对于 `|` 操作符的退化行为，读取并返回基本类型，那肯定也不支持继续链式 `|` 操作
。

