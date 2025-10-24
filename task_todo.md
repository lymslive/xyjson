# 项目开发原始需求管理

本文档由人工维护，**AI受限编辑** ，仅允许在每个 `## TODO` 下加个子标题 `### DONE` 关联 `ai_request.log.md` 的完成任务ID记录。

需求ID 格式：`TODO:YYYY-MM-DD/n` ，日期/编号。日期表示头脑风暴冒出该需求想法的
日期，若一天中产出多个想法则加编号。仅有初步概念但未细化的需求暂不加日期 id ，
先快速列出 `TODO: 标题`，后面有细化内容时再赋日期 id ，转为正式需求。

## TODO:2025-10-12/1 拆分单元测试文件

头文件 include/xyjson.h 可以只有一个，但测试文件不宜全塞在一个
utest/t_xyjson.cpp 文件。

拆分原由：
- 增加单元测试文件维护性
- 多个源文件编译单元可检测出头文件中遗忘内联的函数定义

初步拆分方案：（建议）
- t_experiment.cpp: 开发中实验性测试代码
- t_basic.cpp: 基本用法
- t_readonly.cpp: 测试 yyjson 只读模型封装
- t_mutable.cpp: 测试 yyjson 可写模型封装
- t_iterator.cpp: 测试迭代器访问功能
- t_misc.cpp: 其他不适合上述分类的测试

单元测试用例名调整，去掉原来统一的 `yyjson_` 前缀，每个文件内的用例名加上文件名
前缀，但不需包含 `t_`。如果适合，也允许将原来的一个测试用例拆成多个划分到相同
或不同的文件中。

在 utest 目录增加一个 `readme.md` 文件，简要说明各测试文件的用意。还可再加入测
试命令用法。方便后面增加、修改测试用例等维护工作参考。

拆分后只涉及 cmake 构建脚本修改，不该影响功能的回归测试。

### DONE:20251012-121900

备注：
制定拆分方案写入 `doing_plan.tmp/` 后，由 AI 拆分消耗 token 比较大，也较慢，可
能还不如手工复制粘贴快。

## TODO:2025-10-12/2 头文件结构规划

期待目标是将 xyjson 做成单头文件的 head-only 库，因此 xyjson.h 头文件将会比较
庞大，需要规划一种文件结构，用特殊注释格式将单文件作逻辑划分。

拟分为三级：
1. `@Part 1: 简明标题`，下一行长分隔线 `=`
2. `@Secion 1.1: 简明标题`，下一行长分隔线 `-`
3. `@Group 1.1.1: 简明标题`，下一行长分隔线 `*`

一般地，在上一级的内容比较长时再考虑划分下一级。

文件结构化注释采用 C 风格单行注释 `/* text */` ，且在星号前后留一个空格。
标题下一行是整行分隔线如 `/* ==..72..== */` ，按 Vim 习惯一行 78 字符宽度，除
去首尾注释标志与空格，中间应是 72 个分隔符。标题行注释本身左对齐，不填充额外分
隔符，该多短就多短。

文件末尾加个目录表，Table of Content ，记录每级标题所对应的行号。
目录也采用连续的 C 风格单行注释，78 字符宽，行号右对齐。

目录表有三个可选位置：
- namespace yyjson 前面
- namespace yyjson 后面
- 单独的文档写目录表

虽然我的第一想法是放在头文件的 namespace yyjson 后面，但这可以再议。请从代码可
读性及 AI Agent 方便读取掌握代码结构等角度分析放哪里更好。

### DONE: 20251012-22000

## TODO:2025-10-12/3 常用命令 make 配置集成

本项目使用 cmake 作为构建系统。但我想在根目录再放个简单的 makefile ，用于收集、
配置在开发中常用命令，简化命令输入，包含调用 cmake 做真正的构建。

期望 makiefile 包含如下伪目标：
- build: 调用标准构建命令，进入 build 子目录执行 cmake 与 make
- test: 依赖 build ，再执行 ./build/utxyjson --cout=silent
- insall: 依赖 build ，进入 build 子目录执行 make install 安装
- clean: 删除 build 构建目录，以便重新构建
- toc: 调用 script/gen_toc.pl 同步头文件目录，回答 y 跳过交互
- help: 打印可用伪目标的 make 命令及简要描述
- 默认目标：无参 make 也相当于 make help

### DONE:20251013-095233
后期补充：
增加 build/fast 与 test/fast 目标，可跳过重复 cmake .. 命令
使用 touch 技术标记时间戳可以书写伪目标依赖，不必 /fast 额外目标

## TODO:2025-10-13/1 完全 head-only 化之一

目标是将 src/xyjsion.cpp 的方法实现全部移到 include/xyjson.h 头文件中。
现在还有三块代码放在 cpp 文件中：

- Document/MutableDocument 的 read/write 方法，最初觉得 doc 类使用远不如 value
  结点类使用频繁，就把它们放在 cpp 文件了。
- Value/MutableValue 的 toNumberCast 方法 ，用于将一个 json 标量结点转为整数使用
  ，实现代码较长，当初觉得不宜内联放在头文件。
- Value/MutableValue 的 pathex 方法，用于解析 json pointer 风格的多层路径，也
  比较长的实现。

其中第一块的代码比较容易迁移，都是一些较小的函数，可以拷回头文件加上 inline 前
缀，分别置于 Section 4.2 与 Section 4.4 的位置，额外加次级 Group .

修改 xyjson.h 后，可执行 make toc 命令同步更新目录。

### DONE:20251013-110656

## TODO:2025-10-13/2 完全 head-only 化之二

两个类的 toNumberCast 方法实现很类似，增加一个非类模板函数，模板类型参数适配
Value 或 MutableValue 类，然后在各自的 toNumber 类中调用新的模板函数。

新的模板函数也可命名为 toNumberCast ，而将原来的 toNumberCast 方法删去。

### DONE:20251013-140710

## TODO:2025-10-13/3 完全 head-only 化之三

