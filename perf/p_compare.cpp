/**
 * @file p_compare.cpp
 * @author xyjson project
 * @date 2025-11-23
 * @brief 性能测试 - 对比 xyjson 不同写法的性能
 */

#include "couttast/couttast.h"
#include "xyjson.h"
#include "perf_common.h"

#include <string>

using namespace yyjson;
using namespace perf;

/* 一些结论：
 * 只读数组的迭代器访问比循环索引慢，可写数组迭代器比循环索引快
 * 对象因循环索引要拼接 key 字符串，慢很多，迭代器更快。
 */

DEF_TOOL(compare_array_100, "数组访问对比迭代器与循环索引")
{
    Document doc = createJsonContainer(100);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "iterator method",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array" | kArray;
            for (auto it = array.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        },
        "path method",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array";
            size_t count = array.size();
            for (size_t i = 0; i < count; ++i) {
                sum += array[i] | 0;
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        }
    );

    COUTF(passed, true);
}

DEF_TOOL(compare_object_100, "对象访问对比迭代器与循环索引")
{
    Document doc = createJsonContainer(100);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "iterator method",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object" | kObject;
            for (auto it = obj.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        },
        "path method",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object";
            size_t count = obj.size();
            for (size_t i = 0; i < count; ++i) {
                std::string key = "k" + std::to_string(i);
                sum += obj / key.c_str() | 0; // 通过键名访问对象值
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        }
    );

    COUTF(passed, true);
}

DEF_TOOL(compare_mutarr_100, "数组访问对比迭代器与循环索引")
{
    MutableDocument doc = createMutableJsonContainer(100);
    
    bool passed = relativePerformance(
        "iterator method",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array" | kArray;
            for (auto it = array.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        },
        "path method",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array";
            size_t count = array.size();
            for (size_t i = 0; i < count; ++i) {
                sum += array[i] | 0;
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        }
    );

    COUTF(passed, true);
}

DEF_TOOL(compare_mutobj_100, "对象访问对比迭代器与循环索引")
{
    MutableDocument doc = createMutableJsonContainer(100);
    
    bool passed = relativePerformance(
        "iterator method",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object" | kObject;
            for (auto it = obj.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        },
        "path method",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object";
            size_t count = obj.size();
            for (size_t i = 0; i < count; ++i) {
                std::string key = "k" + std::to_string(i);
                sum += obj / key.c_str() | 0; // 通过键名访问对象值
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        }
    );

    COUTF(passed, true);
}

DEF_TOOL(compare_array_500, "数组访问对比迭代器与循环索引")
{
    Document doc = createJsonContainer(500);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "iterator method",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array" | kArray;
            for (auto it = array.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 124750); // 验证业务正确性: 0+1+...+499 = 124750
        },
        "path method",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array";
            size_t count = array.size();
            for (size_t i = 0; i < count; ++i) {
                sum += array[i] | 0;
            }
            COUTF(sum, 124750); // 验证业务正确性: 0+1+...+499 = 124750
        }
    );

    COUTF(passed, true);
}

DEF_TOOL(compare_array_1000, "数组访问对比迭代器与循环索引")
{
    Document doc = createJsonContainer(1000);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "iterator method",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array" | kArray;
            for (auto it = array.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 499500); // 验证业务正确性: 0+1+...+999 = 499500
        },
        "path method",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array";
            size_t count = array.size();
            for (size_t i = 0; i < count; ++i) {
                sum += array[i] | 0;
            }
            COUTF(sum, 499500); // 验证业务正确性: 0+1+...+999 = 499500
        }
    );

    COUTF(passed, true);
}

