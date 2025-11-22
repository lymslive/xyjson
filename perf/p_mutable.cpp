/**
 * @file p_mutable.cpp
 * @author xyjson project
 * @date 2025-11-22
 * @brief 可变操作性能测试 - 测试 MutableDocument/MutableValue 的修改操作
 */

#include "couttast/couttast.h"
#include "xyjson.h"
#include <yyjson.h>
#include <string>
#include "perf_common.h"

using namespace yyjson;
using namespace perf;

DEF_TAST(mutable_set, "可变值设置对比")
{
    // 创建一个包含各种类型字段的简单 JSON
    std::string jsonText = R"json({
        "bool_field": false,
        "int_field": 0,
        "int64_field": 0,
        "uint64_field": 0,
        "double_field": 0.0,
        "string_field": ""
    })json";

    MutableDocument mutDoc(jsonText);
    MutableDocument mutDoc2(jsonText);
    yyjson_mut_doc* yy_mut_doc = mutDoc2.c_doc();
    
    bool passed = relativePerformance(
        "xyjson mutable set operations",
        [&mutDoc]() {
            // 设置各种基本类型
            (mutDoc / "bool_field") = true;
            (mutDoc / "int_field") = 42;
            (mutDoc / "int64_field") = (int64_t)1234567890;
            (mutDoc / "uint64_field") = (uint64_t)9876543210;
            (mutDoc / "double_field") = 3.14159;
            (mutDoc / "string_field") = "hello world";
            
            // 验证设置的值
            COUTF((mutDoc / "bool_field") | false, true);
            COUTF((mutDoc / "int_field") | 0, 42);
            COUTF((mutDoc / "double_field") | 0.0, 3.14159);
        },
        "yyjson mutable set operations",
        [yy_mut_doc]() {
            yyjson_mut_val* root = yyjson_mut_doc_get_root(yy_mut_doc);
            
            // 设置各种基本类型
            yyjson_mut_val* bool_field = yyjson_mut_obj_get(root, "bool_field");
            yyjson_mut_val* int_field = yyjson_mut_obj_get(root, "int_field");
            yyjson_mut_val* int64_field = yyjson_mut_obj_get(root, "int64_field");
            yyjson_mut_val* uint64_field = yyjson_mut_obj_get(root, "uint64_field");
            yyjson_mut_val* double_field = yyjson_mut_obj_get(root, "double_field");
            yyjson_mut_val* string_field = yyjson_mut_obj_get(root, "string_field");
            
            yyjson_mut_set_bool(bool_field, true);
            yyjson_mut_set_int(int_field, 42);
            yyjson_mut_set_sint(int64_field, 1234567890);
            yyjson_mut_set_uint(uint64_field, 9876543210);
            yyjson_mut_set_real(double_field, 3.14159);
            yyjson_mut_set_str(string_field, "hello world");
            
            // 验证设置的值
            COUTF(yyjson_mut_get_bool(bool_field), true);
            COUTF(yyjson_mut_get_int(int_field), 42);
            COUTF(yyjson_mut_get_real(double_field), 3.14159);
        },
        1000
    );

    COUTF(passed, true);
}

DEF_TAST(mutable_array_100, "可变数组构造对比(100个元素)")
{
    bool passed = relativePerformance(
        "xyjson mutable array construction",
        []() {
            MutableDocument mutDoc;
            auto array = mutDoc * kArray;
            
            // 使用 << 操作符构造数组
            for (int i = 0; i < 100; ++i) {
                array << i;
            }
            
            COUTF(array.size(), 100);
            COUTF((array[0] | 0), 0);
            COUTF((array[99] | 0), 99);
        },
        "yyjson mutable array construction",
        []() {
            yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
            yyjson_mut_val* array = yyjson_mut_arr(doc);
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_obj_add_val(doc, root, "array", array);
            yyjson_mut_doc_set_root(doc, root);
            
            // 使用 yyjson API 构造数组
            for (int i = 0; i < 100; ++i) {
                yyjson_mut_val* val = yyjson_mut_int(doc, i);
                yyjson_mut_arr_append(array, val);
            }
            
            COUTF(yyjson_mut_arr_size(array), 100);
            COUTF(yyjson_mut_get_int(yyjson_mut_arr_get(array, 0)), 0);
            COUTF(yyjson_mut_get_int(yyjson_mut_arr_get(array, 99)), 99);
            
            yyjson_mut_doc_free(doc);
        },
        100
    );

    COUTF(passed, true);
}

