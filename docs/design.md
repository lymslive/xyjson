# xyjson 操作符重载设计思路

本文档介绍 xyjson 库的设计思路，尤其是操作符重载的选择考量。

## 1 Json 数据在 C++ 程序中的基本操作

C/C++ 有很多 json 库，各有优缺点与侧重点，很难像其他一些语言一样有个统一的标准
json 库。但不管哪个 Json 库，在涉及 Json 数据处理时，大约都要考虑如下一些基本
操作：

1. 解析 json 串，转化为一种内存模型或数据结构，基本都是一种 DOM 树形结构，这也
   是由 json 本身的逻辑结构决定的。
2. 从根结点开始查找、获取 json DOM 树中的某个特定结点。
3. 将 json 结点的统一类型，转为 C++ 原生数据类型，参与 C++ 程序内后续计算；json
   的叶结点是个简单标量，可对应转换为 C++ 各种数字与字符串类型；json 复杂之处在
   于数组与对象容器，却没有合适的原生类型与之对应，强行转为标准库容器也是低效不
   值当的，但是 json 库一般会提供某种迭代 api ，循环地从 json 容器中取出其子结
   点。
4. 如果要修改 json ，则要考虑另一套反向操作，将 C++ 的原生基本类型转为 json 结
   点，再将其添加到其他 json 容器结点，构造恰当的层次结构。
5. 将 json 在 C++ 程序的内存模型，序列化为 json 串，对外输出，与外界交互。

大多 json 库在比较与宣扬其性能时，一般只着重于第一步与最后一步的解析与序列化。
但其实只有中间三步才能完成特定的业务工作，只将 json 解析与输出等于没干实事。

xyjson 并不是一个独立的 json 库，它是基于 yyjson 这个高性能 C 库的一种 C++ 风
格封装的单头文件库。首尾两步的 json 解析与序列化委托 yyjson 底层库完成，xyjson
更关注中间三步的易用性与高性能的平衡。

## 2 查找 Json 结点的路径操作 `/`

以一个简单的 json 数据为例：

```json
{
    "user": {
        "name": "Alice",
        "age": 25
    }
}
```

假设要找到 `user` 下面的 `name` 字段（结点），逻辑上需要以下几步：

- 从表示 json DOM 的数据结构中获取根结点
- 在根结点下面找到 `user` 结点
- 在 `usr` 下面找到 `name` 结点

简单说，有几层嵌套就需要有几层逻辑操作。有些 json 库不一定有独立的 DOM 抽象，
直接使用 json 结点类型，那也只减少一层操作，不影响大局。

如果用 yyjson 的 api ，做这几步操作的代码大致是：

<!-- example:design_2_yyjson_obj_get -->
```cpp
// 假设源 json 串存于 strJson 字符串变量
yyjson_doc* doc = yyjson_read(strJson.c_str(), strJson.size(), 0);

yyjson_val* root = yyjson_doc_get_root(doc);
yyjson_val* user = yyjson_obj_get(root, "user");
yyjson_val* name = yyjson_obj_get(user, "name");
```

除去第一行解析 json 外，这三行代码已经是极简写法了。正常 C++ 程序员看到指针，
可能都还要忍不住要做判空的错误处理，如果每步都再加个 `if` 判断，那代码量就多起
来了，却仅仅为取得一个 json 的第二层子结点。

事实上，就 yyjson 的大部分 api 而言，不加判断地传入空指针也没事，例如
`yyjson_obj_get` 它内部会判断参数为空，快速返回空指针结果，那也是合情合理的。

但有些 C++ json 库，可能使用异常的设计，尝试对非对象的 json 取字段会抛异常，尝
试取对象不存在的字段也会抛异常，那就需要每步都做安全判断了，使得代码看起来非常
冗余。

而在 xyjson 怎么做的呢，它重载了除法 `/` 操作符，上例的三步操作可以写成一个“
连除”表达式：

<!-- example:design_2_xyjson_path -->
```cpp
yyjson::Document doc(strJson.c_str(), strJson.size());

// 下行 auto 推导为 yyjson::Value ，可以且建议直接写出
auto name = doc / "user" / "name";
```

将 `/` 符号重新称呼为路径操作，这应该是比较自然、符合直觉的，因为它已在计算领
域广泛使用：

- Linux 的文件系统使用 `/` 分隔路径，表示父子、包含关系；
- Json Pointer 标准也使用 `/` 表示 json 结点的路径；

在一些脚本语言中处理 json ，也许还可以写成更简单的表达式：

```js
doc = { ... };
name = doc.user.name;
```

