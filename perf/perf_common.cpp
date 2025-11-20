/**
 * @file perf_common.cpp
 * @author xyjson project
 * @date 2025-11-07
 * @brief 性能测试通用辅助函数实现
 */

#include "perf_common.h"
#include "couttast/couttast.h"
#include "couttast/tastargv.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>

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

    return duration;
}

// 调整循环次数到合适的值（取整并保证两个有效数字）
// 调整循环次数到合适的值（向上圆整，保留两位有效数字）
int adjustIterations(int base_iterations, long long total_time_us, long long min_time_ms) {
    if (min_time_ms <= 0 || total_time_us >= min_time_ms * 1000) {
        return base_iterations;
    }
    
    // 计算需要增加的倍数
    double multiplier = (min_time_ms * 1000.0) / total_time_us;
    
    // 计算理论上的迭代次数
    double target_iterations = base_iterations * multiplier;
    
    // 向上取整到整数
    int rounded = (int)std::ceil(target_iterations);
    
    // 获取数值的位数和幂次
    int power = (int)std::floor(std::log10(rounded));
    int magnitude = (int)std::pow(10, power);
    
    // 获取最高位和次高位
    int first_digit = rounded / magnitude;
    int second_digit = (rounded % magnitude) / (magnitude / 10);
    
    // 根据次高位进行调整
    if (second_digit <= 2) {
        // 次高位是0-2，调整为2
        return first_digit * magnitude + 2 * (magnitude / 10);
    } else if (second_digit <= 5) {
        // 次高位是3-5，调整为5
        return first_digit * magnitude + 5 * (magnitude / 10);
    } else {
        // 次高位是6-9，进位到下一个值
        if (first_digit < 9) {
            return (first_digit + 1) * magnitude;
        } else {
            // 如果最高位是9，进位到10的更高幂次
            return 10 * magnitude;
        }
    }
}

// 相对性能测试函数
// 返回是否通过性能测试
// 接收命令行参数：
//   --runtime_ms=N    最小运行时间，默认200ms
//   --overhead_percent=P 允许的最大开销百分比，默认5.0
//   --no_loop=1       只运行一次测试，不进行性能对比
bool relativePerformance(const std::string& test_name,
                        std::function<void()> test_func,
                        const std::string& base_name,
                        std::function<void()> base_func,
                        int iterations,
                        long long min_time_ms,
                        double overhead_percent) {
    
    // 从命令行参数读取默认值
    long long runtime_ms = 200;
    double overhead_threshold = 5.0;
    int no_loop = 0;
    
    // 尝试绑定命令行参数
    BIND_ARGV(runtime_ms, "runtime_ms");
    BIND_ARGV(overhead_threshold, "overhead_percent");
    BIND_ARGV(no_loop, "no_loop");

    // 使用命令行参数或默认值
    if (min_time_ms == 0) min_time_ms = runtime_ms;
    if (overhead_percent == 0.0) overhead_percent = overhead_threshold;
    
    if (no_loop != 0)
    {
        iterations = 1;
    }

    int final_iterations = iterations;

    // 第一次运行测试函数
    long long test_time_us = measurePerformance(test_name, test_func, iterations);
    
    // 如果需要，调整迭代次数
    if (no_loop == 0 && min_time_ms > 0)
    {
        final_iterations = adjustIterations(iterations, test_time_us, min_time_ms);
        if (final_iterations != iterations) {
            DESC("调整迭代次数: %d -> %d", iterations, final_iterations);
            test_time_us = measurePerformance(test_name, test_func, final_iterations);
        }
    }
    
    // 运行基准函数
    long long base_time_us = measurePerformance(base_name, base_func, final_iterations);
    
    // 如果只测试一次， 只测试函数功能，不测性能，直接返回成功
    if (no_loop != 0)
    {
        return true;
    }

    // 计算性能指标
    double test_avg = (double)test_time_us / final_iterations;
    double base_avg = (double)base_time_us / final_iterations;
    double overhead = test_avg / base_avg;
    double overhead_pct = (overhead - 1.0) * 100.0;
    
    // 打印对比结果
    DESC("相对性能对比 - %s", test_name.c_str());
    DESC("  测试函数 (%s): %.2f μs/次 (总: %ld μs, 次数: %d)", 
          test_name.c_str(), test_avg, test_time_us, final_iterations);
    DESC("  基准函数 (%s): %.2f μs/次 (总: %ld μs, 次数: %d)", 
          base_name.c_str(), base_avg, base_time_us, final_iterations);
    DESC("  开销比: %.2fx", overhead);
    DESC("  开销%%: %.1f%%", overhead_pct);
    
    // 检查是否满足性能要求
    bool passed = overhead_pct <= overhead_percent;
    if (passed) {
        DESC("  ✓ 性能测试通过 (允许开销: %.1f%%)", overhead_percent);
    } else {
        DESC("  ✗ 性能测试不通过 (允许开销: %.1f%%)", overhead_percent);
    }
    DESC("");
    
    return passed;
}

// 创建包含数组和对象的JSON容器
yyjson::Document createJsonContainer(int n) {
    // 创建可变文档
    yyjson::MutableDocument mutDoc;
    
    // 创建数组
    yyjson::MutableValue array = mutDoc * yyjson::kArray;
    for (int i = 0; i < n; ++i) {
        array.append(i);
    }
    
    // 创建对象
    yyjson::MutableValue obj = mutDoc * yyjson::kObject;
    for (int i = 0; i < n; ++i) {
        std::string key = "k" + std::to_string(i);
        obj.add(key.c_str(), i);
    }
    
    // 构建最终的JSON文档
    yyjson::MutableValue root = mutDoc.root();
    root.add("array", array);
    root.add("object", obj);
    
    // 转换为只读文档
    return yyjson::Document(mutDoc);
}

} // namespace perf
