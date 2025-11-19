/**
 * @file t_api.cpp
 * @author lymslive
 * @date 2025-11-17
 * @brief docs/api.md 文档示例的单元测试
 * */
#include "couttast/couttast.h"
#include "xyjson.h"

// use to mark code snippet from document
#define MARKDOWN_CODE_SNIPPET

// begin code in markdown file with ```cpp
#ifdef MARKDOWN_CODE_SNIPPET
    using namespace yyjson;
#endif
// end code in markdown file with ```

#define CODE(statement) statement

DEF_TAST(api_2_2_literal_operator, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    using namespace yyjson;
    auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
    COUT(doc.isValid(), true);
    COUT(doc / "name" | std::string(), "Alice");
    COUT(doc / "age" | 0, 30);
    
    // 等效：显式调用 yyjson::operator""_xyjson
    {
        const char* str = R"({"name": "Alice", "age": 30})";
        size_t len = ::strlen(str);
        auto doc = yyjson::operator""_xyjson(str, len);
        COUT(doc.isValid(), true);
    }
    
    // 等效：使用构造函数
    {
        yyjson::Document doc(R"({"name": "Alice", "age": 30})");
        COUT(doc.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_3_member_access, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc(R"({"name": "Alice", "age": 30})");
    
    auto json = doc / "age";
    auto iter = doc % "age";
    if (json->isInt()) {
        int age = json->toInteger(); // 结果：30
        COUT(age, 30);
    }
    if (iter->isInt()) {
        int age = iter->toInteger(); // 结果：30
        COUT(age, 30);
    }
    
    // 等效：使用 . 成员访问操作符
    // jsonT 类直接使用 . 访问，iteratorT 类要先解引用
    {
        auto json = doc / "age";
        auto iter = doc % "age";
        if (json.isInt()) {
            int age = json.toInteger(); // 结果：30
            COUT(age, 30);
        }
        if ((*iter).isInt()) {
            int age = (*iter).toInteger(); // 结果：30
            COUT(age, 30);
        }
    }
#endif
}

DEF_TAST(api_2_4_1_object_field_access, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice"}})"_xyjson;
    auto user = doc["user"];
    auto name = user["name"];
    COUT(name.isValid(), true);
    COUT(name | std::string(), "Alice");
    
    /* use method: index() */
    {
        auto name = user.index("name");
        COUT(name.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_4_2_array_index_access, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    auto items = doc["items"];
    auto first = items[0];
    COUT(first.isValid(), true);
    COUT(first | 0, 10);
    
    /* use method: index() */
    {
        auto first = items.index(0);
        COUT(first.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_4_3_mutable_object_add_field, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc["new_field"] = "value";
    mutDoc["array"] = "[]";
    COUT(mutDoc["new_field"] | std::string(), "value");
    COUT(mutDoc["array"].isArray(), true);
    
    /* use method: index() */
    {
        yyjson::MutableDocument mutDoc;
        mutDoc.root().index("new_field").set("value");
        mutDoc.root().index("array").set("[]");
        COUT(mutDoc["new_field"] | std::string(), "value");
    }
#endif
}

DEF_TAST(api_2_5_1_iterator_copy_advance, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    auto iter = doc / "items" % 0;
    auto oldIter = iter++;  // 返回原迭代器，然后iter前进
    COUT(*oldIter | 0, 10);
    COUT(*iter | 0, 20);
    
    /* use method: next() */
    {
        auto iter = doc / "items" % 0;
        auto oldIter = iter;
        iter.next();
        COUT(*oldIter | 0, 10);
        COUT(*iter | 0, 20);
    }
#endif
}

DEF_TAST(api_2_6_1_iterator_copy_retreat, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    auto iter = doc / "items" % 1;
    auto oldIter = iter--;  // 返回原迭代器，然后iter后退
    COUT(*oldIter | 0, 20);
    COUT(*iter | 0, 10);
    
    /* use method: prev() */
    {
        auto iter = doc / "items" % 1;
        auto oldIter = iter;
        iter.prev();
        COUT(*oldIter | 0, 20);
        COUT(*iter | 0, 10);
    }
#endif
}

DEF_TAST(api_2_7_1_iterator_advance, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    
    auto iter = doc / "items" % 0;
    ++iter;  // 原迭代器前进并返回
    COUT(*iter | 0, 20);
    
    /* use method: next() */
    {
        auto iter = doc / "items" % 0;
        iter.next();
        COUT(*iter | 0, 20);
    }
#endif
}

DEF_TAST(api_2_8_1_iterator_retreat, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    
    auto iter = doc / "items" % 1;
    COUT(*iter | 0, 20);
    --iter;  // 原迭代器后退并返回
    COUT(*iter | 0, 10);
    
    /* use method: prev() */
    {
        auto iter = doc / "items" % 1;
        COUT(*iter | 0, 20);
        iter.prev();
        COUT(*iter | 0, 10);
    }
#endif
}

DEF_TAST(api_2_9_1_invalid_value_check, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({})"_xyjson;
    
    COUT(bool(doc), true);
    COUT(!(doc / "nonexistent"), true);
    COUT(!(doc % "nonexistent"), true);
    
    if (!doc) {
        // 文档无效，可能是解析错误
    }
    if (!(doc / "nonexistent")) {
        // 路径不存在
    }
    if (!(doc % "nonexistent")) {
        // 迭代器无效
    }
    
    /* use method: isValid() */
    COUT(doc.hasError(), false);
    COUT(doc["nonexistent"].isValid(), false);
    COUT(doc.root().iterator("nonexistent").isValid(), false);
    if (doc.hasError()) { // 或 !doc.isValid()
        // 文档有错误
    }
    if (!doc["nonexistent"].isValid()) {
        // 结点无效
    }
    if (doc.root().iterator("nonexistent").isValid() == false) {
        // 迭代器无效
    }
#endif
}

DEF_TAST(api_2_10_1_bool_conversion, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    
    /* use operator */
    COUT(bool(doc), true);
    if (doc) {
        // 文档有效，解析无错误
    }
    bool isValid = bool(doc / "name");
    COUT(isValid, true);
    
    /* use method */
    COUT(doc.hasError(), false);
    if (doc.hasError()) {
        // 文档解析出错
    }
    bool isValid2 = doc["name"].isValid();
    COUT(isValid2, true);
#endif
}

DEF_TAST(api_2_10_1_bool_conversion_extended, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":false, "number":0, "string":"", "array":[], "object":{}})"_xyjson;
    
    bool hasNode = false;
    // 以下 hasNode 都返回 true
    hasNode = (bool)doc["null"];
    COUT(hasNode, true);
    hasNode = (bool)doc["bool"];
    COUT(hasNode, true);
    hasNode = (bool)doc["number"];
    COUT(hasNode, true);
    hasNode = (bool)doc["string"];
    COUT(hasNode, true);
    hasNode = (bool)doc["array"];
    COUT(hasNode, true);
    hasNode = (bool)doc["object"];
    COUT(hasNode, true);
    
    bool hasValue = true;
    // 以下 hasValue 都返回 false
    hasValue = (int)doc["null"];
    COUT(hasValue, false);
    hasValue = (int)doc["bool"];
    COUT(hasValue, false);
    hasValue = (int)doc["number"];
    COUT(hasValue, false);
    hasValue = (int)doc["string"];
    COUT(hasValue, false);
    hasValue = (int)doc["array"];
    COUT(hasValue, false);
    hasValue = (int)doc["object"];
    COUT(hasValue, false);
#endif
}

DEF_TAST(api_2_10_2_int_conversion, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    int iError  = (int)doc["error"];  // 0
    COUT(iError, 0);
    int iNull   = (int)doc["null"];   // 0
    COUT(iNull, 0);
    int iBool   = (int)doc["bool"];   // 1
    COUT(iBool, 1);
    int iNumber = (int)doc["number"]; // 3
    COUT(iNumber, 3);
    int iString = (int)doc["string"]; // 3
    COUT(iString, 3);
    int iArray  = (int)doc["array"];  // 2
    COUT(iArray, 2);
    int iObject = (int)doc["object"]; // 3
    COUT(iObject, 3);
    
    /* use method: toInteger() */
    {
        int iError  = doc["error"].toInteger();  // 0
        COUT(iError, 0);
        int iNull   = doc["null"].toInteger();   // 0
        COUT(iNull, 0);
        int iBool   = doc["bool"].toInteger();   // 1
        COUT(iBool, 1);
        int iNumber = doc["number"].toInteger(); // 3
        COUT(iNumber, 3);
        int iString = doc["string"].toInteger(); // 3
        COUT(iString, 3);
        int iArray  = doc["array"].toInteger();  // 2
        COUT(iArray, 2);
        int iObject = doc["object"].toInteger(); // 3
        COUT(iObject, 3);
    }
#endif
}

DEF_TAST(api_2_10_3_double_conversion, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14, "int":3,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    double fError  = (double)doc["error"];  // 0.0
    COUT(fError, 0.0);
    double fNull   = (double)doc["null"];   // 0.0
    COUT(fNull, 0.0);
    double fBool   = (double)doc["bool"];   // 0.0
    COUT(fBool, 0.0);
    double fNumber = (double)doc["number"]; // 3.14
    COUT(fNumber, 3.14);
    double fInt    = (double)doc["int"];    // 3.0
    COUT(fInt, 3.0);
    double fString = (double)doc["string"]; // 0.0
    COUT(fString, 0.0);
    double fArray  = (double)doc["array"];  // 0.0
    COUT(fArray, 0.0);
    double fObject = (double)doc["object"]; // 0.0
    COUT(fObject, 0.0);
    
    /* use method: toNumber() */
    {
        double fError  = doc["error"].toNumber();  // 0.0
        COUT(fError, 0.0);
        double fNull   = doc["null"].toNumber();   // 0.0
        COUT(fNull, 0.0);
        double fBool   = doc["bool"].toNumber();   // 0.0
        COUT(fBool, 0.0);
        double fNumber = doc["number"].toNumber(); // 3.14
        COUT(fNumber, 3.14);
        double fInt    = doc["int"].toNumber();    // 3.0
        COUT(fInt, 3.0);
        double fString = doc["string"].toNumber(); // 0.0
        COUT(fString, 0.0);
        double fArray  = doc["array"].toNumber();  // 0.0
        COUT(fArray, 0.0);
        double fObject = doc["object"].toNumber(); // 0.0
        COUT(fObject, 0.0);
    }
#endif
}

DEF_TAST(api_2_10_4_string_conversion, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    std::string strError  = (std::string)doc["error"];  // ""
    COUT(strError, "");
    std::string strNull   = (std::string)doc["null"];   // null
    COUT(strNull, "null");
    std::string strBool   = (std::string)doc["bool"];   // true
    COUT(strBool, "true");
    std::string strNumber = (std::string)doc["number"]; // 3.14
    COUT(strNumber, "3.14");
    std::string strString = (std::string)doc["string"]; // 3.14pi
    COUT(strString, "3.14pi");
    std::string strArray  = (std::string)doc["array"];  // [null,null]
    COUT(strArray, "[null,null]");
    std::string strObject = (std::string)doc["object"]; // {"x":0,"y":0,"z":0}
    COUT(strObject, "{\"x\":0,\"y\":0,\"z\":0}");
    std::string strRoot   = (std::string)doc.root();
    COUT(strRoot.find("{") == 0, true);
    
    /* use method: toString() */
    {
        std::string strError  = doc["error"].toString();  // ""
        COUT(strError, "");
        std::string strNull   = doc["null"].toString();   // null
        COUT(strNull, "null");
        std::string strBool   = doc["bool"].toString();   // true
        COUT(strBool, "true");
        std::string strNumber = doc["number"].toString(); // 3.14
        COUT(strNumber, "3.14");
        std::string strString = doc["string"].toString(); // 3.14pi
        COUT(strString, "3.14pi");
        std::string strArray  = doc["array"].toString();  // [null,null]
        COUT(strArray, "[null,null]");
        std::string strObject = doc["object"].toString(); // {"x":0,"y":0,"z":0}
        COUT(strObject, "{\"x\":0,\"y\":0,\"z\":0}");
        std::string strRoot   = doc.root().toString();
        COUT(strRoot.find("{") == 0, true);
    }
#endif
}

DEF_TAST(api_2_10_4_string_conversion_pretty, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    std::string strString = doc["string"].toString(true); // "3.14pi"
    COUT(strString, "\"3.14pi\"");
    std::string strArray  = doc["array"].toString(true);
    COUT(strArray.find("[") == 0, true);
    std::string strObject = doc["object"].toString(true);
    COUT(strObject.find("{") == 0, true);
#endif
}

DEF_TAST(api_2_10_5_doc_to_mutabledoc, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc("{}");
    yyjson::MutableDocument mutDoc(doc);
    COUT(mutDoc.isValid(), true);
    
    /* use method: mutate() */
    {
        yyjson::MutableDocument mutDoc = doc.mutate();
        COUT(mutDoc.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_10_6_mutabledoc_to_doc, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc("{}");
    yyjson::Document doc(mutDoc);
    COUT(doc.isValid(), true);
    
    /* use method: freeze() */
    {
        yyjson::Document doc = mutDoc.freeze();
        COUT(doc.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_11_1_doc_to_mutabledoc_bitwise, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc("{}");
    yyjson::MutableDocument mutDoc = ~doc;
    COUT(mutDoc.isValid(), true);
    
    /* use method: mutate() */
    {
        yyjson::MutableDocument mutDoc = doc.mutate();
        COUT(mutDoc.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_11_2_mutabledoc_to_doc_bitwise, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc("{}");
    yyjson::Document doc = ~mutDoc;
    COUT(doc.isValid(), true);
    
    /* use method: freeze() */
    {
        yyjson::Document doc = mutDoc.freeze();
        COUT(doc.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_11_3_iterator_get_key, "example from docs/api.md")
{
    CODE(int count = 0);
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        auto keyNode = ~iter;  // 获取键结点
        auto valueNode = *iter;  // 获取值结点
        auto keyName = -iter;  // 获取键名
        COUT(keyNode.isValid(), true);
        COUT(valueNode.isValid(), true);
        COUT(keyName != nullptr, true);
        CODE(count++);
    }
    COUT(count, 2);
    
    /* use method: key() */
    CODE(count = 0);
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        auto keyNode = iter.key();  // 获取键结点
        auto valueNode = iter.value();  // 获取值结点
        auto keyName = iter.name();  // 获取键名
        COUT(keyNode.isValid(), true);
        COUT(valueNode.isValid(), true);
        COUT(keyName != nullptr, true);
        CODE(count++);
    }
    COUT(count, 2);
#endif
}

DEF_TAST(api_2_12_1_iterator_get_value, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    int sum = 0;
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        auto value = *iter;  // 获取当前元素值
        sum += value | 0;
    }
    COUT(sum, 60);
    
    /* use method: value() */
    sum = 0;
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        auto value = iter.value();  // 获取当前元素值
        sum += value | 0;
    }
    COUT(sum, 60);
#endif
}

DEF_TAST(api_2_12_2_doc_get_root, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    auto root = *doc;
    COUT(root.isValid(), true);
    COUT(root["name"] | std::string(), "Alice");
    
    yyjson::MutableDocument mutDoc(doc);
    auto mutRoot = *mutDoc;
    COUT(mutRoot.isValid(), true);
    
    /* use method: root() */
    {
        auto root = doc.root();
        COUT(root.isValid(), true);
        auto mutRoot = mutDoc.root();
        COUT(mutRoot.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_13_1_json_to_int, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    int iError  = +doc["error"];  // 0
    COUT(iError, 0);
    int iNull   = +doc["null"];   // 0
    COUT(iNull, 0);
    int iBool   = +doc["bool"];   // 1
    COUT(iBool, 1);
    int iNumber = +doc["number"]; // 3
    COUT(iNumber, 3);
    int iString = +doc["string"]; // 3
    COUT(iString, 3);
    int iArray  = +doc["array"];  // 2
    COUT(iArray, 2);
    int iObject = +doc["object"]; // 3
    COUT(iObject, 3);
    
    /* use method: toInteger() */
    {
        int iError  = doc["error"].toInteger();  // 0
        COUT(iError, 0);
        int iNull   = doc["null"].toInteger();   // 0
        COUT(iNull, 0);
        int iBool   = doc["bool"].toInteger();   // 1
        COUT(iBool, 1);
        int iNumber = doc["number"].toInteger(); // 3
        COUT(iNumber, 3);
        int iString = doc["string"].toInteger(); // 3
        COUT(iString, 3);
        int iArray  = doc["array"].toInteger();  // 2
        COUT(iArray, 2);
        int iObject = doc["object"].toInteger(); // 3
        COUT(iObject, 3);
    }
#endif
}

DEF_TAST(api_2_13_2_iterator_get_index, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    size_t lastIndex = 0;
    int sum = 0;
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        size_t idx = +iter;
        auto val = *iter;
        lastIndex = idx;
        sum += val | 0;
    }
    COUT(lastIndex, 2);
    COUT(sum, 60);
    
    /* use method: index() */
    lastIndex = 0;
    sum = 0;
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        size_t idx = iter.index();
        auto val = iter.value();
        lastIndex = idx;
        sum += val | 0;
    }
    COUT(lastIndex, 2);
    COUT(sum, 60);
#endif
}

DEF_TAST(api_2_14_1_json_to_string, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    std::string strError  = -doc["error"];  // ""
    COUT(strError, "");
    std::string strNull   = -doc["null"];   // null
    COUT(strNull, "null");
    std::string strBool   = -doc["bool"];   // true
    COUT(strBool, "true");
    std::string strNumber = -doc["number"]; // 3.14
    COUT(strNumber, "3.14");
    std::string strString = -doc["string"]; // 3.14pi
    COUT(strString, "3.14pi");
    std::string strArray  = -doc["array"];  // [null,null]
    COUT(strArray, "[null,null]");
    std::string strObject = -doc["object"]; // {"x":0,"y":0,"z":0}
    COUT(strObject, "{\"x\":0,\"y\":0,\"z\":0}");
    std::string strRoot   = -doc.root();
    COUT(strRoot.find("{") == 0, true);
    
    /* use method: toString() */
    {
        std::string strError  = doc["error"].toString();  // ""
        COUT(strError, "");
        std::string strNull   = doc["null"].toString();   // null
        COUT(strNull, "null");
        std::string strBool   = doc["bool"].toString();   // true
        COUT(strBool, "true");
        std::string strNumber = doc["number"].toString(); // 3.14
        COUT(strNumber, "3.14");
        std::string strString = doc["string"].toString(); // 3.14pi
        COUT(strString, "3.14pi");
        std::string strArray  = doc["array"].toString();  // [null,null]
        COUT(strArray, "[null,null]");
        std::string strObject = doc["object"].toString(); // {"x":0,"y":0,"z":0}
        COUT(strObject, "{\"x\":0,\"y\":0,\"z\":0}");
        std::string strRoot   = doc.root().toString();
        COUT(strRoot.find("{") == 0, true);
    }
#endif
}

DEF_TAST(api_2_15_1_iterator_get_name, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    int count = 0;
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        const char* keyName = -iter;
        auto valNode = *iter;
        COUT(keyName != nullptr, true);
        COUT(valNode.isValid(), true);
        count++;
    }
    COUT(count, 2);
    
    /* use method: name() */
    count = 0;
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        const char* keyName = iter.name();
        auto valNode = iter.value();
        COUT(keyName != nullptr, true);
        COUT(valNode.isValid(), true);
        count++;
    }
    COUT(count, 2);
#endif
}

DEF_TAST(api_2_16_1_path_access, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
        "items": [10, 20, 30]})"_xyjson;
    auto name = *doc / "user" / "name";
    auto item = *doc / "items" / 0;
    COUT(name | std::string(), "Alice");
    COUT(item | 0, 10);
    
    /* use method: pathto() */
    {
        auto name = doc.root().pathto("user").pathto("name");
        COUT(name | std::string(), "Alice");
        auto item = doc.root().pathto("items").index(0);
        COUT(item | 0, 10);
    }
#endif
}

DEF_TAST(api_2_16_1_path_access_with_method, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    int age = (*doc / "user" / "age").toInteger();
    COUT(age, 30);
    
    /* use operator[] */
    {
        int age = doc.root()["user"]["age"].toInteger();
        COUT(age, 30);
    }
    
    /* use method: pathto() */
    {
        int age = doc.root().pathto("user").pathto("age").toInteger();
        COUT(age, 30);
    }
#endif
}

DEF_TAST(api_2_16_2_json_pointer_access, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    auto node = *doc / "/user/name";
    COUT(node | std::string(), "Alice");
    
    /* use method: pathto() */
    {
        auto node = doc.root().pathto("/user/name");
        COUT(node | std::string(), "Alice");
    }
#endif
}

DEF_TAST(api_2_16_3_iterator_seek, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"config": {"theme": "dark", "lang": "zh"}})"_xyjson;
    auto iter = doc / "config" % "";
    auto theme = iter / "theme";  // 第一次查找
    auto lang = iter / "lang";    // 继续在相同对象中查找
    COUT(theme | std::string(), "dark");
    COUT(lang | std::string(), "zh");
    
    /* use method: seek() */
    {
        auto iter = doc / "config" % "";
        auto theme = iter.seek("theme");
        COUT(theme | std::string(), "dark");
        auto lang = iter.seek("lang");
        COUT(lang | std::string(), "zh");
    }
#endif
}

DEF_TAST(api_2_17_1_doc_create_node, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    auto intNode = mutDoc * 314;
    auto strNode = mutDoc * "value";
    auto newArray = mutDoc * "[]";
    auto newObject = mutDoc * "{}";
    COUT(intNode | 0, 314);
    COUT(strNode | std::string(), "value");
    COUT(newArray.isArray(), true);
    COUT(newObject.isObject(), true);
    
    /* use method: create() */
    {
        auto intNode = mutDoc.create(314);
        COUT(intNode | 0, 314);
        auto strNode = mutDoc.create("new_value");
        COUT(strNode | std::string(), "new_value");
        auto newArray = mutDoc.create("[]");
        COUT(newArray.isArray(), true);
        auto newObject = mutDoc.create("{}");
        COUT(newObject.isObject(), true);
    }
#endif
}

DEF_TAST(api_2_17_2_doc_copy_node, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument dstDoc;
    yyjson::MutableDocument srcDocMut(R"({"name": "Alice", "age": 30})");
    yyjson::Document srcDoc(R"({"name": "Alice", "age": 30})");
    
    auto copyName = dstDoc * (srcDoc / "name");
    auto copyAge = dstDoc * (srcDocMut / "age");
    auto copyDoc = dstDoc * srcDoc;
    auto copyDocMut = dstDoc * srcDocMut;
    COUT(copyName | std::string(), "Alice");
    COUT(copyAge | 0, 30);
    COUT(copyDoc.isValid(), true);
    COUT(copyDocMut.isValid(), true);
    
    /* use method: create() */
    {
        auto copyName = dstDoc.create(srcDoc / "name");
        COUT(copyName | std::string(), "Alice");
        auto copyAge = dstDoc.create(srcDocMut / "age");
        COUT(copyAge | 0, 30);
        auto copyDoc = dstDoc.create(srcDoc);
        COUT(copyDoc.isValid(), true);
        auto copyDocMut = dstDoc.create(srcDocMut);
        COUT(copyDocMut.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_17_3_doc_move_node, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"(["Alice", 30])");
    
    auto node = mutDoc * 2.5;     // node 有效
    COUT(node | 0.0, 2.5);
    auto level = mutDoc * std::move(node); // node 失效
    COUT(node.isValid(), false);
    COUT(level | 0.0, 2.5);
    mutDoc.root() << std::move(level);   // level 失效
    COUT(level.isValid(), false);
    //^ 结果：["Alice",30,2.5]
    COUT(mutDoc.root().size(), 3);
    COUT(mutDoc.root().toString(), "[\"Alice\",30,2.5]");
    
    // 直接用链式表达式，临时值是纯右值，属移动语义
    *mutDoc << mutDoc * true; // 结果：["Alice",30,2.5,true]
    COUT(mutDoc.root().size(), 4);
    COUT(mutDoc.root().toString(), "[\"Alice\",30,2.5,true]");
    
    // 添加数组可以更简洁点，会自动调用 create
    *mutDoc << 100 << 200; // 结果：["Alice",30,2.5,true,100,200]
    COUT(mutDoc.root().size(), 6);
    COUT(mutDoc.root().toString(), "[\"Alice\",30,2.5,true,100,200]");
#endif
}

DEF_TAST(api_2_17_4_bind_key_value, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc("{}");
    auto intNode = mutDoc * 314;
    auto keyNode = mutDoc * "key";
    auto kv = std::move(keyNode) * std::move(intNode);
    mutDoc.root() << std::move(kv); // 结果：{"key":314}
    COUT(mutDoc["key"] | 0, 314);
    
    // 使用连乘临时值，一元 `*` 表示 root()
    {
        yyjson::MutableDocument mutDoc("{}");
        *mutDoc << (mutDoc * "key") * (mutDoc * 314);
        COUT(mutDoc["key"] | 0, 314);
    }
#endif
}

DEF_TAST(api_2_17_5_bind_key_name, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    auto keyValue = mutDoc * "value" * "key";
    auto kvConfig = "config" * (mutDoc * "{}");
    
    /* use method: tag() */
    {
        auto keyValue = mutDoc.create("value").tag("key");
        COUT(keyValue.isValid(), true);
        auto kvConfig = mutDoc.create("{}").tag("config");
        COUT(kvConfig.isValid(), true);
    }
    
    // 将绑定的结果插入根对象，生成: {"key":"value","config":{}}
    mutDoc.root() << std::move(keyValue) << std::move(kvConfig);
    COUT(mutDoc["key"] | std::string(), "value");
    COUT(mutDoc["config"].isObject(), true);
    
    // 连乘直接插入，一元 `*` 表示 root()
    {
        yyjson::MutableDocument mutDoc;
        *mutDoc << mutDoc * "value" * "key" << "config" * (mutDoc * "{}");
        COUT(mutDoc["key"] | std::string(), "value");
        COUT(mutDoc["config"].isObject(), true);
    }
#endif
}

DEF_TAST(api_2_18_1_array_iterator_create, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    int sum = 0;
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        sum += *iter | 0;
    }
    COUT(sum, 60);
    
    /* use method: iterator() */
    sum = 0;
    for (auto iter = doc["items"].iterator(0); iter; ++iter) {
        sum += *iter | 0;
    }
    COUT(sum, 60);
#endif
}

DEF_TAST(api_2_18_2_object_iterator_create, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    int count = 0;
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        count++;
    }
    COUT(count, 2);
    
    /* use method: iterator() */
    count = 0;
    for (auto iter = doc["user"].iterator(""); iter; ++iter) {
        count++;
    }
    COUT(count, 2);
#endif
}

DEF_TAST(api_2_18_3_iterator_relocate, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
        "items": [10, 20, 30]})"_xyjson;
    auto itArray = doc / "items" % 1; // 创建迭代器初始位置 1
    auto itArrayCopy = itArray % 3;  // 重定位到索引3的元素
    COUT(*itArray | 0, 20);
    COUT(itArrayCopy.isValid(), false); // 索引3超出范围
    
    auto itObject = *doc % "items"; // 创建迭代器初始位置 "items"
    auto itObjectCopy = itObject % "user"; // 重定位到 "user" 位置
    COUT(itObject.isValid(), true);
    COUT(itObjectCopy.isValid(), true);
    
    /* use method: advance() */
    {
        auto itArray = doc["items"].iterator(1);
        auto itArrayCopy = itArray;
        itArrayCopy.begin().advance(3);
        COUT(itArrayCopy.isValid(), false);
        
        auto itObject = doc.root().iterator("items");
        auto itObjectCopy = itObject;
        itObjectCopy.begin().advance("user");
        COUT(itObjectCopy.isValid(), true);
    }
#endif
}

DEF_TAST(api_2_19_1_iterator_advance_multistep, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30, 40, 50]})"_xyjson;
    auto iter = doc / "items" % 0;
    auto iterCopy = iter + 3;  // 前进3步，指向第4个元素
    COUT(*iter | 0, 10);
    COUT(*iterCopy | 0, 40);
    
    /* use method: advance() */
    {
        auto iterCopy = iter;
        iterCopy.advance(3);
        COUT(*iterCopy | 0, 40);
    }