命令行工具 `jq` 也使用 `.` 来表示 json 路径（可能为了避免使用 `/` 与文件名路径
混淆，毕竟命令行参数输入文件名也很常见）。

但是 C++ 是强类型语言，`.` 访问的成员名要在编译期确定，而且 `.` 不可重载，所以
只能重载 `/` 为表示路径操作，我觉得也足够合理了。

在一些 C++ Json 库中，支持 `[]` 索引操作符来取子结点，这个索引操作是在标准容器
中常用的。为沿袭这种习惯，xyjson 也支持 `[]` 索引操作：

<!-- example:design_2_xyjson_index -->
```cpp
yyjson::Value name = doc["user"]["name"];
```

就这个示例，对于只读 Json 而言，`/` 与 `[]` 是等价。但是 `[]` 也沿袭用于
`std::map` 的语义，会自动插入对象原不存在的字段（默认 `null` 类型）。这个特性
功过参半，因为可能在查找子结点时意外插入 `null` 结点。所以在 xyjson ，主推 `/`
路径操作符，`[]` 只建议单层索引写在 `=` 左侧明确是想新增结点的情况。

当然还有一种情况 `[]` 可能更简便，因为它的优先的与 `.` 相同，所以可以直接访
问方法；而 `/` 操作符优先级比 `.` 低，想调用方法必须加括号：

<!-- example:design_2_index_method -->
```cpp
std::string strName = user["name"].toString();
std::string strName = (user / "name").toString();
```

虽然 xyjson 的特色是操作符重载，但并不阻止直接调用具名方法，尤其是并不是每个操
作符都能做到像 `/` 这么符合直觉。

路径操作 `/` 的另一大特性是完全支持 Json Pointer 标准，也就是说，链式路径可以
写在一个字符串参数中，`/` 允许放在 `""` 里面：

<!-- example:design_2_path_pointer -->
```cpp
auto name = doc / "user" / "name";
auto age = doc / "/user/age"; // Json Pointer 标准要求以 / 开头
```

当然了，如果字段名是固定的、编译期已知的，使用分开写的链式 `/` 更有效率。合并
Json Pointer 写法的真正应用场景是访问路径只能在运行时确定，比如来自路径配置。

关于错误处理，路径操作 `/` 不会抛异常，如果字段不存在，将返回一个无效值。如果
中间产生无效值继续使用 `/` 操作也允许，仍然返回无效值。如果不放心，也可以显式
判断一下是否无效值，直接放在 `if` 条件中即可：

<!-- example:design_2_path_error -->
```cpp
auto user = doc / "user";
if (user) {
    auto name = user / "name";
}
```

这是因为也重载了 `operator bool` ，可以在条件上下文中直接判断。

`yyjson::Value` 类只封装了一个 `yyjson_val\*` 指针，在 `if` 中判断其实就是对该
指针判空。也由于只封装了一个 C 指针，再加上内联优化，xyjson 封装类的操作符重载
几乎能追求零成本抽象。就比如这个示例的链式路径操作，与依次调用 yyjson 的几个函
数其实是差不多的性能。

## 3 转换 Json 标量的取值操作 `|`

已知在 xyjson 中根据路径访问 json 结点的语法如下：

<!-- example:design_3_path_recall -->
```cpp
auto name = doc / "user" / "name";
auto age = doc / "user" / "age";
```

这里返回的类型是代表一个 json 结点的 `yyjson:Value` 类型。现在要从 json 结点类
型提取或转化 C++ 基本类型，语法可以是这样的，在斜杠 `/` 链最后写个扶正的竖线
`|` ：

<!-- example:design_3_pipe_getor -->
```cpp
auto name = doc / "user" / "name" | "";
auto age = doc / "user" / "age" | 0;
```

然后这里 `name` 返回的类型就变成了 `const char \*` ，`age` 返回的类型则是 `int`
，也就分别是 `|` 后面的参数类型。

说明一下，这里使用 `auto` 主要是为了演示语法对比，实践中对于简单类型的赋值语句
，不建议滥用 `auto` 。推荐显式写出 `name` 的与 `age` 的类型，而且它也不是只能
使用 `const char \*` 与 `int` ，凡是能兼容与隐式转换的类型都可以，如：


<!-- example:design_3_getor_convert -->
```cpp
std::string name = doc / "user" / "name" | "";
int64_t age = doc / "user" / "age" | 0;
```

再细看 `|` 右侧的参数，它其实起到两个作用：

- 表示它期望左侧 json 的类型，也是操作符的返回类型；
- 当左侧 json 无效或类型不符时，直接将右侧参数当作默认值返回。