对于 pathex 方法，也可弃之不用，yyjson 库支持 json pointer ，可直接调用，不必
自己实现。但是 json pointer 标准要求参数必须以 `/` 开头，这将稍微改变原功能。

上层 pathto(const char* path) 方法调整实现逻辑：
- 如果参数为空，返回自身
- 如果参数以 `/` 开头，按 json pointer 处理
- 否则按单层 index 处理
- 删去 pathex 方法及其调用

可能会破坏原单元测试个别断言，须纠正单元测试适应新功能。
也需要专门增加用例测试 pathto 方法与 `/` 操作符对于单层 index 与多层 pointer
的场景。

yyjson 按 json pointer 取值的 API 有：
yyjson_ptr_get yyjson_ptr_getn yyjson_mut_ptr_get yyjson_mut_ptr_getn 
开发环境一般安装于 /usr/local/include/yyjson.h

如此，xxjson.cpp 的代码功能全部返回头文件，该 cpp 可删。
cmake 也要相应调整，xyjson 目标不再需要编译为静态库，是纯粹的 head-only 库。
安装也只需要头文件。不想用 cmake 系统安装的用户也可简单拷贝 xyjson.h 头文件到
其他项目中与 yyjson.h 一起使用。

### DONE:20251013-174921

## TODO:2025-10-13/4 完善项目规范以及与 AI 协作方式

首先要将原 `ai_request.log.md` 改名为 `task_log.md` ，与本文档名 `task_todo.md` 相呼应。
`task_todo.md` 是从用户角度记录的原始需求，需求 ID 格式是 `YYYY-MM-DD/n`, 日期加序号。
另一个 `task_log.md` 是由 AI 记录的任务完成日志, 任务 ID 格式是 `YYYYMMDD-HHMMSS` ，日期加时间。

与 AI Agent 协作的一般流程可能是：
- 用户产生想法，在 `task_todo.md` 中记录原始需求；需求 ID 取自记录日期，当天产
  生多个想法时附加序号；没有赋 ID 的视为非正式需求；
- 在给 AI 提示词中带上需求ID（TODO ID），让 AI 分析需求并实施，在交互中可能再
  补充细节；
- AI 完成任务后，在 `tast_log.md` 中记录日志，从完成时间生成任务 ID ；同时在
  `tast_todo.md` 文档中同步关联该任务 ID，在原来的 `TODO` 二级标题加一行子级标
  题，附前缀 `DONE`。
- 最后提交 git ，提交消息也由 AI 辅助生成。

git 提交消息遵循常见规范，首行类似 `<type>(scope): 简短描述` 格式，前缀用英文
单词，描述以及正文用中文。`type` 常见的如 feat fix docs refactor style perf
test chore 等。

*由 AI 提交的 scope 填需求 ID *

当有时没有在 `task_todo.md` 中预记录原始需求就直接与 AI 交互完成任务时，工作完
成告一段落后仍就在 `task_log.md` 中记录日志。然后 git 提交消息的 scope 可留空
，或选个合适的英文单词，这也容易与数字格式的需求 ID 区分开。

有时用户可能作微小调整，比如对 AI 完成的工作不完全满意，或在给 AI 提需求ID之前
确保代码、文档等已提交，防止意外事故。这样的人工提交时的 scope 也可留空。总之
带需求ID的提交相当于主提交，但为灵活起见，也允许不带需求ID的辅提交。

文档（包括git提交消息）用中文，但是代码注释使用英文。代码注释以简约为主，不求
详尽，不必为每一个函数加注释，很多相关的重载函数统一按组加注释说明。

### DONE:20251014-010159

## TODO:2025-10-14/1 优化单元测试中对静态字符的比较断言

xyjson 以及 yyjson 对字符串字面量有专门的性能优化考量，因为它具有静态生命周期，
可以安全引用，在添加到 json 结点时不必拷贝，包括值结点，与 object 的键名结点。

单元测试框架 couttast 原来提供的 `COUT` 宏，在接收两个 `const char*` 参数时比
较指针，这能很好地断言字符串字面量没有被拷贝。但是 `COUT` 比较指针可能有点反直
觉，容易误用，所以后来 couttast 库更新了，使 `COUT` 在接收 `const char*` 参数
时比较串内容，而另外提供了 `COUT_PTR` 显式用于比较字符串指针值。

couttast 升级后，并没有破坏本项目的单元测试，因为原来相同指针的字符串按内容比
较也肯定是相同的，所以回归测试仍通过。但这断言不够准确、严格，因此需要将原来测
试意图比较字符串指针的 `COUT` 语句改为 `COUT_PTR` 。影响范围 `t_mutable.cpp`
，在修改可写 json 模型时才有字符串字面量优化。

to AI:
- 分析 `utest/t_mutable.cpp` 的单元测试用例代码，找出意图比较字符串字面量指针
  的 `COUT` 语句，汇报相应的行号，输出结果至 `doing_plan.tmp/` 子目录。
- 编写一个简单脚本，将测试文件对应行号的 `COUT` 批量修改为 `COUT_PTR` 。
- 编译、测试，若有因本次修改断言失败，改回 `COUT` ，同时也记录回滚的行号。
- 除了修改 `COUT` 宏，不要做其他修改。

### DONE:20251014-082500

AI 处理该问题有点弱啊，只找出四个。后来我自己改了十几处。

## TODO:2025-10-14/2 简化 Document 类不必持有根结点

从逻辑上讲，一棵 json 树的 dom 持有其根结点的所有权。但是 xyjson 只是对 yyjson
的浅封装，真正管理 json 结点包括根结点的是底层 C 结构体，不是 Document 类。所
以 Document 类不必保存一个 Value 成员表示根结点。当用户代码需要访问根结点时，
实时构造一个 Value 值就行。

而且 Document 存一个根结点 Value ，还可能引入同步根结点的麻烦。所以我决定删去
根结点成员，而 root 方法返回类型从 Value 引用改为 Value 值。所有涉及根结点维护
的代码要作相应修改，尤其删去 syncRoot 方法。

