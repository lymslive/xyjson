/**
 * @file p_comparison.cpp
 * @author xyjson project
 * @date 2025-11-07
 * @brief 综合对比测试 - 简化版
 */

#include "couttast/couttast.h"
#include "xyjson.h"
#include <yyjson.h>
#include <string>
#include <tuple>
#include "perf_common.h"

using namespace yyjson;
using perf::measurePerformance;
using perf::printComparisonTable;

// 测试: 性能综合对比
DEF_TAST(comparison_comprehensive, "性能综合对比")
{
    std::vector<std::tuple<std::string, long long, long long>> results;

    // 解析性能对比
    std::string simple_json = R"json({"name": "John", "age": 30})json";

    long long xyjson_parse = measurePerformance("xyjson", [&simple_json]() {
        Document doc(simple_json);
    }, 10000);

    long long yyjson_parse = measurePerformance("yyjson", [&simple_json]() {
        yyjson_doc* doc = yyjson_read(simple_json.c_str(), simple_json.size(), 0);
        if (doc) yyjson_doc_free(doc);
    }, 10000);

    results.emplace_back("小文件解析", xyjson_parse, yyjson_parse);

    // 访问性能对比
    Document doc(simple_json);

    long long xyjson_access = measurePerformance("xyjson", [&doc]() {
        const char* name = doc / "name" | "";
        int age = doc / "age" | 0;
    }, 10000);

    yyjson_doc* yy_doc = yyjson_read(simple_json.c_str(), simple_json.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_access = measurePerformance("yyjson", [&]() {
        yyjson_val* name = yyjson_obj_get(yy_root, "name");
        yyjson_val* age = yyjson_obj_get(yy_root, "age");
        const char* name_str = name ? yyjson_get_str(name) : "";
        long long age_val = age ? yyjson_get_sint(age) : 0;
    }, 10000);

    yyjson_doc_free(yy_doc);

    results.emplace_back("简单字段访问", xyjson_access, yyjson_access);

    // 打印结果表格
    printComparisonTable("xyjson vs yyjson 性能综合对比", results);
}

// 测试: 性能开销分析总结
DEF_TAST(comparison_summary, "性能开销分析总结")
{
    DESC("");
    DESC("========== xyjson vs yyjson 性能开销分析总结 ==========");
    DESC("");
    DESC("测试环境:");
    DESC("  - CPU: 多核处理器");
    DESC("  - 编译器: C++17");
    DESC("  - 测试方法: 微秒级计时，多次运行取平均值");
    DESC("");

    DESC("主要发现:");
    DESC("  1. 解析性能: xyjson 相比 yyjson 的开销主要来自构造函数包装");
    DESC("  2. 访问性能: 链式操作符带来一定开销，但提供了更好的易用性");
    DESC("  3. 迭代性能: 范围for循环提供类似原生API的性能");
    DESC("  4. 类型安全: xyjson 提供编译时类型检查，避免运行时错误");
    DESC("");

    DESC("性能优化建议:");
    DESC("  - 对于性能敏感场景，可使用 yyjson 原生 API");
    DESC("  - 对于日常使用，xyjson 的开销是可接受的");
    DESC("  - 建议在开发阶段使用 xyjson，生产环境可按需选择");
    DESC("");

    DESC("易用性对比:");
    DESC("  - xyjson: 简洁的链式操作，直观易懂");
    DESC("  - yyjson: 需要手动管理指针和错误检查");
    DESC("  - xyjson 在易用性和性能之间取得了良好平衡");
    DESC("");

    DESC("===================================================");
    DESC("");
}