所以在 xyjson 中，取值操作符 `|` 也可以读作“或”，事实上调用的方法名就是
`Value::getor` 。如果用 yyjson 的 C API 来写，大约是如下用法：

<!-- example:design_3_yyjson_getor -->
```cpp
// 假设已取的 json 结点指针 name 与 age
// yyjson_val *name, *age;

// const char* pszName = name | "";
if (yyjson_is_str(name)) {
    const char* pszName = yyjson_get_str(name);
}
else {
    const char* pszName = "";
}

// int iAge = name | 0;
if (yyjson_is_int(name)) {
    int iAge = yyjson_get_int(name);
}
else {
    int iAge = 0;
}
```

其他大多 C++ Json 库的操作也差不多，需要做类型判断与转换类型两步操作。xyjson
将这两步操作合并为一个 `|` 操作符，并且提供简单的错误机制，返回默认值。

也有些的 C++ Json 库使用类似 tuple 的 get 方法，使用模板类型参数，形如
`int iAge = json.get<int>();` 。然而我觉得业务代码中最好不要用模板，模板是给库
作者使用的。

xyjson 还从 `|` 操作符衍生扩展了其他几个相关的操作符。比如 `|=` 的复合赋值：

<!-- example:design_3_getor_assign -->
```cpp
// 假设已取得 yyjson::Value age 结点
int iAge = 18;
iAge = age | iAge;

// 假装 | 支持交换律
iAge = iAge | age;

// 再简化写成
iAge |= age;
```

复合操作 `|=` 在类型不匹配时能避免自赋值的开销，尤其在使用 `std::string` 类型时
可能提升性能。但是 `|` 与 `|=` 都还存在的个微不足道的问题，当它返回默认值时，
它无法区分到底是因为 json 类型不匹配呢，还是 json 中恰好存了这个默认值。在需要
关注这个问题并显式判断取值是否成功时，还支持使用 `>>` 操作符：

<!-- example:design_3_get_judge -->
```cpp
int iAge = 18;
if (age >> iAge) { // 不嫌麻烦还可以在 >> 前面加 age && 且条件连接
    // 使用 json 结点中的 age 值
}
```

当只需判断 json 结点的类型，而不需要取值时，可以使用与 `|` 相对的另一个符号
`&` ：

<!-- example:design_3_type_check -->
```cpp
if (name & "") {
    // name 是字符串类型，与 "" 相同
    std::cout << (name | "");
}
if (age & 0) {
    // age 是整数类型，与 0 相同
    std::cout << (age | 0);
}
```

一个基本原则是，当 `json & x` 返回 `true` 时，`json | x` 就能成功提取 `json`
的值，否则返回 `x` 默认值。

在大部分业务场景，并不需要为了使用 `|` 而先去判断 `&` ，直接使用 `json | x` 最
简洁。类型判断可用于 json 类型不规范、不确定的情况，例如：

<!-- example:design_3_type_dispatch -->
```cpp
if (age & 0) { // age.isInt()
    int iAge = age | 0;
}
else (age & "") { // age.isString()
    int iAge = ::atoi(age | "");
}
```

## 4 修改 Json 标量的赋值操作 `=`

如果认为 `|` 对应着 `get` 方法，那么 `=` 就对应着 `set` 方法。它可以将 C++ 的
一些基本类型直接赋给 json 结点，修改 json 的内容。当然，这只能用于可写 json 模
型：

<!-- example:design_4_mutable_set -->
```cpp
yyjson::MutableDocument doc(R"({"user": {"name": "Alice", "age":25})");

doc / "name" = "Bob;
doc / "age" = 20;
doc["active"] = true; // 用 [] 添加新结点
```

这没有特别意外，因为其他一些 C++ Json 库也支持 `=` 重载，比较符合直觉。

不过 xyjson 会处理两个特殊字符串字面量 `"[]"` 与 `"{}"` ，分别将其当作空数组与对象。

<!-- example:design_4_special_literal -->
```cpp
yyjson::MutableDocument doc;
doc["array"] = "[]";  // setArray
doc["object"] = "{}"; // setObject()
```

C++ 没法像其他一些脚本语言一样直接用裸字面量 `[]` 与 `{}` 表示 json 数组与对象，
只好退而求其次，用加引号的特殊字符串字面量来表示。但也仅限字面量，如果字符串变
量存个 `"[]"` ，却不会当作数组，仍当字符串处理。

## 5 添加 Json 容器的插入操作 `<<`

前面介绍的 `|` 与 `=` 操作符的读写，主要是针对 Json 叶结点标量的。而 json 能成
为通用数据格式的表达力，来源于它提供的两种容器，数组与对象。于是对 json 容器增
删元素或子结点，是另一块重要操作。