#endif
}

DEF_TAST(api_2_20_1_stream_output, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
//+ std::cout << doc / "name" << std::endl;
    std::ostringstream oss;
    oss << doc / "name";
    COUT(oss.str(), "Alice");
    
    /* use method: toString() */
    {
//+     std::cout << (doc / "name").toString() << std::endl;
        std::ostringstream oss;
        oss << (doc / "name").toString();
        COUT(oss.str(), "Alice");
    }
#endif
}

DEF_TAST(api_2_20_2_doc_read_string, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    bool success = doc << R"({"name": "Alice"})";
    COUT(success, true);
    COUT(doc["name"] | std::string(), "Alice");
    
    /* use method: read() */
    {
        yyjson::Document doc;
        bool success = doc.read(R"({"name": "Alice"})");
        COUT(success, true);
        COUT(doc["name"] | std::string(), "Alice");
    }
    
    // 也可直接用构造函数
    {
        yyjson::Document doc(R"({"name": "Alice"})");
        COUT(doc["name"] | std::string(), "Alice");
    }
#endif
}

DEF_TAST(api_2_20_3_doc_read_file, "example from docs/api.md")
{
    // Create a temporary test file before the markdown code snippet
    const char* temp_file = "/tmp/data.json";
    
    // Write test data to temporary file
    FILE* fp = fopen(temp_file, "w");
    if (fp)
    {
        fprintf(fp, "{\"name\": \"Alice\", \"age\": 30, \"active\": true}");
        fclose(fp);
    }
    
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    std::ifstream file("/tmp/data.json");
    bool success = doc << file;
    COUT(success, true);
    COUT(doc.isValid(), true);
    COUT(doc / "name" | std::string(), "Alice");
    COUT(doc / "age" | 0, 30);
    COUT(doc / "active" | false, true);
    
    /* use method: read() */
    {
        yyjson::Document doc;
        std::ifstream file("/tmp/data.json");
        bool success = doc.read(file);
        COUT(success, true);
        COUT(doc.isValid(), true);
        COUT(doc / "name" | std::string(), "Alice");
        COUT(doc / "age" | 0, 30);
    }
    
    // 另有 readFile() 方法接收文件名参数
    {
        yyjson::Document doc;
        bool success = doc.readFile("/tmp/data.json");
        COUT(success, true);
        COUT(doc.isValid(), true);
        COUT(doc / "name" | std::string(), "Alice");
        COUT(doc / "age" | 0, 30);
    }
#endif

    // Clean up temporary file
    remove(temp_file);
}

