/**
 * @file p_access.cpp
 * @author xyjson project
 * @date 2025-11-07
 * @brief 访问性能测试 - 对比 xyjson 与 yyjson 原生 API 的数据访问性能
 */

#include "couttast/couttast.h"
#include "xyjson.h"
#include <yyjson.h>
#include <string>
#include "perf_common.h"

using namespace yyjson;
using namespace perf;

// 测试 1: 简单字段访问对比
DEF_TAST(access_simple, "简单字段访问对比")
{
    std::string jsonText = R"json({
        "name": "John Doe",
        "age": 30,
        "email": "john@example.com",
        "is_active": true
    })json";

    Document doc(jsonText);

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        const char* name = doc / "name" | "";
        int age = doc / "age" | 0;
        const char* email = doc / "email" | "";
        bool active = doc / "is_active" | false;
    }, 10000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* name = yyjson_obj_get(yy_root, "name");
        yyjson_val* age = yyjson_obj_get(yy_root, "age");
        yyjson_val* email = yyjson_obj_get(yy_root, "email");
        yyjson_val* is_active = yyjson_obj_get(yy_root, "is_active");
        const char* name_str = name ? yyjson_get_str(name) : "";
        long long age_val = age ? yyjson_get_sint(age) : 0;
        const char* email_str = email ? yyjson_get_str(email) : "";
        bool active_val = is_active ? yyjson_get_bool(is_active) : false;
    }, 10000);

    yyjson_doc_free(yy_doc);

    printComparison("简单字段访问", "4个字段(name,age,email,active)", xyjson_total, yyjson_total, 10000);
}

// 测试 2: 嵌套字段访问对比
DEF_TAST(access_nested, "嵌套字段访问对比")
{
    std::string jsonText = R"json({
        "user": {
            "profile": {
                "name": "John Doe",
                "age": 30,
                "address": {
                    "city": "New York",
                    "zip": "10001"
                }
            }
        }
    })json";

    Document doc(jsonText);

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        const char* name = doc / "user" / "profile" / "name" | "";
        int age = doc / "user" / "profile" / "age" | 0;
        const char* city = doc / "user" / "profile" / "address" / "city" | "";
        const char* zip = doc / "user" / "profile" / "address" / "zip" | "";
    }, 10000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* user = yyjson_obj_get(yy_root, "user");
        yyjson_val* profile = user ? yyjson_obj_get(user, "profile") : NULL;
        yyjson_val* name = profile ? yyjson_obj_get(profile, "name") : NULL;
        yyjson_val* age = profile ? yyjson_obj_get(profile, "age") : NULL;
        yyjson_val* address = profile ? yyjson_obj_get(profile, "address") : NULL;
        yyjson_val* city = address ? yyjson_obj_get(address, "city") : NULL;
        yyjson_val* zip = address ? yyjson_obj_get(address, "zip") : NULL;
        const char* name_str = name ? yyjson_get_str(name) : "";
        long long age_val = age ? yyjson_get_sint(age) : 0;
        const char* city_str = city ? yyjson_get_str(city) : "";
        const char* zip_str = zip ? yyjson_get_str(zip) : "";
    }, 10000);

    yyjson_doc_free(yy_doc);

    printComparison("嵌套字段访问", "4层嵌套访问", xyjson_total, yyjson_total, 10000);
}

// 测试 3: 数组元素访问对比
DEF_TAST(access_array_10, "数组元素访问对比(10个元素)")
{
    std::string jsonText = R"json({
        "scores": [95, 87, 92, 78, 88, 91, 85, 93, 89, 90]
    })json";

    Document doc(jsonText);

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        auto scores = doc / "scores";
        int sum = 0;
        size_t count = scores.size();
        for (size_t i = 0; i < count; ++i) {
            sum += scores[i] | 0;
        }
    }, 10000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* scores = yyjson_obj_get(yy_root, "scores");
        long long sum = 0;
        size_t count = yyjson_arr_size(scores);
        for (size_t i = 0; i < count; ++i) {
            yyjson_val* val = yyjson_arr_get(scores, i);
            sum += yyjson_get_sint(val);
        }
    }, 10000);

    yyjson_doc_free(yy_doc);

    printComparison("数组元素访问", "10个元素的数组遍历", xyjson_total, yyjson_total, 10000);
}