to AI: 请用 TDD 开发方法完成该需求，大致流程如下：

- 在 `utest/t_basic.cpp` 测试文件的最前面增加一个 basic_size 测试用例，断言当
  前项目中定义的各个类的 sizeof ，编译测试成功。
- 修改 Document 与 MutableDocument 的 sizeof 期望值仅为一个指针大小，测试失败
  。
- 删去 Document 与 MutableDocument 的 `m_root` 成员，解决编译错误及必要优化，
  回归测试成功。

### DONE:20251014-121311

## TODO:2025-10-14/3 优化 Document 一元操作符设计

现状：一元操作符 + - 在 Value 类用于表示转换整数与字符串这两类主要信息，而
Document 的一元 `+` 操作表示取根结点，功能上不一致。

期望：Document 取根结点的操作符改为 `*` ，而 `+` 改为对根结点转整数

设计原因：现在二元 `*` 操作符（乘号）用于创建 MutableValue ，从
MutableDocument 取根结点也会返回一个 MutableValue ，用一元 `*` 操作符表达这个
功能有相关性。

其他不一致的地方，Document 类实现了 toString() 方法，但没实现 toNumber() 方法
。简化删除 Document::toString() 方法，需要用到它的地方改为调用 root 的同名方法
。一些操作符需要在 Document 与 Value 都定义，但具名方法没必要重复实现。

to AI:
- 将 Document 的一元操作符 `+` 改为 `*`, 并检查注释一致性
- 删除 Document::toString 方法，其调用者改为调用 root 的 toString 的方法
- 同步修改相关单元测试
- 将 Document 的一元操作符 `+` 重新设计为取根结点转整数的功能
- 为 Document 的一元 `+` 新功能加测试用例，可在已有测试用例中扩展一段或新加用
  例名，对常规 json 而言，`+` 应该是取根结点对象/数组的大小
- MutableDocument 作同样的修改
- 检查 docs/xyjson_operator.md 文档，作相应调整反映修改

### DONE:20251014-164224

## TODO:2025-10-14/4 开发脚本更新单元测试一览表

utest/README.md 末尾有一个单元测试一览表，内容主要由 `./utxyjson --List` 输出
，但要经过一些格式转换为 markdown 表格。为适应单元测试用例的扩张，需求写个脚本
来维护这个表格的同步更新。

to AI: 
请写个 perl 脚本，放在 script/utable.pl ，实现从 `./utxyjson --List` 到
当前表格格式的自动转换，可以在最左侧再加一列序号。

期望能自动更新 utest/README.md，如果不好定位位置，可以先在文档中加个隐藏注释行标记插入生成的文本。

然后在 makefile 添加一个快捷目标，类似 make toc 那样更新单元测试的目录表

### DONE:20251014-180019

## TODO:2025-10-14/5 完善类型判断的操作符设计

当前取值操作符 `|` 暗含类型判断，但没有专门的操作符只作类型判断返回 bool 值。
故设计与 `|` 相对应的 `&` 操作符用于判断类型，右侧参数与 `|` 相似接收 `0` 与
`""` 等类型代表值。当 `json & 0` 返回 true 时，`json | 0` 也保证能取到值。
从语义上讲，`&` 也可读作“且”，`|` 读作“或”，但是比真正的逻辑操作符 `&&` 和
`||` 省一个字符。

为此按惯例还要为 Value 入 MutableValue 增加一系列具名方法，可叫 `isType`，接收
一个参数，支持各种基本类型，判断是否与该类型相同，用不上实参。函数签名类似
`get` 方法，大致是 `bool isType(int) const` 等。

然后操作符 `json & 0` 的用法就相当于调用 `json.isType(0)` ，用模板函数实现。

还有另一个问题再考虑，像`0` 与 `""` 、`0.0` 这些类型代表值可定义为编译期常量，
给个名字，如 `kInt` 等。对于不喜使用魔数的用户可以写成 `json & kInt` 与 `json
| kInt`，语义更好些，只不过再加上命名空间略冗长，如何取舍由外部用户自行决定。

这些类型代表值的定义放在前面 `@Part 1` 部分。

此外再定义一系列常量，给本项目重载过的操作符命名。有些操作符在不同参数下有相当
不同的含义，则允许有多个名字。例如 `okExtract` 与 `okPipe` 的值都是 `"|"` 。前
缀 `ok` 可认为是 `operator const` 的缩写。

这些操作符名字常量在当前代码中可能还用不上，但在前面定义也可充当自描述文档。

这里涉及的新功能，可在 `t_basic.cpp` 文件中增加单元测试用例，包括使用类型代表
值常量。

补充需求：

isType 系列方法定义位置前移，放在已有各个具体的类型判断方法与 typeName 方法之后。

按已有惯例，特殊字面常量 `"{}"` 与 `"[]"` 可用于设置或插入空对象、空数组，所以
也应能类型代表值 kObject 与 kArray 。于是 `isType(const char\*)` 需要特殊处理
，除判断 json 字符串类型外，也能判断对象或数组类型。

数字类型判断要严格，整数与浮点不同，而且除常用 int 外，还要支持判断大整数
`int64_t` 与 `uint64_t` ，分别已有 isSint 与 isUint 方法参考。

### DONE:20251014-225417

## TODO:2025-10-14/6 取值操作符增加获取底层 C 结构体指针功能

目前取值操作符 `|` 可以从 json 结点提取基本类型。现在想再扩展功能，也支持提取
yyjson 底层 C 结构体指针，允许用户有特殊需要时仍可灵活使用它的 C API 。

按当前代码架构，应该只要增加 `get` 方法重载即可。请按
`bool get(int& result) const;` 系列方法惯例， 为
Value 类 增加重载 `bool get(yyjson_val * & result) const;` 
MutableValue 增加重载方法 `bool get(yyjson_mut_val * & result) const;`
以及 `bool get(yyjson_mut_doc * & result) const;`