xyjson 使用插入操作符 `<<` 来形象地表示向容器添加子结点，例如：

<!-- example:design_5_container_insert -->
```cpp
yyjson::MutableDocument doc;
doc["array"] = "[]";
doc["object"] = "{}";

doc / "array" << 1 << 2 << 3;
doc / "object" << "first" << 1 << "second" << 2 << "third" << 3;
//^ 结果：{"array":[1,2,3]","object":{"first":1,"second":2,"third":3}}
```

其中，向对象容器添加元素的链式插入操作有点特殊，它要求键与值交替插入。上例是为
了演示链式插入的相似性，实践中建议每行表达式只用两个 `<<` 添加一个键值对，比较
不容易弄混。

对象的链式插入要求额外维护一个插入状态，这使得 `MutableValue` 类的大小从 16 字
节（64位系统的两个指针）扩充为 24 个字节，有一定的性能代价。但是 xyjson 也提供
了一个条件编译宏可以选择禁用该功能，默认是开启的。

如果禁用了对象链式插入功能，直接调用方法更为简洁，如 `object.add(key, value)`
或 `array.append(valeu)`，不过数组的链式插入仍然可用。

从容器中删除的逆操作，xyjson 选择了重载反方向的双箭头操作符 `>>` 。对于大多数
据结构，删除操作都比插入更麻烦。故删除须谨慎，xyjson 要求必须提供一个左值变量
放在 `>>` 右侧来接收被删除的子结点，用法如：

<!-- example:design_5_container_remove -->
```cpp
// 接上例 doc = {"array":[1,2,3]","object":{"first":1,"second":2,"third":3}}

yyjson::MutableValue a1, a2, a3;
doc / "array" >> a3 >> a2 >> a1;

yyjson::KeyValue 01, 02, 03;
doc / "object" >> o3 >> o2 >> o1;
```

如果被删除的子结点真的没用了，后面不管它就是，或者也可以用个无意义变量名如 `\_`
来接收。当然被删除的结点可以重新添加到（同一个文档树）的另一个容器中，或者重新
插回原来的容器中，例如，将上例删除的元素逆序插回去：

<!-- example:design_5_reinsert_back -->
```cpp
// 接上例 doc = {"array":[],"object":{}}
doc / "array" << a3 << a2 << a1;
doc / "object" << o3 << o2 << o1;
//^ 结果：{"array":[3,2,1]","object":{"third":3,"second":2,"first":1}}
```

不过 xyjson 的最终实现，在将 `KeyValue` 插回对象时，要求右值的移动语义，避免将
同一个结点重复插入不同对象中造成所有权混乱，故上例的 `<<` 右侧参数还要加
`std::move` 转为右值。

另外注意，对 MutableValue（可写 json 结点类型） 的增删操作，都是在容器尾部进行
的，yyjson 的对象结构也是线性有序的。如果想在中间位置插入或删除，可以先从容器
创建迭代器，定位到所需位置，再将迭代器放在 `<<` 或 `>>` 操作符左侧即可。

此外，`<<` 与 `>>` 也用于 json 文档对整体的输入与输出，也即反序列化（解析）与
序列化，例如：

<!-- example:design_5_document_inout -->
```cpp
yyjson::Document doc;
doc << R"({"array":[1,2,3]","object":{"first":1,"second":2,"third":3}})";

std::string output;
doc >> output;
```

除了字符串，也支持将文件（`FILE\*` 或 `std::fstream`类型）放在 `doc <<` 与
`doc >>` 右侧参数。

据说，`<<` 是 C++ 程序中被重载最频繁的操作符号，想来此言不虚。

## 6 访问 Json 容器的迭代操作 `%`

迭代器可谓是操作容器的一种标准设计模式，在 C++ 标准库中就广泛使用，以致在 C++
中都不必感觉这算是设计模式。

要使用迭代器，先从容器创建迭代器开始。xyjson 选择 `%` 来表示该功能，因为 `%`
的原意是取模，与除法 `/` 相关的概念。而在 xyjson 中，`/` 被重载为路径操作符，
那是一种找子结点的方法，而迭代器是找子结点的另一种（可复用）方法，所以用变形的
`%` 来表示迭代器。另一种解释是 `% n` 取模会在 `[0, n-1]` 区间循环，而迭代器也
主要用于循环。

总之，不管如何理解或联想，xyjson 就可用 `%` 来创建迭代，并且支持两种参数来创建
两种迭代器，整数表示数组迭代器，字符串表示对象迭代器。例如：

