/**
 * @file p_iterator.cpp
 * @author xyjson project
 * @date 2025-11-07
 * @brief 迭代器性能测试 - 简化版
 */

#include "couttast/couttast.h"
#include "xyjson.h"
#include <yyjson.h>
#include <string>
#include "perf_common.h"

using namespace yyjson;
using perf::measurePerformance;
using perf::printComparison;

// 测试 1: 小数组遍历对比
DEF_TAST(iterator_small_array, "小数组遍历对比")
{
    std::string jsonText = R"json({
        "numbers": [1,2,3,4,5,6,7,8,9,10]
    })json";

    Document doc(jsonText);

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        auto numbers = doc / "numbers" | kArray;
        long long sum = 0;
        for (auto it = numbers.begin(); it != numbers.end(); ++it) {
            sum += *it | 0;
        }
    }, 10000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* numbers = yyjson_obj_get(yy_root, "numbers");
        long long sum = 0;
        size_t idx, count;
        yyjson_val* val;
        yyjson_arr_foreach(numbers, idx, count, val) {
            sum += yyjson_get_sint(val);
        }
    }, 10000);

    yyjson_doc_free(yy_doc);

    printComparison("小数组遍历", "10个元素的数组", xyjson_total, yyjson_total, 10000);
}

// 测试 2: 对象数组遍历对比
DEF_TAST(iterator_array_objects, "对象数组遍历对比")
{
    std::string jsonText = R"json({
        "employees": [
            {"name": "Alice", "age": 30, "salary": 90000},
            {"name": "Bob", "age": 25, "salary": 75000},
            {"name": "Carol", "age": 35, "salary": 105000}
        ]
    })json";

    Document doc(jsonText);

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        auto employees = doc / "employees" | kArray;
        long long total_salary = 0;
        for (auto it = employees.begin(); it != employees.end(); ++it) {
            total_salary += (*it) / "salary" | 0;
        }
    }, 5000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* employees = yyjson_obj_get(yy_root, "employees");
        long long total_salary = 0;
        size_t idx, count;
        yyjson_val* emp;
        yyjson_arr_foreach(employees, idx, count, emp) {
            yyjson_val* salary = yyjson_obj_get(emp, "salary");
            total_salary += salary ? yyjson_get_sint(salary) : 0;
        }
    }, 5000);

    yyjson_doc_free(yy_doc);

    printComparison("对象数组遍历", "3个员工对象", xyjson_total, yyjson_total, 10000);
}