DEF_TAST(api_2_20_4_array_append, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc["items"] = "[]";
    mutDoc / "items" << 1 << "two" << 3.14;
    //^ 结果：{"items":[1,"two",3.14]}
    COUT(mutDoc["items"][0] | 0, 1);
    COUT(mutDoc["items"][1] | std::string(), "two");
    COUT(mutDoc["items"][2] | 0.0, 3.14);
    
    /* use method: push() */
    {
        yyjson::MutableDocument mutDoc;
        mutDoc["items"] = "[]";
        mutDoc["items"].push(1).push("two").push(3.14);
        COUT(mutDoc["items"][0] | 0, 1);
    }
    
    // push 是 json 容器通用方法，数组也可用 append()
    {
        yyjson::MutableDocument mutDoc;
        mutDoc["items"] = "[]";
        mutDoc["items"].append(1).append("two").append(3.14);
        COUT(mutDoc["items"][0] | 0, 1);
    }
#endif
}

DEF_TAST(api_2_20_5_object_insert, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    auto root = *mutDoc;
    root << "key1" << "value1" << "key2" << 42;
    COUT(mutDoc["key1"] | std::string(), "value1");
    COUT(mutDoc["key2"] | 0, 42);
    
    /* use method: push() */
    {
        yyjson::MutableDocument mutDoc;
        auto root = *mutDoc;
        root.push("key1").push("value1").push("key2").push(42);
        COUT(mutDoc["key1"] | std::string(), "value1");
    }
    
    // 也可用 add() 方法接收两个参数
    {
        yyjson::MutableDocument mutDoc;
        auto root = *mutDoc;
        root.add("key1", "value1").add("key2", 42);
        COUT(mutDoc["key1"] | std::string(), "value1");
    }
#endif
}