<!-- example:design_6_iterator_ops -->
```cpp
yyjson::Document doc;
doc << R"({"array":[1,2,3]","object":{"first":1,"second":2,"third":3}})";

// (auto it = doc["array"].iterator(0); it.isValid(); it.next())
for(auto it = doc / "array" % 0; it; ++it) {
    // *it 代表每个子结点：1, 2, 3
    *it == it.value(); // yyjson::Value
    +it == it.index(); // size_t
}

// (auto it = doc["object"].iterator(""); it.isValid(); it.next())
for(auto it = doc / "object" % ""; it; ++it) {
    // *it 也代表每个值结点：1, 2, 3
    *it == it.value(); // yyjson::Value
    +it == it.index(); // size_t
    -it == it.name();  // const char*
    ~it == it.key();   // yyjson::Value
}
```

在上例所示的迭代器标准 `for` 循环中，循环头三部分涉及三个操作重载：

- `for` 第一部分初始化，用 `%` 创建迭代器，由参数决定迭代器类型与初始位置，也
  即允许 `% 1` 或 `% "second"` 表示从第二个元素开始迭代。
- `for` 第二部分循环条件，因迭代器类重载了 `operator bool` ，故可直接当作条件
  判断，如果觉得裸变量太孤单，也可写成 `!!it` 或 `it.isValid()`。
- `for` 第三部分后处理，就是经典的 `++` 自增操作，表示迭代器 `next()` 方法语义。

在循环体部分，还支持使用另外几个一元操作符：

- `\*it` 是标准的迭代器解引用，它返回代表 json 结点的 `yyjson::Value` 或
  `yyjson::MutableValue` 类的值（不是引用），也支持 `->` 重载，`it->method()`
  等效于 `(\*it).method()`。
- `+it` 在数组迭代器中，表示当前索引下标，对象迭代器也同样有当前索引的概念，因
  为 yyjson 的对象也与数组一样是线性结构，只不过对象迭代器可能很少用到索引。
- `-it` 在对象迭代器返回键名字符串，数组迭代器的该方法返回 `nullptr` 。
- `~it` 在对象迭代器中返回键节点。有一些 C++ json 库的对象迭代器指向某个表示键
  值对的中间类型，但 xyjson 的对象迭代器采用与数组迭代器一致的表示，`\*it` 也
  指向（根据键名找到的）值结点，如果需要用到值结点，可用取反操作 `~it` 获取。

事实上，后面这几个一元操作符，在表示结点类的 `Value/MutableValue` 与表示文档树
的 `Document/MutableDocument` 类中也有重载，其表意有一定的关联性。

- `\*Document` 表示取文档树的根结点，也是返回 `Value` 类型，因为很多时候对文档
  的操作，就是对其根结点的操作。
- `~` 也表示在只读 `Document` 与可写 `MutableDocument` 之间的互相转换，读写模
  型分离是 yyjson 库的一个特色，其他大多 json 库只维护一种数据结构模型。
- `+Value` 表示将任意 json 结点转为整数使用，返回 `int` ，特别地，会对字符作
  `atoi` 转换，对容器类型就取其 `size()` 大小。
- `-Value` 表示将任意 json 结点转为字符串表示，返回 `std::string` ，也相当于序
  列化了。

插个话，重载一元 `+` 的灵感来源，是以前看到某文章提及为 `enum class` 重载一元
`+` 操作符转整数，代替 `static_cast` 使用方便太多。所以我也想到用 `+json` 来解
释任意 json 类型在整数上下文的意义。然后想到整数与字符串是最常用最重要的两种基
本类型，所以用另外相对应的一元 `-` 表示将任意 json 类型转字符串操作。前缀 `-`
也常用于命令行参数，而命令行参数都解析为字符串。

一元 `+` 与 `-` 的操作，不同于此前介绍的 `| 0` 或 `| ""` 严格要求 json 是整数
类型或字符串类型。在 C++ 程序中一般推荐使用强类型，但泛化地强转整数与字符串也
有它适用的场景。

## 6 总结

本文介绍了 xyjson 中最关键的一些操作符用法及其重载的设计思路与原由，详细文档可
再参考 [用户指南](usage.md) 与 [api 手册](api.md) 。xyjson 的愿景是希望在 C++
程序中处理 json 数据时能像处理基本类型一样使用各种操作符来表达 json 的独有操作
，甚至达到像计算机程序最初处理数学符号那样简洁，使其在形式上尽量独立于具体
json 库的繁琐 api 调用。当然也要选择一个高性能的底层 json 库，以及通过合理的封
装，尽量减少这种操作符抽象的开销，使其仍然保持高性能。