DEF_TAST(mutable_array_500, "可变数组构造对比(500个元素)")
{
    bool passed = relativePerformance(
        "xyjson mutable array construction (500)",
        []() {
            MutableDocument mutDoc;
            auto array = mutDoc * kArray;
            
            // 使用 << 操作符构造数组
            for (int i = 0; i < 500; ++i) {
                array << i;
            }
            
            COUTF(array.size(), 500);
            COUTF((array[0] | 0), 0);
            COUTF((array[499] | 0), 499);
        },
        "yyjson mutable array construction (500)",
        []() {
            yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
            yyjson_mut_val* array = yyjson_mut_arr(doc);
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_obj_add_val(doc, root, "array", array);
            yyjson_mut_doc_set_root(doc, root);
            
            // 使用 yyjson API 构造数组
            for (int i = 0; i < 500; ++i) {
                yyjson_mut_val* val = yyjson_mut_int(doc, i);
                yyjson_mut_arr_append(array, val);
            }
            
            COUTF(yyjson_mut_arr_size(array), 500);
            COUTF(yyjson_mut_get_int(yyjson_mut_arr_get(array, 0)), 0);
            COUTF(yyjson_mut_get_int(yyjson_mut_arr_get(array, 499)), 499);
            
            yyjson_mut_doc_free(doc);
        }
    );

    COUTF(passed, true);
}

DEF_TAST(mutable_array_1000, "可变数组构造对比(1000个元素)")
{
    bool passed = relativePerformance(
        "xyjson mutable array construction (1000)",
        []() {
            MutableDocument mutDoc;
            auto array = mutDoc * kArray;
            
            // 使用 << 操作符构造数组
            for (int i = 0; i < 1000; ++i) {
                array << i;
            }
            
            COUTF(array.size(), 1000);
            COUTF((array[0] | 0), 0);
            COUTF((array[999] | 0), 999);
        },
        "yyjson mutable array construction (1000)",
        []() {
            yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
            yyjson_mut_val* array = yyjson_mut_arr(doc);
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_obj_add_val(doc, root, "array", array);
            yyjson_mut_doc_set_root(doc, root);
            
            // 使用 yyjson API 构造数组
            for (int i = 0; i < 1000; ++i) {
                yyjson_mut_val* val = yyjson_mut_int(doc, i);
                yyjson_mut_arr_append(array, val);
            }
            
            COUTF(yyjson_mut_arr_size(array), 1000);
            COUTF(yyjson_mut_get_int(yyjson_mut_arr_get(array, 0)), 0);
            COUTF(yyjson_mut_get_int(yyjson_mut_arr_get(array, 999)), 999);
            
            yyjson_mut_doc_free(doc);
        }
    );

    COUTF(passed, true);
}