是的，可写模型除了支持取 json 结点指针，也应支持取它所属的 doc 指针，它在修改
json 时是重要参数。

类型代表值常量相应地增加三个 kNode kMutNode kMutDoc 。

以及在 `t_basic.cpp` 文件中增加单元测试用例，获取底层指针用 yyjson API 作些简单操作。

### DONE:20251015-002058

## TODO:2025-10-15/1 项目文档优化方案

to AI: 仔细阅读现有文档，及 xyjson.h 注释，结合以下建议，深度优化文档撰写。各
个文档要有侧重，不要过多重复。

### 命名范调整

* docs/xyjson_userguide.md 改名 docs/usage.md
* docs/xyjson_operator.md  改名 docs/api.md
* 新增 docs/design.md
* 新增 README-en.md

实施中先不改名，而是参考原文档修改优化写入新文档，新文档完善后再删除旧文档。

README.md 先优化中文版，然后翻译一份 READE-en.md 英文版。在这两个版本的
README 头部要有显著标志互相切换的链接。

README 正文在合适的位置上要有 `docs/` 相关文档的链接；以及另外两个与开发相关的
文档 `task_todo.md` 与 `task_log.md` 文件的链接。

### README.md 优化建议

增加简述项目命名意思：一是与 yyjson 极似，依赖，致敬；二是 x y 常用于数学符号
，xyjson 通过操作符重载追求像数学变量符号一样操作 json 数据。

依赖项提前，给出 yyjson 与 couttast 依赖库的链接，后者注明可选仅测试与开发依赖。

由于纯头文件库支持免安装，先给出最简示例，再谈 cmake 构建，集成安装。

项目暂无 examples/ 子目录，但文档中可预留并注明 todo 待添加。或者说在 utest/
子目录中以测试用例的方式给了较详尽的示例。

在 README 中适当减少用例数量，用例中用注释说明结果。给出 usage.md 链接看更多用
例，usage.md 的用例不必太多注释结果，如有必要可在用例后的正文描述结果。

### usage.md 优化建议

从库使用者的角度，详述如何利用 xyjson 库解决实际业务中常见的 json 处理问题。

先要简述底层 yyjson 库只读与可写模型的思想，xyjson 的封装特点是操作符重载，但
也都有对应的具名方法可用。然后大致按以下顺序讲叙：

* 只读模型的基本操作，也适于可模型
  - 读入、解析 json 文档
  - 访问 json 结点
  - 读取值
  - 类型判断与错误处理支持
  - 重新序列化，转字符串输出
* 可写 json 模型操作
  - 修改值，重新赋值
  - 数组插入新结点
  - 对象插入新结点
  - 删除结点暂不支持，未封装
* 迭代器（四种）
  - 创建迭代器
  - 迭代器自增步进
  - 迭代器跳跃 seek
* 高级用法与其他未写到的用法
  - 一元操作符
  - 比较操作，深度比较
  - 管道函数自定义转换
* 性能优化实践
  - 保存中间结点，避免重复长路径索引
  - 遍历时改用迭代器
  - 安全字符串如何用避免拷贝，xxxRef 方法，字面量用操作符自动优化
* 常见错误
  - Document 类不可拷贝，Value 可拷贝，拷贝的是底层 json 结点引用
  - 路径操作符 `/` 只读，不能自动添加结点，`[]` 可以添加结点
  - 对非标量叶结点取值 `|` 无效
  - 输入操作符 `<<` 不对同类型结点有不同意义，业务上勿混用。
* 未实现的功能
  - 没有对 yyjson 全部功能封装
  - 可以获取底层指针，直接使用原生功能

### api.md 优化建议

作为查询手册，需要详尽、务实、忌浮夸。

开头部分先用列表格式列出核心类，再建操作符总览表。
总览表在当前基础上再加两列，操作符适应的类，及第二参数类型。
列增多后，描叙部分不能太长，如涉及过长描叙，可加脚注后表格后补充。

另要注明操作符重载不能改变优化级与结合律。

然后按操作符分节，讲叙每个操作符的意义与用法。示例代码都采用两份，先用操作符版
本，再用对应的方法名版本，但分开作为相邻的两个代码片断。其他非 api 细节的内容
更适合 usage.md 。

每个操作符一节（二级标题），同一个操作符在不同类或语境下有较大的意义区别，再加次级标题。

大致顺序大纲：

* `/` 路径操作符，支持 json pointer
* `|` 取值操作符；自定义函数管道
* `|=` 取值操作符自赋版
* `&` 类型判断
* `[]` 单层索引，可写对象能自动添加键
* `!`  错误判断，类有效性
* `==` 相等性判断
* `!=` 不等性判断
* `=` 类的标准赋值；基本类型赋 json 结点
* `<<` 反序列化输入文档；容器结点添加值；标量结点赋值；标准流输出
* `>>` 序列化输出
* `*` 二元乘，创建 json 结点；一元解引用，取根结点
* `+` 一元正号，转整数
* `-` 一元负号，转字符串
* `~` 一元取反，只读可写文档互转
* `%` 创建迭代器；迭代器搜索
* `%=` 迭代器搜索前进，未创建新迭代器
* `++` 迭代器自增
* `+=` 迭代器步进

### design.md

重在封装库的设计理念，操作符重载的择取思路。语言可更风趣生动些，联想比喻。
也可与其他 json 库的典型用法作比较，且不限 C++ 。

在介绍操作符时可参考 api.md 的顺序，但不一定要严格一致。代码用例从简，甚至可用
伪代码表达未实现或不能实现的写法。

### DONE:20251015-103000

生成文档太有 AI 味，还得自己修改。

### 补充修改需求

to AI:
你之前生成的这个文档太简略，没有人文味，只有章节大纲代码。作为教程向文档，我希
望有更详细说明，定位是面对 C++ 开发者的技术文档，希望能指导其他开发者正确、高
效地使用 xyjson 我已经修改了前两个二级章节，请从 `## 可写 JSON 模型操作` 开始
参考此文风修改。另外，有些示例代码是错的，对库功能理解有误，我已经用 `//! ` 标
记出来了，请重新修改。并且把原来的误用示例添加到后面常用错误那节中。

