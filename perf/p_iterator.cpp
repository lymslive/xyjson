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
DEF_TAST(iterator_array_10, "小数组遍历对比(10个元素)")
{
    std::string jsonText = R"json({
        "numbers": [1,2,3,4,5,6,7,8,9,10]
    })json";

    Document doc(jsonText);

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
//      auto numbers = doc / "numbers" | kArray;
        long long sum = 0;
//      for (auto it = numbers.begin(); it != numbers.end(); ++it) {
        for (auto it = doc / "numbers" % 0; it; ++it) {
            sum += *it | 0;
        }
        COUTF(sum, 55);
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
        COUTF(sum, 55);
    }, 10000);

    yyjson_doc_free(yy_doc);

    printComparison("小数组遍历", "10个元素的数组", xyjson_total, yyjson_total, 10000);
}

// 测试 2: 对象数组遍历对比
DEF_TAST(iterator_array_objects_3, "对象数组遍历对比(3个对象)")
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
//      auto employees = doc / "employees" | kArray;
        long long total_salary = 0;
//      for (auto it = employees.begin(); it != employees.end(); ++it) {
        for (auto it = doc / "employees" % 0; it; ++it) {
            total_salary += (*it) / "salary" | 0;
        }
        COUTF(total_salary, 270000);
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
        COUTF(total_salary, 270000);
    }, 5000);

    yyjson_doc_free(yy_doc);

    printComparison("对象数组遍历", "3个员工对象", xyjson_total, yyjson_total, 10000);
}

// 测试 3: 100 元素数组迭代器对比
DEF_TAST(iterator_array_100, "小数组遍历对比(100个元素)")
{
    // Generate 100 numbers
    std::string jsonText = "{\"numbers\": [";
    for (int i = 0; i < 100; ++i) {
        jsonText += std::to_string(i + 1);
        if (i < 99) jsonText += ",";
    }
    jsonText += "]}";

    Document doc(jsonText);
    if (!doc) {
        DESC("Error: Failed to parse JSON");
        return;
    }

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        long long sum = 0;
        size_t count = 0;
        for (auto it = doc / "numbers" % 0; it; ++it) {
            sum += *it | 0;
            ++count;
        }
        COUTF(count, 100);
        COUTF(sum, 5050);
    }, 1000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);
    if (!yy_root) {
        yyjson_doc_free(yy_doc);
        DESC("Error: yyjson failed to parse");
        return;
    }

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* numbers = yyjson_obj_get(yy_root, "numbers");
        long long sum = 0;
        size_t idx, count;
        yyjson_val* val;
        yyjson_arr_foreach(numbers, idx, count, val) {
            sum += yyjson_get_sint(val);
        }
        COUTF(count, 100);
        COUTF(sum, 5050);
    }, 1000);

    yyjson_doc_free(yy_doc);

    printComparison("小数组遍历", "100个元素的数组", xyjson_total, yyjson_total, 1000);
}

// 测试 4: 100 对象数组迭代器对比
DEF_TAST(iterator_array_objects_100, "对象数组遍历对比(100个对象)")
{
    // Generate 100 employee objects
    std::string jsonText = "{\"employees\": [";
    for (int i = 0; i < 100; ++i) {
        jsonText += "{\"name\":\"Employee" + std::to_string(i) + "\",\"age\":" + std::to_string(25 + (i % 30)) + ",\"salary\":" + std::to_string(50000 + i * 1000) + "}";
        if (i < 99) jsonText += ",";
    }
    jsonText += "]}";

    Document doc(jsonText);
    if (!doc) {
        DESC("Error: Failed to parse JSON");
        return;
    }

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        long long total_salary = 0;
        size_t count = 0;
        for (auto it = doc / "employees" % 0; it; ++it) {
            total_salary += (*it) / "salary" | 0;
            ++count;
        }
        COUTF(count, 100);
        COUTF(total_salary, 9950000);
    }, 1000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);
    if (!yy_root) {
        yyjson_doc_free(yy_doc);
        DESC("Error: yyjson failed to parse");
        return;
    }

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* employees = yyjson_obj_get(yy_root, "employees");
        long long total_salary = 0;
        size_t idx, count;
        yyjson_val* emp;
        yyjson_arr_foreach(employees, idx, count, emp) {
            yyjson_val* salary = yyjson_obj_get(emp, "salary");
            total_salary += salary ? yyjson_get_sint(salary) : 0;
        }
        COUTF(count, 100);
        COUTF(total_salary, 9950000);
    }, 1000);

    yyjson_doc_free(yy_doc);

    printComparison("对象数组遍历", "100个员工对象", xyjson_total, yyjson_total, 1000);
}