DEF_TAST(mutable_object_100, "可变对象构造对比(100个属性)")
{
    bool passed = relativePerformance(
        "xyjson mutable object construction",
        []() {
            MutableDocument mutDoc;
            auto obj = mutDoc * kObject;
            
            // 使用 << 操作符构造对象
            for (int i = 0; i < 100; ++i) {
                std::string key = "key" + std::to_string(i);
                obj << key.c_str() << i;
            }
            
            COUTF(obj.size(), 100);
            COUTF((obj / "key0" | 0), 0);
            COUTF((obj / "key99" | 0), 99);
        },
        "yyjson mutable object construction",
        []() {
            yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
            yyjson_mut_val* obj = yyjson_mut_obj(doc);
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_obj_add_val(doc, root, "object", obj);
            yyjson_mut_doc_set_root(doc, root);
            
            // 使用 yyjson API 构造对象
            for (int i = 0; i < 100; ++i) {
                std::string key = "key" + std::to_string(i);
                yyjson_mut_val* val = yyjson_mut_int(doc, i);
                yyjson_mut_obj_add(obj, yyjson_mut_strncpy(doc, key.c_str(), key.size()), val);
            }
            
            COUTF(yyjson_mut_obj_size(obj), 100);
            COUTF(yyjson_mut_get_int(yyjson_mut_obj_get(obj, "key0")), 0);
            COUTF(yyjson_mut_get_int(yyjson_mut_obj_get(obj, "key99")), 99);
            
            yyjson_mut_doc_free(doc);
        },
        100
    );

    COUTF(passed, true);
}

DEF_TAST(mutable_object_500, "可变对象构造对比(500个属性)")
{
    bool passed = relativePerformance(
        "xyjson mutable object construction (500)",
        []() {
            MutableDocument mutDoc;
            auto obj = mutDoc * kObject;
            
            // 使用 << 操作符构造对象
            for (int i = 0; i < 500; ++i) {
                std::string key = "key" + std::to_string(i);
                obj << key.c_str() << i;
            }
            
            COUTF(obj.size(), 500);
            COUTF((obj / "key0" | 0), 0);
            COUTF((obj / "key499" | 0), 499);
        },
        "yyjson mutable object construction (500)",
        []() {
            yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
            yyjson_mut_val* obj = yyjson_mut_obj(doc);
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_obj_add_val(doc, root, "object", obj);
            yyjson_mut_doc_set_root(doc, root);
            
            // 使用 yyjson API 构造对象
            for (int i = 0; i < 500; ++i) {
                std::string key = "key" + std::to_string(i);
                yyjson_mut_val* val = yyjson_mut_int(doc, i);
                yyjson_mut_obj_add(obj, yyjson_mut_strncpy(doc, key.c_str(), key.size()), val);
            }
            
            COUTF(yyjson_mut_obj_size(obj), 500);
            COUTF(yyjson_mut_get_int(yyjson_mut_obj_get(obj, "key0")), 0);
            COUTF(yyjson_mut_get_int(yyjson_mut_obj_get(obj, "key499")), 499);
            
            yyjson_mut_doc_free(doc);
        }
    );

    COUTF(passed, true);
}

DEF_TAST(mutable_object_1000, "可变对象构造对比(1000个属性)")
{
    bool passed = relativePerformance(
        "xyjson mutable object construction (1000)",
        []() {
            MutableDocument mutDoc;
            auto obj = mutDoc * kObject;
            
            // 使用 << 操作符构造对象
            for (int i = 0; i < 1000; ++i) {
                std::string key = "key" + std::to_string(i);
                obj << key.c_str() << i;
            }
            
            COUTF(obj.size(), 1000);
            COUTF((obj / "key0" | 0), 0);
            COUTF((obj / "key999" | 0), 999);
        },
        "yyjson mutable object construction (1000)",
        []() {
            yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
            yyjson_mut_val* obj = yyjson_mut_obj(doc);
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_obj_add_val(doc, root, "object", obj);
            yyjson_mut_doc_set_root(doc, root);
            
            // 使用 yyjson API 构造对象
            for (int i = 0; i < 1000; ++i) {
                std::string key = "key" + std::to_string(i);
                yyjson_mut_val* val = yyjson_mut_int(doc, i);
                yyjson_mut_obj_add(obj, yyjson_mut_strncpy(doc, key.c_str(), key.size()), val);
            }
            
            COUTF(yyjson_mut_obj_size(obj), 1000);
            COUTF(yyjson_mut_get_int(yyjson_mut_obj_get(obj, "key0")), 0);
            COUTF(yyjson_mut_get_int(yyjson_mut_obj_get(obj, "key999")), 999);
            
            yyjson_mut_doc_free(doc);
        }
    );

    COUTF(passed, true);
}