## TODO:2025-10-16/1 开发脚本读取 TODO 需求及更新完成状态

新增 script/todo.pl 脚本，实现功能：
- 如果接收一个参数表示需求ID，读取 `task_todo.md` 文档中与该需求ID匹配的 section
  内容，打印至终端。
- 如果接收两个参数，第二个视为完成的关联任务ID，则在与需求 ID 所在 section 末尾
  添加一行三级标题行，标记 `### DONE:任务ID`。
  * 一个需求 ID 允许关联多个任务 ID，所以如果已有 DONE: 标题行，直接在该行附加
    任务 ID，以空格分隔。
  * 新增 `DONE:` 行如果不是文件末尾，再加一空行与后面内容分开。

文件结构与扫描逻辑：
- 每个 TOD 需求以二级标题开始，形如 `## TODO:YYYY-MM-DD/n title` ，其中
  `YYYY-MM-DD/n` 就表示需求ID。为容错，需求ID与前面的 `TODO` 前缀不限以 `:` 分
  隔，以任何一个字符或没有间隔紧邻着也接受。
- 结束是下一个二级标题前，或遇到 `---` 长分隔线，或到文件末尾。
  行。

perl 脚本要求：
- 使用 perl5 基本模块，不要依赖 cpan 的额外模块
- 脚本规模超过几十行，就要考虑拆分子函数结构化
- 开头注释说明用法

### DONE: 20251016-231556

## TODO:2025-10-17/1 toString(true) 改为缩进格式化转字符串

toString(true) 这个函数很容易认人误解，包括 AI 的联想，以为可选 true 参数表示
格式化输出。事实上它只为根结点是字符串类型时加上引号，以满足合法序列化需求，其
他情况都是单行无格式序列化，与 toString() 完全一样。

所以还是决定将 toString(true) 改为格式化输出的行为，该变更预估可能的影响如下：
- 对于字符串结点，修改前后功能一样，toString() 没引号，toString(true) 有引号；
- 对于其他标量结点，修改前后功能一样，且 toString() 与 toString(true) 功能一样；
- 对于常规容器结点，toString() 在修改前后及 toString(true) 修改前是单行无格式序列化，toString(true) 在修改后是有缩进格式序列化

综合起来，修改后的功能更符合直觉，更有价值。

to AI: 请查阅 yyjson.h 的 API ，实现常规缩进的格式化序列化，并同步更新单元测试。

### DONE: 20251017-232802

## TODO:2025-10-17/2 设计几个空类表示空对象空数组空字符串

现在使用特殊字面 `"{}"` 与 `"[]"` 表示空对象与空数组，在有关赋值与容器插入时发
挥作用，但这也导致了传入字符串参数时要多做两次 strcmp 操作，没有真正的类型区分。

增加类定义：
struct EmptyObject{};
struct EmptyArray{};
struct EmptyString{};

及相应的类型代表值：kObject kArray kString
特殊字面量 `"{}"` 与 `"[]"` 在方法参数中的特殊意义先保留，只改 kObject 等常量
的类型。扩展相关方法重载支持这几个空类参数。

期望可用于 & = << 等操作符，修改其对应的具名方法，增加相应的测试用例。
当然保持原有语义，EmptyObject 与 EmptyArray 不能用于 | ，EmptyString 感觉上可以用于 |，
但 get 方法不能接收 EmtySting 引用，单独在操作符接口层改就不值当了。

to AI: 我已经在 @Part 1 增加了这三个空类，及修改了常量定义，请完成后续工作。

### DONE: 20251018-000707

## TODO:2025-10-18/1 支持 MutableValue 类型的 key * value 绑定

目前支持 `<<` 将键值对插入 Json 对象结点。
用操作符形如 `objectNode << key * (mutDoc * value)` 。
但是 key 类型目前只支持原始字符串类型，现在希望增加也支持已经是 MutableValue
类型的 key 。允许写成 `objectNode << (mutDoc * key) * (mutDoc * value)` 。

请增加 MutableValue::tag 方法重载，参数是另一个 MutableValue, 当然应该是能做
key 的字符串类型。
也可能要增加操作 `*` 重载，使两个 MutableValue 的 `key * value` 调用
`value.tag(key)` 。

同时检查一下 tag 方法的实现，是否可用已有的 create 方法重构，避免重复，保持一
致。

涉及新功能开发，须增加测试用例，尽量在原来的 tag 与 `*` 测试附加添加新段落或用
例名。

### DONE: 20251018-005422

## TODO:2025-10-18/2 迭代器优化

变更设计，将四个迭代器的 seek 方法的默认行为改为需要 rewind ，每次重头找，这样可能
更符合 `%` 操作符在定点创建新迭代器的语义。同步修改可能破坏的单元测试断言。

### DONE: 20251018-011944

## TODO:2025-10-19/1 检查对 json 大整数的支持

我对 yyjson 库对大整数的处理还不太确定，请帮忙查阅其 API 说明，它对 int uint
sint 是怎么区分的，有什么联系，包括 `is_` 判断与 `get_` 取值。

在 `utest/t_experiment.cpp` 写个演示性单元测试，使用 yyjson 的 C API ，说明它
是如何处理各类整数的。然后在 `utest/t_basic.cpp` 写个稍简短的测试，以确保
xyjson 封装的操作符处理大整数也是正确的。

在分析一下 toNumberCast 方法的实现，是否可以简化一些 if 分析。isInt 是否就已隐
含 isUint 或 isSint ?

### DONE: 20251019-094102

## TODO:2025-10-19/2 考虑为 Value 添加 < 比较操作

先分析一下能否给 json 值设计一个有意义的确定性排序，xyjson 库是否建议为
Value 类加个重载 < ，因为之前只重载了 == 相等性。

