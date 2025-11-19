/**
 * @file t_design.cpp
 * @author lymslive
 * @date 2025-11-19
 * @brief docs/design.md 文档示例的单元测试
 */
#include "couttast/couttast.h"
#include "xyjson.h"
#include <iostream>

// use to mark code snippet from document
#define MARKDOWN_CODE_SNIPPET

// mark extrat statement need in test but not show in document.
#define CODE(statement) statement

DEF_TAST(design_2_yyjson_obj_get, "example from docs/design.md")
{
    std::string strJson = R"({ "user": { "name": "Alice", "age": 25 } })";
    
#ifdef MARKDOWN_CODE_SNIPPET
    // 假设源 json 串存于 strJson 字符串变量
    yyjson_doc* doc = yyjson_read(strJson.c_str(), strJson.size(), 0);
    
    yyjson_val* root = yyjson_doc_get_root(doc);
    yyjson_val* user = yyjson_obj_get(root, "user");
    yyjson_val* name = yyjson_obj_get(user, "name");
#endif
    COUT(doc != nullptr, true);
    COUT(name != nullptr, true);
}

DEF_TAST(design_2_xyjson_path, "example from docs/design.md")
{
    std::string strJson = R"({ "user": { "name": "Alice", "age": 25 } })";
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc(strJson.c_str(), strJson.size());
    
    // 下行 auto 推导为 yyjson::Value ，可以且建议直接写出
    auto name = doc / "user" / "name";
#endif
    COUT(name.isValid(), true);
    COUT(name | "", "Alice");
}

DEF_TAST(design_2_xyjson_index, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Value name = doc["user"]["name"];
#endif
    COUT(name | "", "Alice");
}

DEF_TAST(design_2_index_method, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
    auto user = doc / "user";
#ifdef MARKDOWN_CODE_SNIPPET
    std::string strName = user["name"].toString();
    int age = (user / "age").toInteger();
#endif
    COUT(strName, "Alice");
    COUT(age, 25);
}

DEF_TAST(design_2_path_pointer, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
#ifdef MARKDOWN_CODE_SNIPPET
    auto name = doc / "user" / "name";
    auto age = doc / "/user/age"; // Json Pointer 标准要求以 / 开头
#endif
    COUT(name | "", "Alice");
    COUT(age | 0, 25);
}

DEF_TAST(design_2_path_error, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
#ifdef MARKDOWN_CODE_SNIPPET
    auto user = doc / "user";
    if (user) {
        auto name = user / "name";
        COUT(name | "", "Alice");
    }
#endif
}

DEF_TAST(design_3_path_recall, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
#ifdef MARKDOWN_CODE_SNIPPET
    auto name = doc / "user" / "name";
    auto age = doc / "user" / "age";
#endif
    COUT(name | "", "Alice");
    COUT(age | 0, 25);
}

DEF_TAST(design_3_pipe_getor, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
#ifdef MARKDOWN_CODE_SNIPPET
    auto name = doc / "user" / "name" | "";
    auto age = doc / "user" / "age" | 0;
#endif
    COUT(name, "Alice");
    COUT(age, 25);
}

DEF_TAST(design_3_getor_convert, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
#ifdef MARKDOWN_CODE_SNIPPET
    std::string name = doc / "user" / "name" | "";
    int64_t age = doc / "user" / "age" | 0;
#endif
    COUT(name, "Alice");
    COUT(age, 25);
}

DEF_TAST(design_3_yyjson_getor, "example from docs/design.md")
{
    std::string strJson = R"({ "user": { "name": "Alice", "age": 25 } })";
    yyjson_doc* doc = yyjson_read(strJson.c_str(), strJson.size(), 0);
    yyjson_val* root = yyjson_doc_get_root(doc);
    yyjson_val* user = yyjson_obj_get(root, "user");
    yyjson_val* name = yyjson_obj_get(user, "name");
    yyjson_val* age = yyjson_obj_get(user, "age");
#ifdef MARKDOWN_CODE_SNIPPET
    // 假设已取的 json 结点指针 name 与 age
    // yyjson_val *name, *age;
    
    // const char* pszName = name | "";
    COUT(yyjson_is_str(name), true);
    if (yyjson_is_str(name)) {
        const char* pszName = yyjson_get_str(name);
        COUT(pszName, "Alice");
    }
    else {
        const char* pszName = "";
    }
    
    // int iAge = name | 0;
    COUT(yyjson_is_int(age), true);
    if (yyjson_is_int(age)) {
        int iAge = yyjson_get_int(age);
        COUT(iAge, 25);
    }
    else {
        int iAge = 0;
    }
#endif
}

DEF_TAST(design_3_getor_assign, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
    auto age = doc / "user" / "age";
#ifdef MARKDOWN_CODE_SNIPPET
    // 假设已取得 yyjson::Value age 结点
    int iAge = 18;
    iAge = age | iAge;
    COUT(iAge, 25);
    
    // 假装 | 支持交换律
//+ iAge = iAge | age;
    
    // 再简化写成
    iAge |= age;
    COUT(iAge, 25);
#endif
}

DEF_TAST(design_3_get_judge, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
    auto age = doc / "user" / "age";
#ifdef MARKDOWN_CODE_SNIPPET
    int iAge = 18;
    COUT(age >> iAge, true);
    if (age >> iAge) { // 不嫌麻烦还可以在 >> 前面加 age && 且条件连接
        // 使用 json 结点中的 age 值
    }
    COUT(iAge, 25);
#endif
}