DEF_TAST(api_2_20_5_object_insert_keyvalue, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    *mutDoc << mutDoc * "Alice" * "name" << "age" * (mutDoc * 25);
    //^ 结果：{"name":"Alice","age":25}
    COUT(mutDoc["name"] | std::string(), "Alice");
    COUT(mutDoc["age"] | 0, 25);
    COUT(mutDoc.root().toString(), R"({"name":"Alice","age":25})");
    
    // 删除结点
    yyjson::KeyValue age, name;
    *mutDoc >> age >> name;
    COUT((mutDoc / "name").isValid(), false);
    COUT((mutDoc / "age").isValid(), false);
    
    // 换个顺序重新插入
    *mutDoc << std::move(age) << std::move(name);
    //^ 结果：{"age":25,"name":"Alice"}
    COUT(mutDoc["name"] | std::string(), "Alice");
    COUT(mutDoc["age"] | 0, 25);
    COUT(mutDoc.root().toString(), R"({"age":25,"name":"Alice"})");
#endif
}

DEF_TAST(api_2_20_6_array_insert, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc["items"] = "[]";
    auto iter = mutDoc / "items" % 0;
    iter << 10 << 20 << 30;
    COUT(mutDoc["items"].size(), 3);
    COUT(mutDoc["items"][0] | 0, 10);
    COUT(mutDoc["items"][1] | 0, 20);
    COUT(mutDoc["items"][2] | 0, 30);
    
    /* use method: insert() */
    {
        yyjson::MutableDocument mutDoc;
        mutDoc["items"] = "[]";
        auto iter = mutDoc / "items" % 0;
        iter.insert(10); ++iter;
        iter.insert(20); ++iter;
        iter.insert(30); ++iter;
        COUT(mutDoc["items"].size(), 3);
        COUT(mutDoc["items"][0] | 0, 10);
        COUT(mutDoc["items"][1] | 0, 20);
        COUT(mutDoc["items"][2] | 0, 30);
    }
#endif
}

