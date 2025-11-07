/**
 * @file perf_common.h
 * @author xyjson project
 * @date 2025-11-07
 * @brief 性能测试通用辅助函数声明
 */

#ifndef PERF_COMMON_H
#define PERF_COMMON_H

#include <string>
#include <functional>

namespace perf {

// 读取文件内容
std::string readFile(const std::string& filename);

// 多次运行测试并计算平均时间（微秒）
long long measurePerformance(const std::string& name,
                             std::function<void()> test_func,
                             int iterations = 1000);

// 打印性能对比结果
void printComparison(const std::string& category,
                     const std::string& test_name,
                     long long xyjson_total_us,
                     long long yyjson_total_us,
                     int iterations);

// 打印表格格式的对比结果
void printComparisonTable(const std::string& title,
                          const std::vector<std::tuple<std::string, long long, long long>>& results);

} // namespace perf

#endif // PERF_COMMON_H
