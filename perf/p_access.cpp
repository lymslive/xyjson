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

DEF_TAST(access_simple, "简单字段访问对比")
{
    std::string jsonText = R"json({
        "name": "John Doe",
        "age": 30,
        "email": "john@example.com",
        "is_active": true
    })json";

    Document doc(jsonText);
    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    
    bool passed = relativePerformance(
        "xyjson simple access",
        [&doc]() {
            auto root = *doc;
            const char* name = root / "name" | "";
            int age = root / "age" | 0;
            const char* email = root / "email" | "";
            bool active = root / "is_active" | false;
            COUTF(age, 30);
        },
        "yyjson simple access",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* name = yyjson_obj_get(root, "name");
            yyjson_val* age = yyjson_obj_get(root, "age");
            yyjson_val* email = yyjson_obj_get(root, "email");
            yyjson_val* is_active = yyjson_obj_get(root, "is_active");
            const char* name_str = yyjson_is_str(name) ? yyjson_get_str(name) : "";
            int age_val = yyjson_is_int(age) ? yyjson_get_int(age) : 0;
            const char* email_str = yyjson_is_str(email) ? yyjson_get_str(email) : "";
            bool active_val = yyjson_is_bool(is_active) ? yyjson_get_bool(is_active) : false;
            COUTF(age_val, 30);
        },
        10000
    );

    yyjson_doc_free(yy_doc);
    COUT(passed, true);
}

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
    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    
    bool passed = relativePerformance(
        "xyjson nested access",
        [&doc]() {
            const char* name = doc / "user" / "profile" / "name" | "";
            int age = doc / "user" / "profile" / "age" | 0;
            const char* city = doc / "user" / "profile" / "address" / "city" | "";
            const char* zip = doc / "user" / "profile" / "address" / "zip" | "";
            COUTF(age, 30);
        },
        "yyjson nested access",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* user = yyjson_obj_get(root, "user");
            yyjson_val* profile = user ? yyjson_obj_get(user, "profile") : NULL;
            yyjson_val* name = profile ? yyjson_obj_get(profile, "name") : NULL;
            yyjson_val* age = profile ? yyjson_obj_get(profile, "age") : NULL;
            yyjson_val* address = profile ? yyjson_obj_get(profile, "address") : NULL;
            yyjson_val* city = address ? yyjson_obj_get(address, "city") : NULL;
            yyjson_val* zip = address ? yyjson_obj_get(address, "zip") : NULL;
            const char* name_str = yyjson_is_str(name) ? yyjson_get_str(name) : "";
            int age_val = yyjson_is_int(age) ? yyjson_get_int(age) : 0;
            const char* city_str = yyjson_is_str(city) ? yyjson_get_str(city) : "";
            const char* zip_str = yyjson_is_str(zip) ? yyjson_get_str(zip) : "";
            COUTF(age_val, 30);
        },
        10000
    );

    yyjson_doc_free(yy_doc);
    COUT(passed, true);
}

DEF_TAST(access_array_10, "数组元素访问对比(10个元素)")
{
    std::string jsonText = R"json({
        "scores": [95, 87, 92, 78, 88, 91, 85, 93, 89, 90]
    })json";

    Document doc(jsonText);
    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    
    bool passed = relativePerformance(
        "xyjson array access",
        [&doc]() {
            auto scores = doc / "scores";
            int sum = 0;
            size_t count = scores.size();
            for (size_t i = 0; i < count; ++i) {
                sum += scores[i] | 0;
            }
        },
        "yyjson array access",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* scores = yyjson_obj_get(root, "scores");
            long long sum = 0;
            size_t count = yyjson_arr_size(scores);
            for (size_t i = 0; i < count; ++i) {
                yyjson_val* val = yyjson_arr_get(scores, i);
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
        },
        10000
    );

    yyjson_doc_free(yy_doc);
    COUT(passed, true);
}

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
    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    
    bool passed = relativePerformance(
        "xyjson object array access",
        [&doc]() {
            auto employees = doc / "employees";
            int total_age = 0;
            size_t count = employees.size();
            for (size_t i = 0; i < count; ++i) {
                total_age += employees[i] / "age" | 0;
            }
        },
        "yyjson object array access",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* employees = yyjson_obj_get(root, "employees");
            long long total_age = 0;
            size_t count = yyjson_arr_size(employees);
            for (size_t i = 0; i < count; ++i) {
                yyjson_val* emp = yyjson_arr_get(employees, i);
                yyjson_val* age = yyjson_obj_get(emp, "age");
                total_age += yyjson_is_int(age) ? yyjson_get_int(age) : 0;
            }
        },
        10000
    );

    yyjson_doc_free(yy_doc);
    COUT(passed, true);
}