DEF_TAST(api_2_20_6_object_insert, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc["user"] = "{}";
    auto iter = mutDoc / "user" % "";
    iter << "name" << "Alice" << "age" * (mutDoc * 25);
    COUT(mutDoc["user"]["name"] | std::string(), "Alice");
    COUT(mutDoc["user"]["age"] | 0, 25);
    
    /* use method: insert() */
    {
        yyjson::MutableDocument mutDoc;
        mutDoc["user"] = "{}";
        auto iter = mutDoc / "user" % "";
        iter.insert("name"); iter.insert("Alice"); ++iter;
        iter.insert("age", 25); ++iter;
        COUT(mutDoc["user"]["name"] | std::string(), "Alice");
        COUT(mutDoc["user"]["age"] | 0, 25);
    }
#endif
}

DEF_TAST(api_2_21_1_doc_write_string, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    std::string str;
    doc >> str;
    COUT(str, R"({"name":"Alice"})");
    
    /* use method: write() */
    {
        std::string str;
        doc.write(str);
        COUT(str, R"({"name":"Alice"})");
    }
#endif
}

DEF_TAST(api_2_21_2_doc_write_file, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    std::ofstream file("/tmp/output.json");
    bool success = doc >> file;
    COUT(success, true);
    
    /* use method: write() */
    {
        std::ofstream file("/tmp/output.json");
        bool success = doc.write(file);
        COUT(success, true);
    }
    
    // 另有 writeFile() 方法接收文件名参数
    {
        bool success = doc.writeFile("/tmp/output.json");
        COUT(success, true);
    }
#endif
    remove("/tmp/output.json");
}

DEF_TAST(api_2_21_3_value_extract, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc(R"({"name": "Alice", "age":25})");
    
    std::string name;
    if (doc / "name" >> name) {
        // 成功提取
    }
    COUT(name, "Alice");
    
    int age;
    if (doc / "age" >> age) {
        // 成功提取
    }
    COUT(age, 25);
    
    /* use method: get() */
    {
        std::string name;
        if (doc["name"].get(name)) {
        }
        COUT(name, "Alice");
    
        int age;
        if (doc["age"].get(age)) {
        }
        COUT(age, 25);
    }
#endif
}

DEF_TAST(api_2_21_4_array_pop, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
    yyjson::MutableValue p1, p2, p3;
    mutDoc / "items" >> p3;
    COUT(p3 | 0, 30);
    mutDoc / "items" >> p2 >> p1;
    COUT(p2 | 0, 20);
    COUT(p1 | 0, 10);
    COUT(mutDoc["items"].size(), 0);
    
    /* use method: pop() */
    {
        yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
        yyjson::MutableValue p1, p2, p3;
        mutDoc["items"].pop(p3);
        COUT(p3 | 0, 30);
        mutDoc["items"].pop(p2).pop(p1);
        COUT(p2 | 0, 20);
        COUT(p1 | 0, 10);
        COUT(mutDoc["items"].size(), 0);
    }
#endif
}

DEF_TAST(api_2_21_5_object_pop, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
    yyjson::KeyValue name, age;
    mutDoc / "user" >> age >> name;
    COUT(mutDoc["user"].size(), 0);
    
    /* use method: pop() */
    {
        yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
        yyjson::KeyValue name, age;
        mutDoc["user"].pop(age).pop(name);
        COUT(mutDoc["user"].size(), 0);
    }
#endif
}

DEF_TAST(api_2_21_6_array_iterator_remove, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
    yyjson::MutableValue p1, p2,p3;
    auto it = mutDoc / "items" % 0;
    it >> p1;
    COUT(p1 | 0, 10);
    it >> p2 >> p3;
    COUT(p2 | 0, 20);
    COUT(p3 | 0, 30);
    COUT(mutDoc["items"].size(), 0);
    
    /* use method: remove() */
    {
        yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
        auto it = mutDoc / "items" % 0;
        auto p1 = it.remove();
        COUT(p1 | 0, 10);
        auto p2 = it.remove();
        COUT(p2 | 0, 20);
        auto p3 = it.remove();
        COUT(p3 | 0, 30);
    }
#endif
}

