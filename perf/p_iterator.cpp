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
using namespace perf;

DEF_TAST(iterator_array_10, "小数组遍历对比(10个元素)")
{
    std::string jsonText = R"json({
        "numbers": [1,2,3,4,5,6,7,8,9,10]
    })json";

    Document doc(jsonText);
    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);
    
    bool passed = relativePerformance(
        "xyjson array iterator",
        [&doc]() {
            long long sum = 0;
            for (auto it = doc / "numbers" % 0; it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 55);
        },
        "yyjson array foreach",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* numbers = yyjson_obj_get(root, "numbers");
            long long sum = 0;
            size_t idx, count;
            yyjson_val* val;
            yyjson_arr_foreach(numbers, idx, count, val) {
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
            COUTF(sum, 55);
        },
        10000
    );

    yyjson_doc_free(yy_doc);
    COUT(passed, true);
}

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
    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);
    
    bool passed = relativePerformance(
        "xyjson object array iterator",
        [&doc]() {
            long long total_salary = 0;
            for (auto it = doc / "employees" % 0; it; ++it) {
                total_salary += (*it) / "salary" | 0;
            }
            COUTF(total_salary, 270000);
        },
        "yyjson object array foreach",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* employees = yyjson_obj_get(root, "employees");
            long long total_salary = 0;
            size_t idx, count;
            yyjson_val* emp;
            yyjson_arr_foreach(employees, idx, count, emp) {
                yyjson_val* salary = yyjson_obj_get(emp, "salary");
                total_salary += yyjson_is_int(salary) ? yyjson_get_int(salary) : 0;
            }
            COUTF(total_salary, 270000);
        },
        5000
    );

    yyjson_doc_free(yy_doc);
    COUT(passed, true);
}

DEF_TAST(iterator_array_100, "数组迭代器对比(100个元素)")
{
    Document doc = createJsonContainer(100);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "xyjson array iterator",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array" | kArray;
            // it != array.end() 比直接 it 判断慢一些
            for (auto it = array.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        },
        "yyjson array iterator",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* array = yyjson_obj_get(root, "array");
            
            long long sum = 0;
            size_t idx, max;
            yyjson_val* val;
            yyjson_arr_foreach(array, idx, max, val) {
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
            COUTF(sum, 4950);
        }
    );

    COUT(passed, true);
}

DEF_TAST(iterator_object_100, "对象迭代器对比(100个属性)")
{
    Document doc = createJsonContainer(100);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "xyjson object iterator",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object" | kObject;
            for (auto it = obj.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        },
        "yyjson object iterator",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* obj = yyjson_obj_get(root, "object");
            
            long long sum = 0;
            size_t idx, max;
            yyjson_val* key, *val;
            yyjson_obj_foreach(obj, idx, max, key, val) {
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
            COUTF(sum, 4950);
        },
        100
    );

    COUT(passed, true);
}

DEF_TAST(iterator_object_500, "对象迭代器对比(500个属性)")
{
    Document doc = createJsonContainer(500);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "xyjson object iterator (500)",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object" | kObject;
            for (auto it = obj.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 124750); // 验证业务正确性: 0+1+...+499 = 124750
        },
        "yyjson object iterator (500)",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* obj = yyjson_obj_get(root, "object");
            
            long long sum = 0;
            size_t idx, max;
            yyjson_val* key, *val;
            yyjson_obj_foreach(obj, idx, max, key, val) {
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
            COUTF(sum, 124750);
        },
        50
    );

    COUT(passed, true);
}

DEF_TAST(iterator_object_1000, "对象迭代器对比(1000个属性)")
{
    Document doc = createJsonContainer(1000);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "xyjson object iterator (1000)",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object" | kObject;
            for (auto it = obj.begin(); it; ++it) {
                sum += *it | 0;
            }
            COUTF(sum, 499500); // 验证业务正确性: 0+1+...+999 = 499500
        },
        "yyjson object iterator (1000)",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* obj = yyjson_obj_get(root, "object");
            
            long long sum = 0;
            size_t idx, max;
            yyjson_val* key, *val;
            yyjson_obj_foreach(obj, idx, max, key, val) {
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
            COUTF(sum, 499500);
        },
        20
    );

    COUT(passed, true);
}

