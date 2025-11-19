# xyjson 单元测试

本项目使用 [couttast](https://github.com/lymslive/couttast) 作为测试框架。

## 测试文件结构

编译目标 `utxyjson` 的单元测试文件按照功能模块进行拆分：

- `t_experiment.cpp` - 实验性功能和重载规则测试
- `t_basic.cpp` - 只读 json 模型基础功能测试
- `t_mutable.cpp` - 可写 json 模型操作测试
- `t_stream.cpp` - json 文档流操作测试
- `t_iterator.cpp` - 迭代器功能测试
- `t_conversion.cpp` - 类型转换测试
- `t_advanced.cpp` - 其他高级功能测试

编译目标 `utdocs` 是从文档中提取同步的测试示例：

- `t_readme.cpp` - 同步 READE.md
- `t_usage.cpp` - 同步 docs/usage.md
- `t_api.cpp` - 同步 docs/api.md

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

可由以下命令输出测试用例(utxyjson)目录表：

```bash
script/utable.pl # 或 make utable
```

<!-- UTABLE_START -->
| 序号 | 测试用例名称 | 文件 | 行号 | 描述 |
|------|------|------|------|------|
| 1 | basic_size | t_basic.cpp | 10 | verify class sizes to ensure proper optimization |
| 2 | basic_read_number | t_basic.cpp | 40 | operator read number from scalar value |
| 3 | basic_read_string | t_basic.cpp | 145 | operator read string from scalar value |
| 4 | basic_error_handling | t_basic.cpp | 193 | handle path operator error |
| 5 | basic_reread | t_basic.cpp | 246 | test re-read document |
| 6 | basic_index_operator | t_basic.cpp | 275 | test index method and operator[] |
| 7 | basic_json_pointer | t_basic.cpp | 389 | test JSON Pointer functionality |
| 8 | basic_type_checking | t_basic.cpp | 537 | type checking with isType method and & operator |
| 9 | basic_underlying_pointers | t_basic.cpp | 648 | extract underlying yyjson pointers |
| 10 | mutable_read_modify | t_mutable.cpp | 11 | test read-modify workflow |
| 11 | mutable_value_input | t_mutable.cpp | 51 | test input operator << for mutable value |
| 12 | mutable_assign_copy | t_mutable.cpp | 118 | test = and copy behavior of yyjson wrapper classes |
| 13 | mutable_assign_string | t_mutable.cpp | 216 | test string node in yyjson |
| 14 | mutable_assign_string_literal | t_mutable.cpp | 276 | test string literal optimization in set and assignment |
| 15 | mutable_assign_string_ref | t_mutable.cpp | 330 | test string node reference in yyjson |
| 16 | mutable_kvpair_objadd | t_mutable.cpp | 384 | test object add method |
| 17 | mutable_array_append | t_mutable.cpp | 470 | test append to MutableValue array |
| 18 | mutable_create_methods | t_mutable.cpp | 549 | test MutableDocument create methods and * operator |
| 19 | mutable_append_doc | t_mutable.cpp | 618 | MutableValue array append with Document and MutableDocument |
| 20 | mutable_objadd_doc | t_mutable.cpp | 665 | MutableValue object add with Document and MutableDocument |
| 21 | mutable_keyvalue_add | t_mutable.cpp | 715 | test KeyValue optimization for object insertion |
| 22 | mutable_keyvalue_mutablekey | t_mutable.cpp | 866 | test KeyValue with MutableValue key |
| 23 | mutable_move_semantics | t_mutable.cpp | 916 | test move semantics for MutableValue insertion |
| 24 | mutable_pop_pop | t_mutable.cpp | 1000 | test pop functionality for array and object |
| 25 | mutable_stream_operator | t_mutable.cpp | 1054 | test >> operator for pop functionality |
| 26 | mutable_clear | t_mutable.cpp | 1109 | test clear functionality for MutableValue |
| 27 | stream_document_ops | t_stream.cpp | 10 | test Document stream operators << and >> |
| 28 | stream_file_ops | t_stream.cpp | 75 | test FILE* and std::fstream stream operations |
| 29 | stream_file_nonexistent | t_stream.cpp | 225 | test file operations with non-existent files |
| 30 | stream_std_output | t_stream.cpp | 297 | test standard output stream operators << |
| 31 | stream_file_operations | t_stream.cpp | 379 | test file read/write operations for Document and MutableDocument |
| 32 | iterator_basic_loop | t_iterator.cpp | 25 | basic loop with four iterator |
| 33 | iterator_copy_ctor | t_iterator.cpp | 167 | iterator basic constructor, copying and assignment |
| 34 | iterator_operators | t_iterator.cpp | 220 | test iterator operators and methods |
| 35 | iterator_edge_cases | t_iterator.cpp | 338 | test iterator edge cases |
| 36 | iterator_begin_end | t_iterator.cpp | 490 | test begin/end pattern |
| 37 | iterator_arithmetic | t_iterator.cpp | 566 | test iterator arithmetic operators % %= + += |
| 38 | iterator_fast_seek | t_iterator.cpp | 759 | test fast seek functionality with / operator |
| 39 | iterator_array_insert | t_iterator.cpp | 958 | mutable array iterator insert functionality |
| 40 | iterator_array_remove | t_iterator.cpp | 1089 | mutable array iterator remove functionality |
| 41 | iterator_object_insert | t_iterator.cpp | 1226 | mutable object iterator insert functionality |
| 42 | iterator_object_remove | t_iterator.cpp | 1298 | mutable object iterator remove functionality |
| 43 | iterator_over_state | t_iterator.cpp | 1392 | test over method and iterator end state |
| 44 | iterator_standard_interface | t_iterator.cpp | 1552 | standard iterator interface and algorithm compatibility |
| 45 | iterator_object_chained_insertion | t_iterator.cpp | 1710 | mutable object iterator chained insertion support |
| 46 | iterator_decrement | t_iterator.cpp | 1845 | test iterator decrement operators -- |
| 47 | conversion_methods | t_conversion.cpp | 10 | test toString(), toInteger(), and toNumber() conversion methods |
| 48 | conversion_operators | t_conversion.cpp | 160 | test unary operators - and + for conversion |
| 49 | conversion_document | t_conversion.cpp | 317 | test Document and MutableDocument conversion methods |
| 50 | conversion_doc_unary | t_conversion.cpp | 447 | test Document and MutableDocument unary operators |
| 51 | conversion_user_literals | t_conversion.cpp | 594 | test user-defined literals for direct Document creation |
| 52 | conversion_explicit | t_conversion.cpp | 701 | test explicit type conversion functions |
| 53 | advanced_pipe | t_advanced.cpp | 52 | test pipe() method and \| operator with functions |
| 54 | advanced_trait1 | t_advanced.cpp | 225 | test base type traits for yyjson wrapper classes |
| 55 | advanced_trait2 | t_advanced.cpp | 320 | test extended type traits: is_scalar, enable_getor, is_callable_with_scalar, is_callable_type |
| 56 | advanced_pipe_only | t_advanced.cpp | 371 | test pipe() method only without operator\|(json, func) |
| 57 | advanced_compare_ops | t_advanced.cpp | 414 | test comparison operators |
| 58 | advanced_sort_mixed_array | t_advanced.cpp | 589 | sort a mixed array of json values |
| 59 | advanced_string_optimization | t_advanced.cpp | 630 | test string creation optimization strategies |
| 60 | experiment_overload | t_experiment.cpp | 86 | test overload rule |
| 61 | experiment_docx | t_experiment.cpp | 135 | test operator usage |
| 62 | experiment_large_int | t_experiment.cpp | 168 | test yyjson large integer support |
<!-- UTABLE_END -->