DEF_TAST(api_2_21_7_object_iterator_remove, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
    yyjson::KeyValue name, age;
    auto it = mutDoc / "user" % "";
    it >> name >> age;
    COUT(mutDoc["user"].size(), 0);
    
    /* use method: remove() */
    {
        yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
        auto it = mutDoc / "user" % "";
        auto name = it.remove();
        auto age = it.remove();
        COUT(mutDoc["user"].size(), 0);
    }
#endif
}

DEF_TAST(api_2_22_1_json_less, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"a": 10, "b": 20})"_xyjson;
    bool result = doc / "a" < doc / "b";
    COUT(result, true);
    
    /* use method: less() */
    {
        auto result = doc["a"].less(doc["b"]);
        COUT(result, true);
    }
#endif
}

DEF_TAST(api_2_23_1_json_equal, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc1 = R"({"name": "Alice"})"_xyjson;
    yyjson::Document doc2 = R"({"name": "Alice"})"_xyjson;
    auto root1 = *doc1;
    auto root2 = *doc2;
    
    bool result = root1 == root2;
    COUT(result, true);
    
    /* use method: equal() */
    {
        bool result = root1.equal(root2);
        COUT(result, true);
    }
#endif
}

DEF_TAST(api_2_23_2_json_scalar_equal, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name":"Alice", "age": 25})"_xyjson;
    
    bool nameMatch = doc / "name" == "Alice"; // true
    COUT(nameMatch, true);
    bool ageMatch = doc / "age" == 25; // true
    COUT(ageMatch, true);
    bool ageFloatMatch = doc / "age" == 25.0; // false
    COUT(ageFloatMatch, false);
#endif
}

DEF_TAST(api_2_23_3_iterator_equal, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    
    // 用迭代器判断键是否存在
    auto it = doc / "user" % "age";
    if (it == doc["user"].endObject()) {} // false
    COUT(it == doc["user"].endObject(), false);
    
    /* use method: equal() */
    if (it.equal(doc["user"].endObject())) {} // false
    COUT(it.equal(doc["user"].endObject()), false);
    
    // 更简洁的判断
    if(doc / "user" / "age") {} // true
    if(doc / "user" % "age") {} // true
    COUT((doc / "user" / "age").isValid(), true);
    COUT((doc / "user" % "age").isValid(), true);
#endif
}

DEF_TAST(api_2_23_4_array_iterator_not_equal, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items":[10, 20, 30]})"_xyjson;
    
    int sum = 0;
    for (auto it = doc["items"].beginArray(); it != doc["items"].endArray(); ++it)
    {
        sum += *it | 0;
    }
    COUT(sum, 60);
    
    /* use method: equal() */
    sum = 0;
    for (auto it = doc["items"].beginArray(); !it.equal(doc["items"].endArray()); ++it)
    {
        sum += *it | 0;
    }
    COUT(sum, 60);
#endif
}

DEF_TAST(api_2_24_1_type_check, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice", "age": 30})"_xyjson;
    bool isString = doc / "name" & "";
    COUT(isString, true);
    bool isInt = doc / "age" & 0;
    COUT(isInt, true);
    bool isReal = doc / "age" & 0.0;
    COUT(isReal, false);
    bool isObject = doc.root() & "{}";
    COUT(isObject, true);
    bool isArray = doc.root() & "[]";
    COUT(isArray, false);
    
    /* use method: isType() */
    {
        bool isString = doc["name"].isType("");
        COUT(isString, true);
        bool isInt = doc["age"].isType(0);
        COUT(isInt, true);
        bool isReal = doc["age"].isType(0.0);
        COUT(isReal, false);
        bool isObject = doc.root().isType("{}");
        COUT(isObject, true);
        bool isArray = doc.root().isType("[]");
        COUT(isArray, false);
    }
#endif
}

DEF_TAST(api_2_25_1_get_with_default, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice", "age": 30, "active": true})"_xyjson;
    const char* pszName = doc / "name" | "";
    COUT(std::string(pszName), "Alice");
    std::string name = doc / "name" | "";
    COUT(name, "Alice");
    int age = doc / "age" | 0;
    COUT(age, 30);
    bool active = doc / "active" | false;
    COUT(active, true);
    
    /* use method: getor() */
    {
        const char* pszName = doc["name"].getor("");
        COUT(std::string(pszName), "Alice");
        std::string name = doc["name"].getor("");
        COUT(name, "Alice");
        int age = doc["age"].getor(0);
        COUT(age, 30);
        bool active = doc["active"].getor(false);
        COUT(active, true);
    }
#endif
}

DEF_TAST(api_2_25_2_pipe_function, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "alice"})"_xyjson;
    std::string upper = doc / "name" | [](const std::string& s) {
        std::string r = s;
        std::transform(r.begin(), r.end(), r.begin(), ::toupper);
        return r;
    };
    COUT(upper, "ALICE");
    
    /* use method: pipe() */
    {
        std::string upper = doc["name"].pipe([](const std::string& s) {
            std::string r = s;
            std::transform(r.begin(), r.end(), r.begin(), ::toupper);
            return r;
        });
        COUT(upper, "ALICE");
    }
#endif
}

DEF_TAST(api_2_26_2_value_set, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"name": "Alice"})");
    mutDoc / "name" = "Bob";
    COUT(mutDoc["name"] | std::string(), "Bob");
    mutDoc["age"] = 30;
    COUT(mutDoc["age"] | 0, 30);
    mutDoc / "active" = true; // 无效果
    COUT((mutDoc / "active").isValid(), false);
    //^ 结果：{"name":"Bob","age":30}
    
    /* use method: set() */
    {
        yyjson::MutableDocument mutDoc(R"({"name": "Alice"})");
        (mutDoc / "name").set("Bob");
        COUT(mutDoc["name"] | std::string(), "Bob");
        mutDoc["age"].set(30);
        COUT(mutDoc["age"] | 0, 30);
        (mutDoc / "active").set(true);
        COUT((mutDoc / "active").isValid(), false);
    }
#endif
}

DEF_TAST(api_2_27_1_composite_assignment_extract, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"age": 30})"_xyjson;
    int age = 18;
    age |= doc / "age";  // age = 30
    COUT(age, 30);
    
    /* use method: get() */
    {
        int age = 18;
        doc["age"].get(age);
        COUT(age, 30);
    }
#endif
}

DEF_TAST(api_2_27_2_iterator_multistep_advance, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30, 40]})"_xyjson;
    auto iter = doc / "items" % 0;
    iter += 3;  // 跳过 3 个元素
    COUT(*iter | 0, 40);
    
    /* use method: advance() */
    {
        auto iter = doc / "items" % 0;
        iter.advance(3);
        COUT(*iter | 0, 40);
    }
#endif
}

DEF_TAST(api_2_27_3_iterator_relocate_assign, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30, 40, 50]})"_xyjson;
    auto iter = doc / "items" % 4;
    COUT(*iter | 0, 50);
    iter %= 2;  // 重定位到索引 2
    COUT(*iter | 0, 30);
    
    /* use method: advance() */
    {
        auto iter = doc / "items" % 4;
        COUT(*iter | 0, 50);
        iter.advance(2);
        COUT(iter.isValid(), false);
    }
#endif
}

DEF_TAST(api_2_28_1_doc_index_operator, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document array("[10, 20, 30]");
    yyjson::MutableDocument object("{}");
    
    auto first = array[0];
    COUT(first | 0, 10);
    object["first"] = 10;
    COUT(object["first"] | 0, 10);
    
    /* use method: root() */
    {
        auto first = array.root()[0];
        COUT(first | 0, 10);
        object.root()["second"] = 20;
        COUT(object["second"] | 0, 20);
    }
#endif
}

