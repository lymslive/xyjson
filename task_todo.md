# 项目开发原始需求管理

本文档由人工维护，**AI受限编辑** ，仅允许在每个 `## TODO ` 下加个子标题 `### DONE` 关联 `ai_request.log.md` 的完成任务ID记录。

需求ID 格式：`TODO yyyy-mm-dd/n` ，日期/编号。日期表示头脑风暴冒出该需求想法的
日期，若一天中产出多个想法则加编号。仅有初步概念但未细化的需求暂不加日期 id ，
先快速列出 `TODO: 标题`，后面有细化内容时再赋日期 id 。

## TODO 2025-10-12/1: 拆分单元测试文件

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

### DONE: 20251012-121900

备注：
制定拆分方案写入 `doing_plan.tmp/` 后，由 AI 拆分消耗 token 比较大，也较慢，可
能还不如手工复制粘贴快。

## TODO 2025-10-12/2: 头文件结构规划

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

### DONE: 20251012-220000

## TODO 2025-10-12/3: 常用命令 make 配置集成

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

### DONE: 20251013-095233

## TODO 2025-10-13/1: 完全 head-only 化之一

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

### DONE: 20251013-110656

## TODO 2025-10-13/2: 完全 head-only 化之二

两个类的 toNumberCast 方法实现很类似，增加一个非类模板函数，模板类型参数适配
Value 或 MutableValue 类，然后在各自的 toNumber 类中调用新的模板函数。

新的模板函数也可命名为 toNumberCast ，而将原来的 toNumberCast 方法删去。

### DONE: 20251013-140710

## TODO 2025-10-13/3: 完全 head-only 化之三

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

## TODO: 简化 Document 类不必持有根结点

## TODO: getor 与 | 操作符可取底层 C 结构体指针

## TODO: 增加 isType 方法与 & 操作符判断 json 结点类型

## TODO: 设计模板类的 KeyValue 优化对象容器的插入

## TODO: 考虑条件编译宏过滤不用的功能
