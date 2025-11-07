# xyjson 性能测试

本目录包含 xyjson 相对于 yyjson 原生 C API 的性能测试套件。

## 编译

```bash
# 在构建目录中
cmake .. -DBUILD_PERF=ON
make perf_test
```

## 运行

```bash
# 运行所有性能测试
./perf_test

# 静默模式（仅显示失败和总结）
./perf_test --cout=silent

# 运行特定测试
./perf_test parse
./perf_test access
./perf_test iterator

# 列出所有测试
./perf_test --list
```

## 测试内容

### 1. 解析性能测试 (p_parse.cpp)
- 字符串解析速度
- 文件解析速度
- 不同大小数据集的解析性能

### 2. 访问性能测试 (p_access.cpp)
- 路径访问性能（`/ "a" / "b"`）
- 索引访问性能（`["key"]`）
- 类型转换性能

### 3. 迭代器性能测试 (p_iterator.cpp)
- 数组遍历性能
- 对象遍历性能
- 迭代器 vs 原始循环对比

### 4. 组合操作测试 (p_chained.cpp)
- 链式操作性能
- 批量操作性能
- 深度嵌套访问性能

### 5. 对比测试 (p_comparison.cpp)
- xyjson vs yyjson 详细对比
- 性能开销分析
- 不同场景下的表现

## 测试数据集

- `datasets/small.json` - 约 500B，简单结构
- `datasets/medium.json` - 约 50KB，典型业务数据
- `datasets/large.json` - 约 5MB，大型结构
- `datasets/huge.json` - 约 50MB，极端场景

## 结果输出

所有测试都会输出：
- 平均执行时间（微秒）
- 最小/最大执行时间
- 执行次数
- 性能对比（xyjson vs yyjson）

测试结果会实时输出到控制台，也可以通过重定向保存到文件进行分析。