DEF_TAST(access_complex_file, "复杂文件访问对比")
{
    std::ifstream file("perf/datasets/medium.json");
    std::stringstream buffer;
    buffer << file.rdbuf();
    Document doc(buffer.str());
    
    std::string jsonText = buffer.str();
    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    
    bool passed = relativePerformance(
        "xyjson complex file access",
        [&doc]() {
            int total_employees = doc / "metrics" / "total_employees" | 0;
            auto employees = doc / "employees";
            int count = 0;
            for (size_t i = 0; i < employees.size() && i < 10; ++i) {
                int salary = employees[i] / "salary" | 0;
                count += salary > 0 ? 1 : 0;
            }
        },
        "yyjson complex file access",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* metrics = yyjson_obj_get(root, "metrics");
            long long total_employees = metrics ? yyjson_get_int(yyjson_obj_get(metrics, "total_employees")) : 0;
            yyjson_val* employees = yyjson_obj_get(root, "employees");
            size_t count = 0;
            if (employees) {
                size_t emp_count = yyjson_arr_size(employees);
                for (size_t i = 0; i < emp_count && i < 10; ++i) {
                    yyjson_val* emp = yyjson_arr_get(employees, i);
                    yyjson_val* salary = yyjson_obj_get(emp, "salary");
                    int salary_val = yyjson_is_int(salary) ? yyjson_get_int(salary) : 0;
                    count += salary_val > 0 ? 1 : 0;
                }
            }
        },
        1000
    );

    yyjson_doc_free(yy_doc);
    COUT(passed, true);
}

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
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
            COUTF(sum, 4950);
        }
    );

    COUT(passed, true);
}

DEF_TAST(access_object_100, "对象访问对比(100个属性)")
{
    Document doc = createJsonContainer(100);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "xyjson object access",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object";
            size_t count = obj.size();
            for (size_t i = 0; i < count; ++i) {
                std::string key = "k" + std::to_string(i);
                sum += obj / key.c_str() | 0; // 通过键名访问对象值
            }
            COUTF(sum, 4950); // 验证业务正确性: 0+1+...+99 = 4950
        },
        "yyjson object access",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* obj = yyjson_obj_get(root, "object");
            
            long long sum = 0;
            size_t count = yyjson_obj_size(obj);
            for (size_t i = 0; i < count; ++i) {
                std::string key = "k" + std::to_string(i);
                yyjson_val* val = yyjson_obj_getn(obj, key.c_str(), key.size());
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
            COUTF(sum, 4950);
        },
        100
    );

    COUT(passed, true);
}

DEF_TAST(access_object_500, "对象访问对比(500个属性)")
{
    Document doc = createJsonContainer(500);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "xyjson object access (500)",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object";
            size_t count = obj.size();
            for (size_t i = 0; i < count; ++i) {
                std::string key = "k" + std::to_string(i);
                sum += obj / key.c_str() | 0; // 通过键名访问对象值
            }
            COUTF(sum, 124750); // 验证业务正确性: 0+1+...+499 = 124750
        },
        "yyjson object access (500)",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* obj = yyjson_obj_get(root, "object");
            
            long long sum = 0;
            size_t count = yyjson_obj_size(obj);
            for (size_t i = 0; i < count; ++i) {
                std::string key = "k" + std::to_string(i);
                yyjson_val* val = yyjson_obj_getn(obj, key.c_str(), key.size());
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
            COUTF(sum, 124750);
        },
        50
    );

    COUT(passed, true);
}

// 测试 10: 1000 元素对象访问对比
DEF_TAST(access_object_1000, "对象访问对比(1000个属性)")
{
    Document doc = createJsonContainer(1000);
    yyjson_doc* yy_doc = doc.c_doc();
    
    bool passed = relativePerformance(
        "xyjson object access (1000)",
        [&doc]() {
            long long sum = 0;
            auto obj = doc / "object";
            size_t count = obj.size();
            for (size_t i = 0; i < count; ++i) {
                std::string key = "k" + std::to_string(i);
                sum += obj / key.c_str() | 0; // 通过键名访问对象值
            }
            COUTF(sum, 499500); // 验证业务正确性: 0+1+...+999 = 499500
        },
        "yyjson object access (1000)",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* obj = yyjson_obj_get(root, "object");
            
            long long sum = 0;
            size_t count = yyjson_obj_size(obj);
            for (size_t i = 0; i < count; ++i) {
                std::string key = "k" + std::to_string(i);
                yyjson_val* val = yyjson_obj_getn(obj, key.c_str(), key.size());
                sum += yyjson_is_int(val) ? yyjson_get_int(val) : 0;
            }
            COUTF(sum, 499500);
        },
        20
    );

    COUT(passed, true);
}