我初步考虑了一下，目前 == 重载是深度比较 json 树的内容。如果重载 < 也按内容比
较，该如何确定大小。可能的方案是：

- 先按 json 类型比较，就按 yyjson 定义的类型枚举，有确定性大小
- 各个同类型的 json 标量按其类型比较
- json 容器，先比较容器的大小，再比较数组的每个值，或对象的每个键
- 数字类型的比较可能要注意大小语义的直觉，正整数、负整数、浮点数的大小处理

另一个方案是轻量浅比较，只比较 Value 封装的指针大小，它大致反映了在原 json 串
的前后位置。但是 == 已经按内容比较了，为了一致性，最好把 == 也改为按指针比较。
然后 equal 方法加个可选参数表示按内容比较。

请认真分析这两个提案，或者你有其他提案。从语义合理性、实现难度、运行效率等角度
综合给出评估报告，先写各个方案的执行计划， 暂不修改代码。

yyjson 本地安装于： /usr/local/include/yyjson.h

### DONE: 20251019-105930

## TODO:2025-10-19/3 采用混合方案为 Value 添加 < 比较操作

根据上一任务的评估报告，`doing_plan.tmp/20251019-2.md` ，采用混合方案实现 < 比
较操作。
- 标量按内容值比较
- 容器，包括数组与对象，都先按 size() 比较，再回退指针比较

一些实现要求：
- 在两个 Value 类原有 equal 方法附近实现 less 方法
- 参考 toNumberCast 的模板函数，提炼两 Value 的 less 方法的公共代码，减少重复
- 在 == 操作符实现附近添加 < 实现
- 原来在 t_basic.cpp 的测试用例 basic_compare_ops 移到 t_advanced.cpp 中，然后
  也在 t_advanced.cpp 中增加新功能 < 的测试用例

测试用例除了一个基本的测试覆盖各种类型的 json 比较外，再构建一个可能的应用示例测试：
- 构造一个只读 Document ，一个数组内包含各种类型的子 json
- 构造另一个可写 MutableDocument ，复制原数组内的每个元素，放到 std::vector 中
- 排序后再写回 MutableDocument 的一个数组中
- 验证比较前后两上 Document 的数组结点的序列化结果

### DONE: 20251019-152200

## TODO:2025-10-19/4 优化 < 操作实现

当前模板实现，依赖 `get_const_val` 将 `yyjson_mut_val` 强转 `yyjson_val` 。这
感觉不安全，依赖底层结构体内存布局的近似性，且对于容器类型索引子结点的操作不兼
容。还有对比较数字类型的值也不优雅。

修改建议：
- 两个 Value 类加上 getType 方法，返回 yyjson 表征类型的常量
- 参考 EmtpyObject 等空类标记方案，增加 ZeroNumber 空类表示通用数字类型
- 将现有的 toNumber 方法名改为 toInteger
- 再将 toNumber 方法改为返回 double 类型，使其命名意义与 yyjson 更切合，凡数字
  类型都可用 dobule 表达，包括整数也能强转为 double , 直接使用 C API .
- lessCompare 方法在数字类型时转 toNumber 再比较
- 让 ZeroNumber 类型的代表值能用于 & = << 等操作，增加 isType set 与 create 方法重载
- 增加 getor 特化方法对 ZeroNumber 与 EmptyString 参数的重载，返回值分别是 double
  与 const char* 字符串
- 增加操作符 ~ 调用重载调用 toNumber
- 为新功能增加测试用例，不破坏原有测试用例

### DONE: 20251020-000000

后来还是觉得取消 ~json 操作调用，与 ~doc 语义完全不相关，
且 `| kNumber` 可实现 toNumber 的功能。

## TODO:2025-10-20/1 扩展与基本类型直接作相等性比较

支持 json 与基本类型直接 == 比较：
int int64_t uint64_t double `const char * ` std::string bool

无效 Value 或类型不匹配返回 false

请分析一下能否根据已有操作符使用模板泛型实现，
类似这样的逻辑： (json & rhl) && ((json | rhl) == rhl)
左参数类型用 `jsonT` ，`is_value` 限定
右参数类型用 `scalarT` 命名，不是 `is_value`

与 `const char*` 比较需要特化，否则 `|` 返回指针，然后 `==` 比较指针而不是比较
内容了

然后 `!=` 操作符用 `==` 实现。
当前有许多 `!=` 单独实现，尝试先注释屏蔽，改成通用的模板

代码修改在 include/xyjson.h @Section 5.3 位置，
单元测试在 `utest/t_advanced.cpp` 文件

再评估一下大小比较 `<` 能否扩展至支持比较 json 与基本类型？
我初步分析是没必要的，考虑到类型不一致时 `<` 与 `>` 都 false 就不自洽了。

### DONE: 20251020-114104

## TODO:2025-10-20/2 重新组织 Document 转 root 的操作符

单独开一节 @Section 5.7 收集转 root 的同义操作符。
迁移： /  + - <<(标准流输出)
增补：%
修改：== ，删除 Document 的 equal 方法，直接转调 root ，不判断 doc 指针，开销
影响极小，相同指针的 doc 取的 root 指针也该一样
决定不重载：| & ，比较琐碎，多个重载转 root 也不简洁

手动完成

### DONE: 20251020-152112

## TODO:2025-10-20/3 支持字面量运算符直接转 Document

在 yyjson::literals 子命名空间中定义 operator""_xyjson ，生成 Document.
新测试用例添加在 `t_conversion.cpp` 文件

放在 Part 5 最后，Part 6 之前。

### DONE: 20251020-155345

## TODO:2025-10-21/1 MutableValue << MutableValue 需要区分移动语义

现状：MutableValue << MutableValue 是复制右参数，插入左参数
要求：
- 增加 MutableValue << MutableValue&& 移动重载
- 优化 MutableValue << KeyValue 自动使用移动语义，操作后 KeyVale 无效化

请尽量修改具名方法 create append add input 等实现，少改或不改操作符实现修改。
在 `utest/t_mutable.cpp` 增加单元测试

