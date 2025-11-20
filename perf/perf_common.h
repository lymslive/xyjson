/**
 * @file perf_common.h
 * @author xyjson project
 * @date 2025-11-07
 * @brief 性能测试通用辅助函数声明
 */

#ifndef PERF_COMMON_H
#define PERF_COMMON_H

#include "xyjson.h"

#include <string>
#include <functional>

namespace perf {

// 读取文件内容
std::string readFile(const std::string& filename);

// 多次运行测试并计算平均时间（微秒）
long long measurePerformance(const std::string& name,
                             std::function<void()> test_func,
                             int iterations = 1000);


// 相对性能测试函数
bool relativePerformance(const std::string& test_name,
                        std::function<void()> test_func,
                        const std::string& base_name,
                        std::function<void()> base_func,
                        int iterations = 100,
                        long long min_time_ms = 0,
                        double overhead_percent = 0.0);

// 创建包含数组和对象的JSON容器
yyjson::Document createJsonContainer(int n);

} // namespace perf

#endif // PERF_COMMON_H
