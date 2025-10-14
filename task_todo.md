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

### DONE:20251012-220000

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

------
## TODO:2025-10-14/2 简化 Document 类不必持有根结点

## TODO: getor 与 | 操作符可取底层 C 结构体指针

## TODO: 增加 isType 方法与 & 操作符判断 json 结点类型

## TODO: 设计模板类的 KeyValue 优化对象容器的插入

## TODO: 考虑条件编译宏过滤不用的功能

## TODO: review 代码发现 bug 与可优化点

- Document 互转会调用拷贝构造函数，但这是被禁用的
- val.getor("") 编译失败，但 val | "" 成功
- 单元测试识别指针比较改用 COUT_PTR 
