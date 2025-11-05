# xyjson 示例

本目录包含了演示 xyjson C++ JSON 库特性和用法的示例应用程序。

## 构建示例

构建所有示例：

```bash
# 从项目根目录
mkdir -p build && cd build
cmake .. && make
# 或者简单执行：
make examples
```

构建的可执行文件将在 `build/examples/` 目录中。

也可以单独构建某个示例：

```bash
make jp
make struct_map
make json_transformer
```

## 示例概览

### 1. jp - 极简版 jq 风格的 JSON 路径查询工具

一个命令行工具，用于使用路径表达式查询和提取 JSON 文件中的数据。

**特性：**
- 通过路径查询 JSON（如 `users[0]/name`）
- 从文件或标准输入读取
- 格式化 JSON 输出
- 对无效路径的错误处理

**用法：**
```bash
# 查询 JSON 文件中的特定路径
./jp /path/to/file.json?/users[0]/name

# 从标准输入读取
echo '{"a": {"b": 1}}' | ./jp -?/a/b

# 打印整个 JSON 文件（格式化）
./jp /path/to/file.json

# 打印标准输入（格式化）
cat file.json | ./jp -?
```

**示例：**
```bash
# 给定一个 JSON 文件：
echo '{"users": [{"name": "Alice", "age": 30}]}' > data.json

# 查询第一个用户的姓名
./jp data.json?/users[0]/name
# 输出： "Alice"

# 查询第一个用户的年龄
./jp data.json?/users[0]/age
# 输出： 30
```

**演示内容：**
- 路径操作符（`/`）的使用
- 类型安全值提取（`|` 操作符）
- 错误处理
- 从文件和标准输入读取

---

### 2. struct_map - C++ 结构体与 JSON 映射

演示使用 xyjson 在 C++ 结构体和 JSON 之间进行序列化和反序列化。

**特性：**
- 定义包含多种数据类型的结构体
- 使用操作符重载从 JSON 反序列化
- 使用操作符重载序列化为 JSON
- 业务逻辑处理
- 数据验证

**用法：**
```bash
# 使用默认 JSON
./struct_map

# 从文件读取
./struct_map user.json

# 从标准输入读取
cat user.json | ./struct_map
```

**示例：**
```bash
./struct_map
```

**输出：**
```
=== 反序列化的用户 ===
用户 ID: 1001
  姓名: Alice Smith
  邮箱: alice@example.com
  年龄: 28
  活跃: 是
  分数: 85.5 (高级)
  标签: developer c++ json
  备注: Team lead for backend development

=== 业务逻辑 ===
用户级别: 高级
验证: 通过

=== 重新序列化的 JSON ===
{
    "id": 1001,
    "name": "Alice Smith",
    "email": "alice@example.com",
    "age": 28,
    "is_active": true,
    "score": 85.5,
    "tags": ["developer", "c++", "json"],
    "note": "Team lead for backend development",
    "processed_at": "2025-11-05",
    "processed": true
}
```

**演示内容：**
- 操作符重载（`/` 用于路径访问，`=` 用于赋值）
- 类型提取操作符（`|` 用于默认值）
- 数组操作（`<<` 用于追加）
- 结构体映射模式
- 业务逻辑集成

---

### 3. json_transformer - JSON 数据转换工具

一个用于转换和分析 JSON 数据结构的综合工具。

**特性：**
- 转换 JSON 结构（添加计算字段、标准化数据）
- 按路径过滤数据
- 生成摘要报告
- 链式操作
- 统计跟踪

**用法：**
```bash
# 转换 JSON 结构
./json_transformer --transform data.json

# 按 JSON 路径过滤
./json_transformer --filter data.json?/users[0]/name

# 生成摘要报告
./json_transformer --summary data.json
```

**示例 1：转换**
```bash
# 给定 data.json：
echo '{"name": "alice", "age": 30}' > data.json

./json_transformer --transform data.json
```

**输出：**
```
=== 转换统计 ===
文件处理: 1
对象创建: 0
字段添加: 0

=== 转换后的 JSON ===
{
    "name": "Alice",
    "age": 30
}
```

**示例 2：过滤**
```bash
./json_transformer --filter data.json?/age
```

**输出：**
```
=== 过滤结果 (路径: /age) ===
类型: number
值: 30
```

**示例 3：摘要**
```bash
./json_transformer --summary data.json
```

**输出：**
```
=== JSON 摘要报告 ===
顶层类型: Object
对象包含 2 个键
```

**演示内容：**
- 迭代器使用（`iterator()` 方法）
- 动态 JSON 修改
- 路径查询
- 类型检查和提取
- 统计跟踪

---

## 演示的 xyjson 关键特性

### 操作符重载
- **`/`** - 路径访问操作符
- **`|`** - 带默认值的值提取操作符
- **`=`** - 赋值操作符
- **`<<`** - 流/输入操作符
- **`==`**, **`<`**, **`>`** - 比较操作符

### 类型安全
- 编译时类型检查
- 运行时值提取与默认值
- 类型验证方法

### 内存管理
- 基于 RAII 的内存管理
- 无需手动内存清理
- 自动生命周期管理

### 灵活性
- 只读和可变模型
- 迭代器支持集合
- 基于流的 I/O

---

## 单独构建示例

如果只想构建一个示例：

```bash
# 仅构建 jp
make jp

# 仅构建 struct_map
make struct_map

# 仅构建 json_transformer
make json_transformer
```

---

## 清理构建

如果遇到任何构建问题：

```bash
cd build
rm -rf *
cmake ..
make
```

---

## 依赖要求

- C++17 或更高版本
- CMake 3.15 或更高版本
- yyjson 库（如果未找到会自动下载）
- C++ 编译器（GCC、Clang 或 MSVC）

---

## 注意事项

- 所有示例都是自包含的，除了 xyjson 本身之外不需要外部依赖
- 示例可以用作你自己 JSON 处理应用程序的起点
- 包含错误处理以演示最佳实践
- 代码大量注释以解释 xyjson API 的使用