DEF_TAST(api_2_28_2_doc_unary_plus, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"([10, 20, 30])"_xyjson;
    int size = +doc;  // 3
    COUT(size, 3);
    
    /* use method: root() */
    {
        int size = +doc.root(); // 3
        COUT(size, 3);
    }
#endif
}

DEF_TAST(api_2_28_3_doc_unary_minus, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    std::string str = -doc; // {"name":"Alice"}
    COUT(str, R"({"name":"Alice"})");
    
    /* use method: root() */
    {
        std::string str = -doc.root();
        COUT(str, R"({"name":"Alice"})");
    }
#endif
}

DEF_TAST(api_2_28_4_doc_path_operator, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
        "items": [10, 20, 30]})"_xyjson;
    
    auto name = doc / "user" / "name";
    COUT(name | std::string(), "Alice");
    auto age = doc / "/user/age";
    COUT(age | 0, 30);
    auto item = doc / "/items/0";
    COUT(item | 0, 10);
    
    /* use method: root() */
    {
        auto name = doc.root() / "user" / "name";
        COUT(name | std::string(), "Alice");
        auto age = doc.root() / "/user/age";
        COUT(age | 0, 30);
        auto item = doc.root() / "/items/0";
        COUT(item | 0, 10);
    }
#endif
}

DEF_TAST(api_2_28_5_doc_iterator_operator, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document array("[10, 20, 30]");
    yyjson::Document object(R"({"name": "Alice", "age": 30})");
    
    auto arrIt = array % 0;
    COUT(*arrIt | 0, 10);
    auto objIt = object % "";
    COUT(objIt.isValid(), true);
    COUT(*objIt | "", "Alice");
    
    /* use method: root() */
    {
        auto arrIt = array.root() % 0;
        COUT(*arrIt | 0, 10);
        auto objIt = object.root() % "";
        COUT(objIt.isValid(), true);
        COUT(*objIt | "", "Alice");
    }
#endif
}

DEF_TAST(api_2_28_6_doc_stream_output, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
//+ std::cout << doc << std::endl;
    std::ostringstream oss1;
    oss1 << doc;
    COUT(oss1.str(), R"({"name":"Alice"})");
    
    /* use method: root() */
//+ std::cout << doc.root() << std::endl;
    std::ostringstream oss2;
    oss2 << doc.root();
    COUT(oss2.str(), R"({"name":"Alice"})");
    
    // 格式化输出需显式调用 toString(true)
//+ std::cout << doc.root().toString(true) << std::endl;
    std::ostringstream oss3;
    oss3 << doc.root().toString(true);
    COUT(oss3.str().find("\n") != std::string::npos, true);
#endif
}

DEF_TAST(api_2_28_7_doc_equality, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc1 = R"({"name": "Alice"})"_xyjson;
    yyjson::Document doc2 = R"({"name": "Alice"})"_xyjson;
    bool docEqual = doc1 == doc2;
    COUT(docEqual, true);
    
    /* use method: root() */
    {
        bool rootEqual = doc1.root() == doc2.root();
        COUT(rootEqual, true);
    }
#endif
}

DEF_TAST(api_3_1_type_check_with_constants, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice", "age": 30})"_xyjson;
    
    // 使用类型常量
    {
        using namespace yyjson;
        bool isString = doc / "name" & kString; // true
        bool isInt = doc / "age" & kInt;        // true
        bool isNumber = doc / "age" & kNumber;  // true
        bool isDouble = doc / "age" & kReal;    // false
        COUT(isString, true);
        COUT(isInt, true);
        COUT(isNumber, true);
        COUT(isDouble, false);
    }
    
    // 使用字面量
    {
        bool isString = doc / "name" & "";   // true
        bool isInt = doc / "age" & 0;        // true
        // isNumber 只能用 kNumber 判断
        bool isDouble = doc / "age" & 0.0;   // false
        COUT(isString, true);
        COUT(isInt, true);
        COUT(isDouble, false);
    }
    
#endif
}

DEF_TAST(api_3_2_value_extract_with_constants, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice", "age": 30})"_xyjson;
    
    // 使用类型常量
    {
        using namespace yyjson;
        const char* pszName = doc / "name" | kString; // Alice
        COUT(std::string(pszName), "Alice");
        int iAge = doc / "age" | kInt;      // 30
        COUT(iAge, 30);
        double fAage = doc / "age" | kReal; // 0.0
        COUT(fAage, 0.0);
        double nAage = doc / "age" | kNumber; // 30.0
        COUT(nAage, 30.0);
    }
    
    // 使用字面量
    {
        const char* pszName = doc / "name" | ""; // Alice
        COUT(std::string(pszName), "Alice");
        int iAge = doc / "age" | 0;      // 30
        COUT(iAge, 30);
        double fAage = doc / "age" | 0.0; // 0.0
        COUT(fAage, 0.0);
        // kNumber 没有对应的字面量，或者可以用 ZeroNumber 构造函数
        double nAage = doc / "age" | ZeroNumber(); // 30.0
        COUT(nAage, 30.0);
    }
#endif
}

DEF_TAST(api_3_3_assignment_with_constants, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
    
    // 使用类型常量
    {
        using namespace yyjson;
        mutDoc / "name" = kString;
        COUT(mutDoc["name"].isString(), true);
        mutDoc / "age" = kInt;
        COUT(mutDoc["age"].isInt(), true);
    }
    
    // 使用字面量
    {
        mutDoc / "name" = "";
        COUT(mutDoc["name"].isString(), true);
        mutDoc / "age" = 0;
        COUT(mutDoc["age"].isInt(), true);
    }
#endif
}

DEF_TAST(api_3_4_1_container_type_check, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
        "items": [10, 20, 30]})"_xyjson;
    
    // 使用类型常量
    {
        using namespace yyjson;
        bool isArray = doc / "items" & kArray;    // true
        COUT(isArray, true);
        bool isObject = doc / "user" & kObject;   // true
        COUT(isObject, true);
    }
    
    // 使用字面量
    {
        bool isArray = doc / "items" & "[]"; // true
        COUT(isArray, true);
        bool isObject = doc / "user" & "{}";    // true
        COUT(isObject, true);
    }
#endif
}

DEF_TAST(api_3_4_2_container_type_conversion, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
        "items": [10, 20, 30]})"_xyjson;
    
    auto array = doc / "items" | yyjson::kArray;
    COUT(array.size(), 3);
    auto object = doc / "user" | yyjson::kObject;
    COUT(object.size(), 2);
    
    // 使用具名方法
    {
        auto array = doc["items"].array();
        COUT(array.size(), 3);
        auto object = doc["user"].object();
        COUT(object.size(), 2);
    }
    
    // 支持标准容器操作
    int arraySum = 0;
    for (auto value : array) {
        arraySum += value | 0;
    }
    COUT(arraySum, 60);
    
    int objectCount = 0;
    for (auto value : object) {
        objectCount++;
    }
    COUT(objectCount, 2);
    
    // 等效显式使用 begin/end 迭代器对
    {
        int arraySum = 0;
        for (auto it = array.begin(); it != array.end(); ++it) {
            auto value = *it;
            arraySum += value | 0;
        }
        COUT(arraySum, 60);
    }
    
    {
        int objectCount = 0;
        for (auto it = object.begin(); it != object.end(); ++it) {
            auto value = *it;
            objectCount++;
        }
        COUT(objectCount, 2);
    }
#endif
}

