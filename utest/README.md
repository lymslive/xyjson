# xyjson 单元测试

本项目使用 [couttast](https://github.com/lymslive/couttast) 作为测试框架。

## 测试文件结构

单元测试文件按照功能模块进行拆分：

- `t_experiment.cpp` - 实验性功能和重载规则测试
- `t_basic.cpp` - 只读 json 模型基础功能测试
- `t_mutable.cpp` - 可写 json 模型操作测试
- `t_stream.cpp` - json 文档流操作测试
- `t_iterator.cpp` - 迭代器功能测试
- `t_conversion.cpp` - 类型转换测试
- `t_advanced.cpp` - 其他高级功能测试

### 贡献指南

添加新测试时，请遵循以下准则：
1. 根据功能模块选择对应的测试文件
2. 测试用例名称使用 `模块_功能` 的命名格式
3. 为测试用例提供清晰的描述
4. 确保测试独立且可重复
5. 遵循现有代码风格和格式

## 命令行用法

构建成功后在 build 目录运行。

```bash
# 运行所有测试
./utxyjson --cout=silent

# 运行特定测试用例，可指定多个
./utxyjson basic_read_number ...

# 运行特定文件内的所有用例，也可指定多个文件
./utxyjson t_basic.cpp --cout=silent

# 运行名字中包含 mutable 的用例
./utxyjson mutable --cout=silent

# 列出所有测试用例名，--List 还包含文件名位置等详情
./utxyjson --list

# 获取帮助信息
./utxyjson --Help
```

### 输出控制
**回归测试时建议加上 `--cout=silent` ** 减少冗余输出。
只在复测某个失败用例时不加 `--cout` 参数按默认详细输出。

### 测试用例筛选

- 每个非选项式命令参数独立处理
- 含 `.cpp` 后缀的当作测试文件名
- 匹配用例全名时只运行该用例，否则筛选能部分匹配名字的用例

## 测试用例一览表

可由以下命令输出测试用例目录表：

```bash
script/utable.pl # 或 make utable
```

<!-- UTABLE_START -->
| 序号 | 测试用例名称 | 文件 | 行号 | 描述 |
|------|-------------|------|------|------|
| 1    | `basic_size` | `t_basic.cpp` | 21   | verify class sizes to ensure proper optimization |
| 2    | `basic_read_number` | `t_basic.cpp` | 39   | operator read number from scalar value |
| 3    | `basic_read_string` | `t_basic.cpp` | 136  | operator read string from scalar value |
| 4    | `basic_error_handling` | `t_basic.cpp` | 184  | handle path operator error |
| 5    | `basic_reread` | `t_basic.cpp` | 237  | test re-read document |
| 6    | `basic_index_operator` | `t_basic.cpp` | 264  | test index method and operator[] |
| 7    | `basic_compare_ops` | `t_basic.cpp` | 376  | test comparison operators |
| 8    | `basic_json_pointer` | `t_basic.cpp` | 463  | test JSON Pointer functionality |
| 9    | `mutable_read_modify` | `t_mutable.cpp` | 26   | test read-modify workflow |
| 10   | `mutable_value_input` | `t_mutable.cpp` | 66   | test input operator << for mutable value |
| 11   | `mutable_assign_copy` | `t_mutable.cpp` | 130  | test = and copy behavior of yyjson wrapper classes |
| 12   | `mutable_assign_string` | `t_mutable.cpp` | 228  | test string node in yyjson |
| 13   | `mutable_assign_string_ref` | `t_mutable.cpp` | 288  | test string node reference in yyjson |
| 14   | `mutable_object_insertion` | `t_mutable.cpp` | 341  | test object insertion with KV macro and operator+ |
| 15   | `mutable_array_append` | `t_mutable.cpp` | 421  | test append to MutableValue array |
| 16   | `mutable_create_methods` | `t_mutable.cpp` | 500  | test MutableDocument create methods and * operator |
| 17   | `mutable_append_doc` | `t_mutable.cpp` | 569  | MutableValue array append with Document and MutableDocument |
| 18   | `mutable_objadd_doc` | `t_mutable.cpp` | 616  | MutableValue object add with Document and MutableDocument |
| 19   | `mutable_keyvalue_add` | `t_mutable.cpp` | 666  | test KeyValue optimization for object insertion |
| 20   | `stream_document_ops` | `t_stream.cpp` | 18   | test Document stream operators << and >> |
| 21   | `stream_file_ops` | `t_stream.cpp` | 83   | test FILE* and std::fstream stream operations |
| 22   | `stream_std_output` | `t_stream.cpp` | 233  | test standard output stream operators << |
| 23   | `stream_file_operations` | `t_stream.cpp` | 315  | test file read/write operations for Document and MutableDocument |
| 24   | `iterator_functionality` | `t_iterator.cpp` | 19   | test array and object iterator functionality |
| 25   | `iterator_operators` | `t_iterator.cpp` | 197  | test iterator operators and % operator |
| 26   | `iterator_begin_end` | `t_iterator.cpp` | 506  | test standard begin/end iterator pattern |
| 27   | `iterator_mut_begend` | `t_iterator.cpp` | 669  | test mutable standard begin/end iterator pattern |
| 28   | `conversion_methods` | `t_conversion.cpp` | 17   | test toString() and toNumber() conversion methods |
| 29   | `conversion_operators` | `t_conversion.cpp` | 147  | test unary operators - and + for conversion |
| 30   | `conversion_document` | `t_conversion.cpp` | 303  | test Document and MutableDocument conversion methods |
| 31   | `conversion_doc_unary` | `t_conversion.cpp` | 433  | test Document and MutableDocument unary operators |
| 32   | `advanced_pipe` | `t_advanced.cpp` | 57   | test pipe() method and | operator with functions |
| 33   | `advanced_trait` | `t_advanced.cpp` | 197  | test type traits for yyjson wrapper classes |
| 34   | `experiment_overload` | `t_experiment.cpp` | 85   | test overload rule |
<!-- UTABLE_END -->