补充分析，发现不能重载 << MutableValue&& ，因为 `/` 操作符查找已有结点产生
MutableValue 临时值，是纯右值，如果移动了反而不对。还是应该保持现状复制安全。
MutableValue 是值封装，右值区分意义不大。

<< KeyValue 也一般用于临时值，没必要费事添加后再去修改 KeyValue ，还是文档中说
明清楚得好。

### UNDO: 20251021-210930

## TODO:2025-10-21/2 分析字符串字面量优化方案

MutableValue 在修改或添加 json 结点时，希望能做字符串字面量优化。请仔细思考、
分析现有的实现，指出哪里有缺陷或实现不够优雅简洁，给出修改建议。

目前涉及的字符串参数类型有以下几种：
- 字符中字面量
- const char* 指针，C 风格
- std::string
- std::string_view （暂未使用）后面可能考虑支持

只有字面量是可以安全优化，以引用方式创建 json 结点。其他字符串以拷贝方式创建
json 结点，yyjson 有不同的 api 支持引用或拷贝。

现有一些实现手段：
- 添加了 StringRef 包装类，保存字面量引用信息
- 一些方法(set create append add 等)增加含 Ref 后缀的版本，显式使用引用字符串
- 模板参数用 `const char(&)[N]` 参数匹配字面量

但我对当前并不太满意：
- MutableValue << 操作符重载的模板有很多份冗余
- MutableValue << std::pair 支持不完备
- 具名方法 append add 等不能自动识别字面量，只能用 Ref 后缀版本的方法名

以上是我个人的一些初步分析，请帮忙再给一份深入、全面的分析与修改建议报告。先不
修改代码，将报告写入 `doing_plan.tmp/` 子目录。但可以在 `utest/t_experiment.cpp`
中写一些实验或研究性测试代码，展示关键技术要点。

## TODO:2025-10-22/1 字符串字面量优化之一：底层 create 支持

在 @Section 3.1 增加 create ，支持字字符串字面 `const char[N]` ，关键是要与现在
支持的 `const char*` 区分开，可能要为这两种字符串参数作特殊处理。

要求通过单元测试用例 `experiment_create_optimization` （已添加），
也不破坏其他用例。

核心解决技术：
`std::is_same<T, const char*>::value || std::is_same<T, char*>::value`
降低 `const char*` 重载版本的优化级，字面量 `const char[N]` 就能匹配。

`const char[N]` 重载版本要处理特殊情况 "{}" 与 "[]" 。
遗留问题：比较字面量能否在编译期完成

### REF: 2025-10-21/2

### DONE: 20251022-143132

## TODO:2025-10-22/2 字符串字面量优化之二：转发函数优化

xyjson 中创建 json 结点的最底层函数 `yyjson::create` 已经通过模板技术支持字符
串字面量优化，并能区分 `const char*` C 字符串。

现需要分析从操作符层 `MutableValue <<` 到 `create` 的调用链，简化模板重载的不
必要冗余，考虑使用万能引用 `&&` 与完美转发。中间涉及的函数大致包括 input
(inputKey inputValue) append add tag 等。

增加 MutableValue::input 方法对 KeyValue 与 std::pair 的特化，它只能用于 json
Object ，调用 add 方法

赋值操作 `=` 及其方法 `set` 暂时不改，本需求先专注重构 `<<` 操作符相关。

### DONE:20251022/210318

## TODO:2025-10-22/3 KeyValue 相关操作限定为右值

移动更符合其语义用途，不建议保存中间值，否则要显式 std::move 转右值。

考虑 KeyValue 改名 `_KeyValue` ，表明内部类。但是有点难看。

移动语义本该在 tag `*` 后将原 MutableValue 置空，<< 插入后再将 KeyValue 置空。
但又考虑大部分情况下是使用纯右值，置空操作是否浪费，或者按保守设计写个置空操作，
纯右值时能否被编译器优化。

### DONE:20251023/001149

## TODO:2025-10-23/1 为各种增加 key 结点的方法增加字符串类型限定

目前 MutableValue 有几个方法涉及增加 json key 结点：
- add 插入键值对
- tag 将一个值绑定键
- inputKey 分步输入悬挂键

增加一个 `is_key` 类型限定，匹配几种支持的字符串：
- const char*
- const char[N] 字面量
- char[N] 可写内存 buffer
- std::string

其中 const char[N] 与 char[N] 能退化的 `const char*` ，考虑是否能简化。
也可以写个辅助 constexpr 函数来判断某类型是否能当键的字符串。

希望尽量编译期判断有效 key 类型，避免运行期再判断创建的结点是否
string/key 类型。在方法实现中采用 if constexpr 可能更简洁些。

与 tag 相关的操作符 `*` 也分析一下能否用 `is_key` 限定化简。
与 add inputKey 相关的操作符 `<<` 是通用的输入，不能限定字符串。

### DONE: 20251023/141414

## TODO:2025-10-23/2 字符串字面量优化之三：赋值操作符 set 方法与上层 create 方法

参考 `yyjson::create` 重载函数对字面量与 C 字符串 `const char*` 的区分方案，重
构 `MutableValue::set` 方法。在 `set` 方法层支持字面量，简化 `=` 转发模板。

实现目标：`json.set("literal")` 与 `json="literal"` 都自动优化为按引用字符串，
不再需要显式调用 `setRef` ，或传参 `StringRef` 。

再简化 `MutableDocuemnt::create` 方法，尽量使用统一模板转发底层 `yyjson::create` ，
可参考 append 方法的实现。删除不必要的重载版本，尤其可删除 createRef , 应该直接用
create 方法就能自动识别字面量优化了。

注意 set 方法的实现不能改为调用 create 。

### DONE: 20251023/212121

`MutableDocuemnt::create` 比较容易统一转发化简模板。`MutableValue::set` 与 `=`
在实施过程一些麻烦，只算完成一半。疑难点：