DEF_TAST(api_3_4_3_change_container_type, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
    mutDoc / "name" = yyjson::kObject;
    COUT(mutDoc["name"].isObject(), true);
    mutDoc / "age" = yyjson::kArray;
    COUT(mutDoc["age"].isArray(), true);
    COUT(mutDoc.root().toString(), R"({"name":{},"age":[]})");
    //^ 结果：{"name":{},"age":[]}
    
    // 使用字面量
    {
        yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
        mutDoc / "name" = "{}";
        COUT(mutDoc["name"].isObject(), true);
        mutDoc / "age" = "[]";
        COUT(mutDoc["age"].isArray(), true);
        COUT(mutDoc.root().toString(), R"({"name":{},"age":[]})");
    }
    
    // 使用具名方法
    {
        yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
        mutDoc["name"].setObject();
        COUT(mutDoc["name"].isObject(), true);
        mutDoc["age"].setArray();
        COUT(mutDoc["age"].isArray(), true);
        COUT(mutDoc.root().toString(), R"({"name":{},"age":[]})");
    }
#endif
}

DEF_TAST(api_3_4_4_create_empty_container, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc("{}");
    *mutDoc << "user" << yyjson::kObject;
    COUT(mutDoc["user"].isObject(), true);
    *mutDoc << "items" << yyjson::kArray;
    COUT(mutDoc["items"].isArray(), true);
    
    // 使用字面量
    {
        yyjson::MutableDocument mutDoc("{}");
        *mutDoc << "user" << "{}";
        COUT(mutDoc["user"].isObject(), true);
        *mutDoc << "items" << "[]";
        COUT(mutDoc["items"].isArray(), true);
    }
    
    mutDoc / "user" << "name" << "Alice" << "age" << 25;
    COUT(mutDoc["user"]["name"] | std::string(), "Alice");
    COUT(mutDoc["user"]["age"] | 0, 25);
    mutDoc / "items" << 10 << 20 << 30;
    COUT(mutDoc["items"].size(), 3);
    COUT(mutDoc["items"][0] | 0, 10);
    COUT(mutDoc["items"][1] | 0, 20);
    COUT(mutDoc["items"][2] | 0, 30);
#endif
}

DEF_TAST(api_3_4_4_set_root_container_type, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc; // 默认构造是空对象 "{}"
    mutDoc.root() = yyjson::kArray;
    COUT(mutDoc.root().isArray(), true);
    
    mutDoc.root() << 10 << 20 << 30;
    COUT(mutDoc.root().size(), 3);
    COUT(mutDoc.root()[0] | 0, 10);
    COUT(mutDoc.root()[1] | 0, 20);
    COUT(mutDoc.root()[2] | 0, 30);
    //^ 结果：[10,20,30]
#endif
}

DEF_TAST(api_4_1_get_pointer_method, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc(R"({"name": "Alice", "age":25})");
    yyjson::MutableDocument mutDoc = ~doc;
    
    yyjson_doc* ptrDoc = nullptr;
    yyjson_val* ptrVal = nullptr;
    ptrDoc = doc.get();             // not null
    COUT(ptrDoc != nullptr, true);
    ptrVal = doc["name"].get();     // not null
    COUT(ptrVal != nullptr, true);
    ptrVal = (doc / "nokey").get(); // nullptr
    COUT(ptrVal == nullptr, true);
    ptrVal = (doc["nokey"]).get();  // nullptr
    COUT(ptrVal == nullptr, true);
    
    yyjson_mut_doc* ptrMutDoc = nullptr;
    yyjson_mut_val* ptrMutVal = nullptr;
    ptrMutDoc = mutDoc.get();          // not null
    COUT(ptrMutDoc != nullptr, true);
    ptrMutVal = mutDoc["name"].get();     // not null
    COUT(ptrMutVal != nullptr, true);
    ptrMutDoc = mutDoc["name"].getDoc();  // not null
    COUT(ptrMutDoc != nullptr, true);
    ptrMutVal = (mutDoc / "nokey").get(); // nullptr
    COUT(ptrMutVal == nullptr, true);
    ptrMutVal = (mutDoc["nokey"]).get();  // not null, auto insert
    COUT(ptrMutVal != nullptr, true);
    ptrMutVal = (mutDoc / "nokey").get(); // not null
    COUT(ptrMutVal != nullptr, true);
#endif
}

DEF_TAST(api_4_2_extract_pointer_operator, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc(R"({"name": "Alice", "age":25})");
    yyjson::MutableDocument mutDoc = ~doc;
    
    yyjson_val* ptrVal = nullptr;
    yyjson_mut_doc* ptrMutDoc = nullptr;
    yyjson_mut_val* ptrMutVal = nullptr;
    
    if (doc / "name" >> ptrVal) { }      // true
    if (mutDoc / "name" >> ptrMutVal) {} // true
    if (mutDoc / "name" >> ptrMutDoc) {} // true
    COUT(doc / "name" >> ptrVal, true);
    COUT(mutDoc / "name" >> ptrMutVal, true);
    COUT(mutDoc / "name" >> ptrMutDoc, true);
    COUT(ptrVal != nullptr, true);
    COUT(ptrMutVal != nullptr, true);
    COUT(ptrMutDoc != nullptr, true);
    
    if (doc / "nokey" >> ptrVal) { }      // false
    COUT(doc / "nokey" >> ptrVal, false);
    if (mutDoc / "nokey" >> ptrMutVal) {} // false
    COUT(mutDoc / "nokey" >> ptrMutVal, false);
    if (mutDoc["nokey"] >> ptrMutVal) {}  // true
    COUT(mutDoc["nokey"] >> ptrMutVal, true);
    if (mutDoc / "nokey" >> ptrMutVal) {} // true
    COUT(mutDoc / "nokey" >> ptrMutVal, true);
#endif
}

DEF_TAST(api_4_3_c_style_functions, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc(R"({"user":{"name": "Alice", "age":25},
        "items":[10, 20, 30]})");
    yyjson::MutableDocument mutDoc = ~doc;
    
    yyjson_val* ptrVal = nullptr;
    yyjson_doc* ptrDoc = nullptr;
    yyjson_mut_doc* ptrMutDoc = nullptr;
    yyjson_mut_val* ptrMutVal = nullptr;
    
    // 以下 c_ 方法都返回非空指针
    ptrDoc = doc.c_doc();
    COUT(ptrDoc != nullptr, true);
    ptrVal = doc["user"].c_val();
    COUT(ptrVal != nullptr, true);
    ptrMutDoc = mutDoc.c_doc();
    COUT(ptrMutDoc != nullptr, true);
    ptrMutDoc = mutDoc["user"].c_doc();
    COUT(ptrMutDoc != nullptr, true);
    ptrMutVal = mutDoc["user"].c_val();
    COUT(ptrMutVal != nullptr, true);
    
    auto ita = doc / "items" % 0;
    COUT(ita.isValid(), true);
    auto ito = doc / "user" % "";
    COUT(ito.isValid(), true);
    auto mita = mutDoc / "items" % 0;
    COUT(mita.isValid(), true);
    auto mito = mutDoc / "user" % "";
    COUT(mito.isValid(), true);
    
    // 以下 c_ 方法也返回非空指针
    bool itaIter = (ita.c_iter() != nullptr);
    COUT(itaIter, true);
    bool itoIter = (ito.c_iter() != nullptr);
    COUT(itoIter, true);
    bool mitaIter = (mita.c_iter() != nullptr);
    COUT(mitaIter, true);
    bool mitoIter = (mito.c_iter() != nullptr);
    COUT(mitoIter, true);
    
    bool itaVal = (ita.c_val() != nullptr);
    COUT(itaVal, true);
    bool itoVal = (ito.c_val() != nullptr);
    COUT(itoVal, true);
    bool mitaVal = (mita.c_val() != nullptr);
    COUT(mitaVal, true);
    bool mitoVal = (mito.c_val() != nullptr);
    COUT(mitoVal, true);
    
    bool itoKey = (ito.c_key() != nullptr);
    COUT(itoKey, true);
    bool mitoKey = (mito.c_key() != nullptr);
    COUT(mitoKey, true);
    
    //! 数组迭代器没有定义 c_key 方法
    //! if (ita.c_key() != nullptr) {}
    //! if (mita.c_key() != nullptr) {}
#endif
}

