/**
 * @file perf_common.cpp
 * @author xyjson project
 * @date 2025-11-07
 * @brief 性能测试通用辅助函数实现
 */

#include "perf_common.h"
#include "couttast/couttast.h"
#include <fstream>
#include <sstream>
#include <chrono>

namespace perf {

// 读取文件内容
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 多次运行测试并计算总时间
long long measurePerformance(const std::string& name,
                             std::function<void()> test_func,
                             int iterations) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        test_func();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();

    DESC("  %s: %ld μs (总时间: %ld μs, 次数: %d, 平均: %.2f μs)",
          name.c_str(), duration / iterations, duration, iterations, (double)duration / iterations);
    return duration;
}

// 打印性能对比结果
void printComparison(const std::string& category,
                     const std::string& test_name,
                     long long xyjson_total_us,
                     long long yyjson_total_us,
                     int iterations) {
    double xyjson_avg = (double)xyjson_total_us / iterations;
    double yyjson_avg = (double)yyjson_total_us / iterations;
    double overhead = xyjson_avg / yyjson_avg;
    double overhead_percent = (overhead - 1.0) * 100.0;

    DESC("性能对比 - %s", category.c_str());
    DESC("  测试: %s (循环 %d 次)", test_name.c_str(), iterations);
    DESC("  xyjson:  %.2f μs/次 (总: %ld μs)", xyjson_avg, xyjson_total_us);
    DESC("  yyjson:  %.2f μs/次 (总: %ld μs)", yyjson_avg, yyjson_total_us);
    DESC("  开销比:  %.2fx", overhead);
    DESC("  开销%:   %.1f%%", overhead_percent);
    DESC("");
}

// 打印表格格式的对比结果
void printComparisonTable(const std::string& title,
                          const std::vector<std::tuple<std::string, long long, long long>>& results) {
    DESC("");
    DESC("========== %s ==========", title.c_str());
    DESC("%-40s | %12s | %12s | %10s", "测试项目", "xyjson(μs)", "yyjson(μs)", "开销比");
    DESC("%-40s-+-%12s-+-%12s-+-%10s", "", "", "", "");

    for (const auto& result : results) {
        const std::string& name = std::get<0>(result);
        long long xyjson_us = std::get<1>(result);
        long long yyjson_us = std::get<2>(result);
        double overhead = (double)xyjson_us / yyjson_us;

        DESC("%-40s | %12ld | %12ld | %10.2f",
              name.c_str(), xyjson_us, yyjson_us, overhead);
    }
    DESC("========================================");
    DESC("");
}

} // namespace perf