DEF_TAST(design_3_type_check, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
    auto name = doc / "user" / "name";
    auto age = doc / "user" / "age";
#ifdef MARKDOWN_CODE_SNIPPET
    COUT(name & "", true);
    if (name & "") {
        // name 是字符串类型，与 "" 相同
//+     std::cout << (name | "");
        COUT(name | "", "Alice");
    }
    COUT(age & 0, true);
    if (age & 0) {
        // age 是整数类型，与 0 相同
//+     std::cout << (age | 0);
        COUT(age | 0, 25);
    }
#endif
}

DEF_TAST(design_3_type_dispatch, "example from docs/design.md")
{
    yyjson::Document doc(R"({"user": {"name": "Alice", "age": 25}})");
    auto name = doc / "user" / "name";
    auto age = doc / "user" / "age";
#ifdef MARKDOWN_CODE_SNIPPET
    if (age & 0) { // age.isInt()
        int iAge = age | 0;
    }
    else if (age & "") { // age.isString()
        int iAge = ::atoi(age | "");
    }
#endif
}

DEF_TAST(design_4_mutable_set, "example from docs/design.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument doc(R"({"user": {"name": "Alice", "age":25}})");
    
    doc / "user" / "name" = "Bob";
    doc / "user" / "age" = 20;
    doc["user"]["active"] = true; // 用 [] 添加新结点
#endif
    COUT(doc / "user" / "name" | "", "Bob");
    COUT(doc / "user" / "age" | 0, 20);
    COUT(doc / "user" / "active" | false, true);
}

DEF_TAST(design_4_special_literal, "example from docs/design.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument doc;
    doc["array"] = "[]";  // setArray
    doc["object"] = "{}"; // setObject()
#endif
    COUT(doc / "array" & "[]", true);
    COUT(doc / "object" & "{}", true);
}

DEF_TAST(design_5_container_insert, "example from docs/design.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument doc;
    doc["array"] = "[]";
    doc["object"] = "{}";
    
    doc / "array" << 1 << 2 << 3;
    doc / "object" << "first" << 1 << "second" << 2 << "third" << 3;
    //^ 结果：{"array":[1,2,3],"object":{"first":1,"second":2,"third":3}}
#endif
    COUT(-doc, R"({"array":[1,2,3],"object":{"first":1,"second":2,"third":3}})");
}

DEF_TAST(design_5_container_remove, "example from docs/design.md")
{
    yyjson::MutableDocument doc(R"({"array":[1,2,3],"object":{"first":1,"second":2,"third":3}})");
#ifdef MARKDOWN_CODE_SNIPPET
    // 接上例 doc = {"array":[1,2,3],"object":{"first":1,"second":2,"third":3}}
    
    yyjson::MutableValue a1, a2, a3;
    doc / "array" >> a3 >> a2 >> a1;
    
    yyjson::KeyValue o1, o2, o3;
    doc / "object" >> o3 >> o2 >> o1;
#endif
    COUT(-doc, R"({"array":[],"object":{}})");
}

DEF_TAST(design_5_reinsert_back, "example from docs/design.md")
{
    yyjson::MutableDocument doc(R"({"array":[1,2,3],"object":{"first":1,"second":2,"third":3}})");
    yyjson::MutableValue a1, a2, a3;
    doc / "array" >> a3 >> a2 >> a1;
    yyjson::KeyValue o1, o2, o3;
    doc / "object" >> o3 >> o2 >> o1;
#ifdef MARKDOWN_CODE_SNIPPET
    // 接上例 doc = {"array":[],"object":{}}
    doc / "array" << a3 << a2 << a1;
//+ doc / "object" << o3 << o2 << o1;
    CODE(doc / "object" << std::move(o3) << std::move(o2) << std::move(o1));
    //^ 结果：{"array":[3,2,1],"object":{"third":3,"second":2,"first":1}}
    COUT(-doc, R"({"array":[3,2,1],"object":{"third":3,"second":2,"first":1}})");
#endif
}

DEF_TAST(design_5_document_inout, "example from docs/design.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"array":[1,2,3],"object":{"first":1,"second":2,"third":3}})";
    
    std::string output;
    doc >> output;
#endif
    COUT(output, R"({"array":[1,2,3],"object":{"first":1,"second":2,"third":3}})");
}

DEF_TAST(design_6_iterator_ops, "example from docs/design.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"array":[1,2,3],"object":{"first":1,"second":2,"third":3}})";
    
    // (auto it = doc["array"].iterator(0); it.isValid(); it.next())
    for(auto it = doc / "array" % 0; it; ++it) {
        // *it 代表每个子结点：1, 2, 3
        *it == it.value(); // yyjson::Value
        +it == it.index(); // size_t
        COUT(*it == it.value(), true);
        COUT(+it == it.index(), true);
    }
    
    // (auto it = doc["object"].iterator(""); it.isValid(); it.next())
    for(auto it = doc / "object" % ""; it; ++it) {
        // *it 也代表每个值结点：1, 2, 3
        *it == it.value(); // yyjson::Value
        +it == it.index(); // size_t
        -it == it.name();  // const char*
        ~it == it.key();   // yyjson::Value
        COUT(*it == it.value(), true);
        COUT(+it == it.index(), true);
        COUT(-it == it.name(), true);
        COUT(~it == it.key(), true);
    }
#endif
}