// 测试 4: 对象数组访问对比
DEF_TAST(access_array_objects_3, "对象数组访问对比(3个对象)")
{
    std::string jsonText = R"json({
        "employees": [
            {"name": "Alice", "age": 30},
            {"name": "Bob", "age": 25},
            {"name": "Carol", "age": 35}
        ]
    })json";

    Document doc(jsonText);

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        auto employees = doc / "employees";
        int total_age = 0;
        size_t count = employees.size();
        for (size_t i = 0; i < count; ++i) {
            total_age += employees[i] / "age" | 0;
        }
    }, 10000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* employees = yyjson_obj_get(yy_root, "employees");
        long long total_age = 0;
        size_t count = yyjson_arr_size(employees);
        for (size_t i = 0; i < count; ++i) {
            yyjson_val* emp = yyjson_arr_get(employees, i);
            yyjson_val* age = yyjson_obj_get(emp, "age");
            total_age += age ? yyjson_get_sint(age) : 0;
        }
    }, 10000);

    yyjson_doc_free(yy_doc);

    printComparison("对象数组访问", "3个员工对象数组", xyjson_total, yyjson_total, 10000);
}

// 测试 5: 类型转换对比
DEF_TAST(access_type_conversion, "类型转换对比")
{
    std::string jsonText = R"json({
        "int_val": 42,
        "double_val": 3.14,
        "bool_val": true,
        "string_val": "hello"
    })json";

    Document doc(jsonText);

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        int int_val = doc / "int_val" | 0;
        double double_val = doc / "double_val" | 0.0;
        bool bool_val = doc / "bool_val" | false;
        const char* string_val = doc / "string_val" | "";
    }, 10000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* int_val = yyjson_obj_get(yy_root, "int_val");
        yyjson_val* double_val = yyjson_obj_get(yy_root, "double_val");
        yyjson_val* bool_val = yyjson_obj_get(yy_root, "bool_val");
        yyjson_val* string_val = yyjson_obj_get(yy_root, "string_val");
        long long int_v = int_val ? yyjson_get_sint(int_val) : 0;
        double double_v = double_val ? yyjson_get_real(double_val) : 0.0;
        bool bool_v = bool_val ? yyjson_get_bool(bool_val) : false;
        const char* string_v = string_val ? yyjson_get_str(string_val) : "";
    }, 10000);

    yyjson_doc_free(yy_doc);

    printComparison("类型转换", "int, double, bool, string", xyjson_total, yyjson_total, 10000);
}

// 测试 6: 复杂文件访问对比
DEF_TAST(access_complex_file, "复杂文件访问对比")
{
    std::ifstream file("perf/datasets/medium.json");
    std::stringstream buffer;
    buffer << file.rdbuf();
    Document doc(buffer.str());

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        int total_employees = doc / "metrics" / "total_employees" | 0;
        auto employees = doc / "employees";
        int count = 0;
        for (size_t i = 0; i < employees.size() && i < 10; ++i) {
            int salary = employees[i] / "salary" | 0;
            count += salary > 0 ? 1 : 0;
        }
    }, 1000);

    std::string jsonText = buffer.str();
    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* metrics = yyjson_obj_get(yy_root, "metrics");
        long long total_employees = metrics ? yyjson_get_sint(yyjson_obj_get(metrics, "total_employees")) : 0;
        yyjson_val* employees = yyjson_obj_get(yy_root, "employees");
        size_t count = 0;
        if (employees) {
            size_t emp_count = yyjson_arr_size(employees);
            for (size_t i = 0; i < emp_count && i < 10; ++i) {
                yyjson_val* emp = yyjson_arr_get(employees, i);
                yyjson_val* salary = yyjson_obj_get(emp, "salary");
                if (salary && yyjson_get_sint(salary) > 0) {
                    count++;
                }
            }
        }
    }, 1000);

    yyjson_doc_free(yy_doc);

    printComparison("复杂文件访问", "medium.json (30KB) 实际业务数据", xyjson_total, yyjson_total, 10000);
}

// 测试 7: 100 元素数组访问对比
DEF_TAST(access_array_100, "数组元素访问对比(100个元素)")
{
    Document doc = createJsonContainer(100);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "xyjson array access",
        [&doc]() {
            long long sum = 0;
            auto array = doc / "array";
            size_t count = array.size();
            for (size_t i = 0; i < count; ++i) {
                sum += array[i] | 0;
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        },
        "yyjson array access",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* array = yyjson_obj_get(root, "array");
            
            long long sum = 0;
            size_t count = yyjson_arr_size(array);
            for (size_t i = 0; i < count; ++i) {
                yyjson_val* val = yyjson_arr_get(array, i);
                sum += yyjson_get_sint(val);
            }
            COUTF(sum, 4950);
        }
    );

    COUT(passed, true);
}