- input<< 方法有分支转发 set= ，会接收一些不支持 set= 的类型参数
- = 操作符也是标准的拷贝赋值方法，可能会冲突

## TODO:2025-10-23/3 再优化增加 key 结点的方法

提炼新增函数，下沉到 createKey ，在原来 `yyjson::create` 后面定义。
- 模板函数，参数限定 is_key_type()
- 普通重载，参数 MutableValue ，先判断是否字符串

上层方法(MutableValue 类)改为先调用 createKey，包括：
- add
- tag
- inputKey

### REF: 2025-10-23/1

### DONE: 20251023/213030

add 与 tag 方法容易改造。但 inputKey 由 input 转发而来，为支持 << 混合键值，会
收到不属于 key 的类型参数，不易优化。

## TODO:2025-10-23/4 字符串字面量优化之四：KV 宏 pair 插入保持引用

目前大多场景下插入字符串结点都实现了字面量引用优化，但这个 std::pair 封装原生
键值的情景还未优化。

单元测试用例 `mutable_kvpair_objadd` to fix: block 暂被注释，因测试不通过。
仔细分析其原因，尝试修复它，让这个功能也支持字面量优化。

如果 std::pair 不行，也可以自定义一个 `yyjson::pair` ，重定义 KV 宏。

按 TDD 开发流程，先取消 `//!` 屏蔽行，再修改 xyjson.h 实现令测试通过。

### UNDO: 20251023/230430

又想到这个功能没啥意义，在写用户文档时都想不到切入点介绍它的意义。
- 性能上不如 add 方法直接
- 也不完全符合操作符风格
- KV 很异类，太短易冲突，长了又不方便
- `std::make_pair` 函数参数退化，无字面量优化，重新实现个自己的性价比不高

所以决定把它删了吧。然后 @Part 6 就没内容，但这个标题行可先留着标志末尾。

## TODO:2025-10-23/5 删除 `<<` 赋值标量功能，只用于插入容器

赋值标量的功能与 `=` 重复了，且带来实现上复杂性，转发 set 污染了 set 接收的模
板参数，给 set 优化带了障碍，性价比不值。决定删了该功能！

删除 input 转发后，set 方法的模板就能区分字面量与 `const char*` 了。

遗留问题：`is_cstr_type` 简化匹配 C-Style 的写法。
需要将原来的 2.5 section trait 提到 part 1 前面来先定义。

### DONE: 20251024/003132

## TODO:2025-10-24/1 显式 move 方案

### 2025-10-21/1
增加 MutableValue::move 方法，生成 MoveValue ，将底层指针 `yyjson_mut_val*`
转移给后者。MoveValue 在添加到其他 MutableValue 的对象或数组时不再拷贝。
为 create 与 createKey 函数增加 MoveValue&& 重载，直接返回指针。
可考虑操作符 ~MutableValue 调用 .move 方法。

### UNDO:

在标准 std::move 与自定义 .move 方案之间反复横跳纠结。
各有优缺点。

最后还是决定先与 KeyValue tag * 一样用 std::move && 表明移动意图。
文档加注不用为已附在 json 树上的结点使用 std::move ，只应移动独立结点：
- 由 create 创建的新结点 MutableValue
- 从 json 树上删除摘下的结点
- 避免 << /path/to/existed/node ，这临时值也匹配 && 右值

## TODO:2025-10-24/2 MutableValue << MutableValue 支持移动语义

现在是复制结点添加到目标容器中，要求支持 std::move 后按右值移动到目标容器。

大致改动点：
- 增加 MutalbeValue::setMoved 方法，标记 `m_val` 为空 
- yyjson::create 方法增加 MutableValue&& 参数重载，直接移动返回其指针
- 增加 createKey 也接收 MutableValue&& 重载，转发 create

然后检查上层方法应该自动支持移动语义了。

可按 TDD 流程开发，先在 `t_mutable.cpp` 测试文件中添加新示例。

### DONE: 20251024-112310

AI 写了行测试调用 add(k, v) 其中 k 是原始指针 `yyjson_mut_val*` ，v 是
`MutableValue` ，现不支持。有必要支持原始指针吗？先不管吧。用户要么全用 C API,
不要与 xyjson 类 API 混用。

## TODO:2025-10-24/3 为辅助功能增加子命名空间

- Section 1.3: Type Traits 放在 trait:: 子空间
- Part 3 的两节都放在 util:: 子空间

- 命名空间不增加缩进
- 调整调用处引用名字

### DONE: 20251024-143000

## TODO: 分析迭代器优化方案
## TODO: 考虑实现 MutableValue 删除功能

## TODO: 优化文档示例代码管理同步单元测试

- 针对文档：READE.md docs/usage.md
- 在代码示例之前加隐藏注释 example:name ，其中 name 遵循单元测试用例名，加
  read_ 或 usage_ 前缀。
- 增加单元测试文档 `utest/t_docx.cpp`，收集文档中的示例，为每个示例定义一个
  `DEF_TAST` 测试用例名。
- 拷入的示例包括首尾的引用块标记，然后注释掉，如 `// '''cpp` 
- 在拷入块标记行前可以添加必要的初始变量定义，以符合语法
- 在拷入块标记行之后添加一些 `COUT` 语句断言示例的变量值符合预期输出
- 编译检查示例语法正确，运行检验行为正确性，作必要修改调整
- 在 script/ 目录下添加一个 perl 脚本，支持将 `t_docx.cpp` 中的示例例代码反向
  拷回文档

## TODO: 考虑条件编译宏过滤不用的功能

## TODO: review 代码发现 bug 与可优化点

- val.getor("") 编译失败，但 val | "" 成功

## TODO: 不支持功能联想

AI 写出这样的代码：
doc / "name" | [](const std::string& s) { 转大写 } | "DEFAULT"
目前支持的函数只接收 Value 参数，但不支持这样传基本类型的

---

- AI  完成任务ID: date +"%Y%m%d-%H%M%S"
- 人工完成任务ID: date +"%Y%m%d/%H%M%S"
