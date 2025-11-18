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
    
    // 等效：显式调用 yyjson::operator""_xyjson
    {
        const char* str = R"({"name": "Alice", "age": 30})";
        size_t len = ::strlen(str);
        auto doc = yyjson::operator""_xyjson(str, len);
    }
    
    // 等效：使用构造函数
    {
        yyjson::Document doc(R"({"name": "Alice", "age": 30})");
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
    }
    if (iter->isInt()) {
        int age = iter->toInteger(); // 结果：30
    }
    
    // 等效：使用 . 成员访问操作符
    // jsonT 类直接使用 . 访问，iteratorT 类要先解引用
    {
        auto json = doc / "age";
        auto iter = doc % "age";
        if (json.isInt()) {
            int age = json.toInteger(); // 结果：30
        }
        if ((*iter).isInt()) {
            int age = (*iter).toInteger(); // 结果：30
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
    
    /* use method: index() */
    {
        auto name = user.index("name");
    }
#endif
}

DEF_TAST(api_2_4_2_array_index_access, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    auto items = doc["items"];
    auto first = items[0];
    
    /* use method: index() */
    {
        auto first = items.index(0);
    }
#endif
}

DEF_TAST(api_2_4_3_mutable_object_add_field, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc["new_field"] = "value";
    mutDoc["array"] = "[]";
    
    /* use method: index() */
    {
        yyjson::MutableDocument mutDoc;
        mutDoc.root().index("new_field").set("value");
        mutDoc.root().index("array").set("[]");
    }
#endif
}

DEF_TAST(api_2_5_1_iterator_copy_advance, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    auto iter = doc / "items" % 0;
    auto oldIter = iter++;  // 返回原迭代器，然后iter前进
    
    /* use method: next() */
    {
        auto iter = doc / "items" % 0;
        auto oldIter = iter;
        iter.next();
    }
#endif
}

DEF_TAST(api_2_6_1_iterator_copy_retreat, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    auto iter = doc / "items" % 1;
    auto oldIter = iter--;  // 返回原迭代器，然后iter后退
    
    /* use method: prev() */
    {
        auto iter = doc / "items" % 0;
        auto oldIter = iter;
        iter.prev();
    }
#endif
}

DEF_TAST(api_2_7_1_iterator_advance, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    
    auto iter = doc / "items" % 0;
    ++iter;  // 原迭代器前进并返回
    
    /* use method: next() */
    {
        iter.next();
    }
#endif
}

DEF_TAST(api_2_8_1_iterator_retreat, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    
    auto iter = doc / "items" % 0;
    --iter;  // 原迭代器后退并返回
    
    /* use method: prev() */
    {
        iter.prev();
    }
#endif
}

DEF_TAST(api_2_9_1_invalid_value_check, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({})"_xyjson;
    
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
    if (doc) {
        // 文档有效，解析无错误
    }
    bool isValid = bool(doc / "name");
    
    /* use method */
    if (doc.hasError()) {
        // 文档解析出错
    }
    bool isValid2 = doc["name"].isValid();
#endif
}

DEF_TAST(api_2_10_1_bool_conversion_extended, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":false, "number":0, "string":"", "array":[], "object":{}})"_xyjson;
    
    bool hasNode = false;
    // 以下 hasNode 都返回 true
    hasNode = (bool)doc["null"];
    hasNode = (bool)doc["bool"];
    hasNode = (bool)doc["number"];
    hasNode = (bool)doc["string"];
    hasNode = (bool)doc["array"];
    hasNode = (bool)doc["object"];
    
    bool hasValue = true;
    // 以下 hasValue 都返回 false
    hasValue = (int)doc["null"];
    hasValue = (int)doc["bool"];
    hasValue = (int)doc["number"];
    hasValue = (int)doc["string"];
    hasValue = (int)doc["array"];
    hasValue = (int)doc["object"];
#endif
}

DEF_TAST(api_2_10_2_int_conversion, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    int iError  = (int)doc["error"];  // 0
    int iNull   = (int)doc["null"];   // 0
    int iBool   = (int)doc["bool"];   // 1
    int iNumber = (int)doc["number"]; // 3
    int iString = (int)doc["string"]; // 3
    int iArray  = (int)doc["array"];  // 2
    int iObject = (int)doc["object"]; // 3
    
    /* use method: toInteger() */
    {
        int iError  = doc["error"].toInteger();  // 0
        int iNull   = doc["null"].toInteger();   // 0
        int iBool   = doc["bool"].toInteger();   // 1
        int iNumber = doc["number"].toInteger(); // 3
        int iString = doc["string"].toInteger(); // 3
        int iArray  = doc["array"].toInteger();  // 2
        int iObject = doc["object"].toInteger(); // 3
    }
#endif
}

DEF_TAST(api_2_10_3_double_conversion, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14, "int":3,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    double fError  = (double)doc["error"];  // 0.0
    double fNull   = (double)doc["null"];   // 0.0
    double fBool   = (double)doc["bool"];   // 0.0
    double fNumber = (double)doc["number"]; // 3.14
    double fInt    = (double)doc["int"];    // 3.0
    double fString = (double)doc["string"]; // 0.0
    double fArray  = (double)doc["array"];  // 0.0
    double fObject = (double)doc["object"]; // 0.0
    
    /* use method: toNumber() */
    {
        double fError  = doc["error"].toNumber();  // 0.0
        double fNull   = doc["null"].toNumber();   // 0.0
        double fBool   = doc["bool"].toNumber();   // 0.0
        double fNumber = doc["number"].toNumber(); // 3.14
        double fInt    = doc["int"].toNumber();    // 3.0
        double fString = doc["string"].toNumber(); // 0.0
        double fArray  = doc["array"].toNumber();  // 0.0
        double fObject = doc["object"].toNumber(); // 0.0
    }
#endif
}

DEF_TAST(api_2_10_4_string_conversion, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    std::string strError  = (std::string)doc["error"];  // ""
    std::string strNull   = (std::string)doc["null"];   // null
    std::string strBool   = (std::string)doc["bool"];   // true
    std::string strNumber = (std::string)doc["number"]; // 3.14
    std::string strString = (std::string)doc["string"]; // 3.14pi
    std::string strArray  = (std::string)doc["array"];  // [null,null]
    std::string strObject = (std::string)doc["object"]; // {"x":0,"y":0,"z":0}
    std::string strRoot   = (std::string)doc.root();
    
    /* use method: toString() */
    {
        std::string strError  = doc["error"].toString();  // ""
        std::string strNull   = doc["null"].toString();   // null
        std::string strBool   = doc["bool"].toString();   // true
        std::string strNumber = doc["number"].toString(); // 3.14
        std::string strString = doc["string"].toString(); // 3.14pi
        std::string strArray  = doc["array"].toString();  // [null,null]
        std::string strObject = doc["object"].toString(); // {"x":0,"y":0,"z":0}
        std::string strRoot   = doc.root().toString();
    }
#endif
}

DEF_TAST(api_2_10_4_string_conversion_pretty, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    std::string strString = doc["string"].toString(true); // "3.14pi"
    std::string strArray  = doc["array"].toString(true);
    std::string strObject = doc["object"].toString(true);
#endif
}

DEF_TAST(api_2_10_5_doc_to_mutabledoc, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc("{}");
    yyjson::MutableDocument mutDoc(doc);
    
    /* use method: mutate() */
    {
        yyjson::MutableDocument mutDoc = doc.mutate();
    }
#endif
}

DEF_TAST(api_2_10_6_mutabledoc_to_doc, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc("{}");
    yyjson::Document doc(mutDoc);
    
    /* use method: freeze() */
    {
        yyjson::Document doc = mutDoc.freeze();
    }
#endif
}

DEF_TAST(api_2_11_1_doc_to_mutabledoc_bitwise, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc("{}");
    yyjson::MutableDocument mutDoc = ~doc;
    
    /* use method: mutate() */
    {
        yyjson::MutableDocument mutDoc = doc.mutate();
    }
#endif
}

DEF_TAST(api_2_11_2_mutabledoc_to_doc_bitwise, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc("{}");
    yyjson::Document doc = ~mutDoc;
    
    /* use method: freeze() */
    {
        yyjson::Document doc = mutDoc.freeze();
    }
#endif
}

DEF_TAST(api_2_11_3_iterator_get_key, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        auto keyNode = ~iter;  // 获取键结点
        auto valueNode = *iter;  // 获取值结点
        auto keyName = -iter;  // 获取键名
    }
    
    /* use method: key() */
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        auto keyNode = iter.key();  // 获取键结点
        auto valueNode = iter.value();  // 获取值结点
        auto keyName = iter.name();  // 获取键名
    }
#endif
}

DEF_TAST(api_2_12_1_iterator_get_value, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        auto value = *iter;  // 获取当前元素值
    }
    
    /* use method: value() */
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        auto value = iter.value();  // 获取当前元素值
    }
#endif
}

DEF_TAST(api_2_12_2_doc_get_root, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    auto root = *doc;
    
    yyjson::MutableDocument mutDoc(doc);
    auto mutRoot = *mutDoc;
    
    /* use method: root() */
    {
        auto root = doc.root();
        auto mutRoot = mutDoc.root();
    }
#endif
}

DEF_TAST(api_2_13_1_json_to_int, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    int iError  = +doc["error"];  // 0
    int iNull   = +doc["null"];   // 0
    int iBool   = +doc["bool"];   // 1
    int iNumber = +doc["number"]; // 3
    int iString = +doc["string"]; // 3
    int iArray  = +doc["array"];  // 2
    int iObject = +doc["object"]; // 3
    
    /* use method: toInteger() */
    {
        int iError  = doc["error"].toInteger();  // 0
        int iNull   = doc["null"].toInteger();   // 0
        int iBool   = doc["bool"].toInteger();   // 1
        int iNumber = doc["number"].toInteger(); // 3
        int iString = doc["string"].toInteger(); // 3
        int iArray  = doc["array"].toInteger();  // 2
        int iObject = doc["object"].toInteger(); // 3
    }
#endif
}

DEF_TAST(api_2_13_2_iterator_get_index, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        size_t idx = +iter;
        auto val = *iter;
    }
    
    /* use method: index() */
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        size_t idx = iter.index();
        auto val = iter.value();
    }
#endif
}

DEF_TAST(api_2_14_1_json_to_string, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"null":null, "bool":true, "number":3.14,
    "string":"3.14pi", "array":[null,null], "object":{"x":0, "y":0, "z":0}})"_xyjson;
    
    std::string strError  = -doc["error"];  // ""
    std::string strNull   = -doc["null"];   // null
    std::string strBool   = -doc["bool"];   // true
    std::string strNumber = -doc["number"]; // 3.14
    std::string strString = -doc["string"]; // 3.14pi
    std::string strArray  = -doc["array"];  // [null,null]
    std::string strObject = -doc["object"]; // {"x":0,"y":0,"z":0}
    std::string strRoot   = -doc.root();
    
    /* use method: toString() */
    {
        std::string strError  = doc["error"].toString();  // ""
        std::string strNull   = doc["null"].toString();   // null
        std::string strBool   = doc["bool"].toString();   // true
        std::string strNumber = doc["number"].toString(); // 3.14
        std::string strString = doc["string"].toString(); // 3.14pi
        std::string strArray  = doc["array"].toString();  // [null,null]
        std::string strObject = doc["object"].toString(); // {"x":0,"y":0,"z":0}
        std::string strRoot   = doc.root().toString();
    }
#endif
}

DEF_TAST(api_2_15_1_iterator_get_name, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        const char* keyName = -iter;
        auto valNode = *iter;
    }
    
    /* use method: name() */
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        const char* keyName = iter.name();
        auto valNode = iter.value();
    }
#endif
}

DEF_TAST(api_2_16_1_path_access, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
        "items": [10, 20, 30]})"_xyjson;
    auto name = *doc / "user" / "name";
    auto item = *doc / "items" / 0;
    
    /* use method: pathto() */
    {
        auto name = doc.root().pathto("user").pathto("name");
        auto item = doc.root().pathto("items").index(0);
    }
#endif
}

DEF_TAST(api_2_16_1_path_access_with_method, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    int age = (*doc / "user" / "age").toInteger();
    
    /* use operator[] */
    {
        int age = doc.root()["user"]["age"].toInteger();
    }
    
    /* use method: pathto() */
    {
        int age = doc.root().pathto("user").pathto("age").toInteger();
    }
#endif
}

DEF_TAST(api_2_16_2_json_pointer_access, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    auto node = *doc / "/user/name";
    
    /* use method: pathto() */
    {
        auto node = doc.root().pathto("/user/name");
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
    
    /* use method: seek() */
    {
        auto iter = doc / "config" % "";
        auto theme = iter.seek("theme");
        auto lang = iter.seek("lang");
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
    
    /* use method: create() */
    {
        auto intNode = mutDoc.create(314);
        auto strNode = mutDoc.create("new_value");
        auto newArray = mutDoc.create("[]");
        auto newObject = mutDoc.create("{}");
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
    
    /* use method: create() */
    {
        auto copyName = dstDoc.create(srcDoc / "name");
        auto copyAge = dstDoc.create(srcDocMut / "age");
        auto copyDoc = dstDoc.create(srcDoc);
        auto copyDocMut = dstDoc.create(srcDocMut);
    }
#endif
}

DEF_TAST(api_2_17_3_doc_move_node, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"(["Alice", 30])");
    
    auto node = mutDoc * 2.5;     // node 有效
    auto level = mutDoc * std::move(node); // node 失效
    mutDoc.root() << std::move(level);   // level 失效
    //^ 结果：["Alice",30,2.5]
    
    // 直接用链式表达式，临时值是纯右值，属移动语义
    *mutDoc << mutDoc * true; // 结果：["Alice",30,2.5,true]
    
    // 添加数组可以更简洁点，会自动调用 create
    *mutDoc << 100 << 200; // 结果：["Alice",30,2.5,true,100,200]
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
    
    // 使用连乘临时值，一元 `*` 表示 root()
    {
        yyjson::MutableDocument mutDoc("{}");
        *mutDoc << (mutDoc * "key") * (mutDoc * 314);
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
        auto kvConfig = mutDoc.create("{}").tag("config");
    }
    
    // 将绑定的结果插入根对象，生成: {"key":"value","config":{}}
    mutDoc.root() << std::move(keyValue) << std::move(kvConfig);
    
    // 连乘直接插入，一元 `*` 表示 root()
    {
        yyjson::MutableDocument mutDoc;
        *mutDoc << mutDoc * "value" * "key" << "config" * (mutDoc * "{}");
    }
#endif
}

DEF_TAST(api_2_18_1_array_iterator_create, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30]})"_xyjson;
    for (auto iter = doc / "items" % 0; iter; ++iter) {
        // 处理数组元素
    }
    
    /* use method: iterator() */
    for (auto iter = doc["items"].iterator(0); iter; ++iter) {
        // 处理数组元素
    }
#endif
}

DEF_TAST(api_2_18_2_object_iterator_create, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    for (auto iter = doc / "user" % ""; iter; ++iter) {
        // 处理对象键值对
    }
    
    /* use method: iterator() */
    for (auto iter = doc["user"].iterator(""); iter; ++iter) {
        // 处理对象键值对
    }
#endif
}

DEF_TAST(api_2_18_3_iterator_relocate, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
        "items": [10, 20, 30]})"_xyjson;
    auto itArray = doc / "items" % 1; // 创建迭代器初始位置 1
    auto itArrayCopy = itArray % 3;  // 重定位到索引3的元素
    auto itObject = *doc % "items"; // 创建迭代器初始位置 "items"
    auto itObjectCopy = itObject % "user"; // 重定位到 "user" 位置
    
    /* use method: advance() */
    {
        auto itArray = doc["items"].iterator(1);
        auto itArrayCopy = itArray;
        itArrayCopy.begin().advance(3);
        auto itObject = doc.root().iterator("items");
        auto itObjectCopy = itObject;
        itObjectCopy.begin().advance("user");
    }
#endif
}

DEF_TAST(api_2_19_1_iterator_advance_multistep, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30, 40, 50]})"_xyjson;
    auto iter = doc / "items" % 0;
    auto iterCopy = iter + 3;  // 前进3步，指向第4个元素
    
    /* use method: advance() */
    {
        auto iterCopy = iter;
        iterCopy.advance(3);
    }
#endif
}

DEF_TAST(api_2_20_1_stream_output, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    std::cout << doc / "name" << std::endl;
    
    /* use method: toString() */
    {
        std::cout << (doc / "name").toString() << std::endl;
    }
#endif
}

DEF_TAST(api_2_20_2_doc_read_string, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    bool success = doc << R"({"name": "Alice"})";
    
    /* use method: read() */
    {
        yyjson::Document doc;
        bool success = doc.read(R"({"name": "Alice"})");
    }
    
    // 也可直接用构造函数
    {
        yyjson::Document doc(R"({"name": "Alice"})");
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
    
    /* use method: push() */
    {
        yyjson::MutableDocument mutDoc;
        mutDoc["items"] = "[]";
        mutDoc["items"].push(1).push("two").push(3.14);
    }
    
    // push 是 json 容器通用方法，数组也可用 append()
    {
        yyjson::MutableDocument mutDoc;
        mutDoc["items"] = "[]";
        mutDoc["items"].append(1).append("two").append(3.14);
    }
#endif
}

DEF_TAST(api_2_20_5_object_insert, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    auto root = *mutDoc;
    root << "key1" << "value1" << "key2" << 42;
    
    /* use method: push() */
    {
        yyjson::MutableDocument mutDoc;
        auto root = *mutDoc;
        root.push("key1").push("value1").push("key2").push(42);
    }
    
    // 也可用 add() 方法接收两个参数
    {
        yyjson::MutableDocument mutDoc;
        auto root = *mutDoc;
        root.add("key1", "value1").add("key2", 42);
    }
#endif
}

DEF_TAST(api_2_20_5_object_insert_keyvalue, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    *mutDoc << mutDoc * "Alice" * "name" << "age" * (mutDoc * 25);
    //^ 结果：{"name":"Alice","age":25}
    
    // 删除结点
    yyjson::KeyValue age, name;
    *mutDoc >> age >> name;
    
    // 换个顺序重新插入
    *mutDoc << std::move(age) << std::move(name);
    //^ 结果：{"age":25,"name":"Alice"}
#endif
}

DEF_TAST(api_2_20_6_array_insert, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc["items"] = "[]";
    auto iter = mutDoc / "items" % 0;
    iter << 10 << 20 << 30;
    
    /* use method: insert() */
    {
        yyjson::MutableDocument mutDoc;
        mutDoc["items"] = "[]";
        auto iter = mutDoc / "items" % 0;
        iter.insert(10); ++iter;
        iter.insert(20); ++iter;
        iter.insert(30); ++iter;
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
    
    /* use method: insert() */
    {
        yyjson::MutableDocument mutDoc;
        mutDoc["user"] = "{}";
        auto iter = mutDoc / "user" % "";
        iter.insert("name"); iter.insert("Alice"); ++iter;
        iter.insert("age", 25); ++iter;
    }
#endif
}

DEF_TAST(api_2_21_1_doc_write_string, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    std::string str;
    doc >> str;
    
    /* use method: write() */
    {
        std::string str;
        doc.write(str);
    }
#endif
}

DEF_TAST(api_2_21_2_doc_write_file, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    std::ofstream file("output.json");
    bool success = doc >> file;
    
    /* use method: write() */
    {
        std::ofstream file("output.json");
        bool success = doc.write(file);
    }
    
    // 另有 writeFile() 方法接收文件名参数
    {
        bool success = doc.writeFile("output.json");
    }
#endif
}

DEF_TAST(api_2_21_3_value_extract, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc(R"({"name": "Alice", "age":25})");
    
    std::string name;
    if (doc / "name" >> name) {
        // 成功提取
    }
    
    int age;
    if (doc / "age" >> age) {
        // 成功提取
    }
    
    /* use method: get() */
    {
        std::string name;
        if (doc["name"].get(name)) {
            // 成功提取
        }
    
        int age;
        if (doc["age"].get(age)) {
            // 成功提取
        }
    }
#endif
}

DEF_TAST(api_2_21_4_array_pop, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
    yyjson::MutableValue p1, p2,p3;
    mutDoc / "items" >> p3;
    mutDoc / "items" >> p2 >> p1;
    
    /* use method: pop() */
    {
        yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
        yyjson::MutableValue p1, p2, p3;
        mutDoc["items"].pop(p3);
        mutDoc["items"].pop(p2).pop(p1);
    }
#endif
}

DEF_TAST(api_2_21_5_object_pop, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
    yyjson::KeyValue name, age;
    mutDoc / "user" >> age >> name;
    
    /* use method: pop() */
    {
        yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
        yyjson::KeyValue name, age;
        mutDoc["user"].pop(age).pop(name);
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
    it >> p2 >> p3;
    
    /* use method: remove() */
    {
        yyjson::MutableDocument mutDoc(R"({"items":[10, 20, 30]})");
        auto it = mutDoc / "items" % 0;
        auto p1 = it.remove();
        auto p2 = it.remove();
        auto p3 = it.remove();
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
    
    /* use method: remove() */
    {
        yyjson::MutableDocument mutDoc(R"({"user":{"name": "Alice", "age":25}})");
        auto it = mutDoc / "user" % "";
        auto name = it.remove();
        auto age = it.remove();
    }
#endif
}

DEF_TAST(api_2_22_1_json_less, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"a": 10, "b": 20})"_xyjson;
    if (doc / "a" < doc / "b") { } // true
    
    /* use method: less() */
    {
        auto result = doc["a"].less(doc["b"]);
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
    
    if (root1 == root2) { } // true
    
    /* use method: equal() */
    if (root1.equal(root2)) { }
#endif
}

DEF_TAST(api_2_23_2_json_scalar_equal, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name":"Alice", "age": 25})"_xyjson;
    
    if (doc  / "name" == "Alice") {} // true
    if (doc  / "age" == 25) {} // true
    if (doc  / "age" == 25.0) {} // false
#endif
}

DEF_TAST(api_2_23_3_iterator_equal, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30}})"_xyjson;
    
    // 用迭代器判断键是否存在
    auto it = doc / "user" % "age";
    if (it == doc["user"].endObject()) {} // false
    
    /* use method: equal() */
    if (it.equal(doc["user"].endObject())) {} // false
    
    // 更简洁的判断
    if(doc / "user" / "age") {} // true
    if(doc / "user" % "age") {} // true
#endif
}

DEF_TAST(api_2_23_4_array_iterator_not_equal, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items":[10, 20, 30]})"_xyjson;
    
    for (auto it = doc["items"].beginArray(); it != doc["items"].endArray(); ++it)
    {
    }
    
    /* use method: equal() */
    for (auto it = doc["items"].beginArray(); !it.equal(doc["items"].endArray()); ++it)
    {
    }
#endif
}

DEF_TAST(api_2_24_1_type_check, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice", "age": 30})"_xyjson;
    bool isString = doc / "name" & ""; // true
    bool isInt = doc / "age" & 0;      // true
    bool isReal = doc / "age" & 0.0;   // false
    bool isObject = doc.root() & "{}"; // true
    bool isArray = doc.root() & "[]";  // false
    
    /* use method: isType() */
    {
        bool isString = doc["name"].isType("");   // true
        bool isInt = doc["age"].isType(0);       // true
        bool isReal = doc["age"].isType(0.0);    // false
        bool isObject = doc.root().isType("{}"); // true
        bool isArray = doc.root().isType("[]");  // false
    }
#endif
}

DEF_TAST(api_2_25_1_get_with_default, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice", "age": 30, "active": true})"_xyjson;
    const char* pszName = doc / "name" | "";
    std::string name = doc / "name" | "";
    int age = doc / "age" | 0;
    bool active = doc / "active" | false;
    
    /* use method: getor() */
    {
        const char* pszName = doc["name"].getor("");
        std::string name = doc["name"].getor("");
        int age = doc["age"].getor(0);
        bool active = doc["active"].getor(false);
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
    
    /* use method: pipe() */
    {
        std::string upper = doc["name"].pipe([](const std::string& s) {
            std::string r = s;
            std::transform(r.begin(), r.end(), r.begin(), ::toupper);
            return r;
        });
    }
#endif
}

DEF_TAST(api_2_26_2_value_set, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"name": "Alice"})");
    mutDoc / "name" = "Bob";
    mutDoc["age"] = 30;
    mutDoc / "active" = true; // 无效果
    //^ 结果：{"name":"Bob","age":30}
    
    /* use method: set() */
    {
        yyjson::MutableDocument mutDoc(R"({"name": "Alice"})");
        (mutDoc / "name").set("Bob");
        mutDoc["age"].set(30);
        (mutDoc / "active").set(true);
    }
#endif
}

DEF_TAST(api_2_27_1_composite_assignment_extract, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"age": 30})"_xyjson;
    int age = 18;
    age |= doc / "age";  // age = 30
    
    /* use method: get() */
    {
        int age = 18;
        doc["age"].get(age);
    }
#endif
}

DEF_TAST(api_2_27_2_iterator_multistep_advance, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30, 40]})"_xyjson;
    auto iter = doc / "items" % 0;
    iter += 3;  // 跳过 3 个元素
    
    /* use method: advance() */
    {
        auto iter = doc / "items" % 0;
        iter.advance(3);
    }
#endif
}

DEF_TAST(api_2_27_3_iterator_relocate_assign, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"items": [10, 20, 30, 40, 50]})"_xyjson;
    auto iter = doc / "items" % 4;
    iter %= 2;  // 重定位到索引 2
    
    /* use method: advance() */
    {
        auto iter = doc / "items" % 4;
        iter.advance(2);
    }
#endif
}

DEF_TAST(api_2_28_1_doc_index_operator, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document array("[10, 20, 30]");
    yyjson::MutableDocument object("{}");
    
    auto first = array[0];
    object["first"] = 10;
    
    /* use method: root() */
    {
        auto first = array.root()[0];
        object.root()["second"] = 20;
    }
#endif
}

DEF_TAST(api_2_28_2_doc_unary_plus, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"([10, 20, 30])"_xyjson;
    int size = +doc;  // 3
    
    /* use method: root() */
    {
        int size = +doc.root(); // 3
    }
#endif
}

DEF_TAST(api_2_28_3_doc_unary_minus, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    std::string str = -doc; // {"name":"Alice"}
    
    /* use method: root() */
    {
        std::string str = -doc.root();
    }
#endif
}

DEF_TAST(api_2_28_4_doc_path_operator, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
        "items": [10, 20, 30]})"_xyjson;
    
    auto name = doc / "user" / "name";
    auto age = doc / "/user/age";
    auto item = doc / "/items/0";
    
    /* use method: root() */
    {
        auto name = doc.root() / "user" / "name";
        auto age = doc.root() / "/user/age";
        auto item = doc.root() / "items/0";
    }
#endif
}

DEF_TAST(api_2_28_5_doc_iterator_operator, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document array("[10, 20, 30]");
    yyjson::Document object(R"("{"name": "Alice", "age": 30}")");
    
    auto arrIt = array % 0;
    auto objIt = object % "";
    
    /* use method: root() */
    {
        auto arrIt = array.root() % 0;
        auto objIt = object.root() % "";
    }
#endif
}

DEF_TAST(api_2_28_6_doc_stream_output, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"name": "Alice"})"_xyjson;
    std::cout << doc << std::endl;
    
    /* use method: root() */
    std::cout << doc.root() << std::endl;
    
    // 格式化输出需显式调用 toString(true)
    std::cout << doc.root().toString(true) << std::endl;
#endif
}

DEF_TAST(api_2_28_7_doc_equality, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc1 = R"({"name": "Alice"})"_xyjson;
    yyjson::Document doc2 = R"({"name": "Alice"})"_xyjson;
    if (doc1 == doc2) {} // true
    
    /* use method: root() */
    if (doc1.root() == doc2.root()) {}
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
    }
    
    // 使用字面量
    {
        bool isString = doc / "name" & "";   // true
        bool isInt = doc / "age" & 0;        // true
        // isNumber 只能用 kNumber 判断
        bool isDouble = doc / "age" & 0.0;   // false
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
        int iAge = doc / "age" | kInt;      // 30
        double fAage = doc / "age" | kReal; // 0.0
        double nAage = doc / "age" | kNumber; // 30.0
    }
    
    // 使用字面量
    {
        const char* pszName = doc / "name" | ""; // Alice
        int iAge = doc / "age" | 0;      // 30
        double fAage = doc / "age" | 0.0; // 0.0
        // kNumber 没有对应的字面量，或者可以用 ZeroNumber 构造函数
        double nAage = doc / "age" | ZeroNumber(); // 30.0
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
        mutDoc / "age" = kInt;
    }
    
    // 使用字面量
    {
        mutDoc / "name" = "";
        mutDoc / "age" = 0;
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
        bool isObject = doc / "user" & kObject;   // true
    }
    
    // 使用字面量
    {
        bool isArray = doc / "items" & "[]"; // true
        bool isObject = doc / "user" & "{}";    // true
    }
#endif
}

DEF_TAST(api_3_4_2_container_type_conversion, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc = R"({"user": {"name": "Alice", "age": 30},
        "items": [10, 20, 30]})"_xyjson;
    
    auto array = doc / "items" | yyjson::kArray;
    auto object = doc / "user" | yyjson::kObject;
    
    // 使用具名方法
    {
        auto array = doc["items"].array();
        auto object = doc["user"].object();
    }
    
    // 支持标准容器操作
    for (auto value : array) { }
    for (auto value : object) { } // 只能隐式取到值结点，不能取到键结点
    
    // 等效显式使用 begin/end 迭代器对
    for (auto it = array.begin(); it != array.end(); ++it) {
        auto value = *it;
    }
    
    for (auto it = object.begin(); it != object.end(); ++it) {
        auto value = *it;
    }
#endif
}

DEF_TAST(api_3_4_3_change_container_type, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
    mutDoc / "name" = yyjson::kObject;
    mutDoc / "age" = yyjson::kArray;
    //^ 结果：{"name":{},"age":[]}
    
    // 使用字面量
    {
        yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
        mutDoc / "name" = "{}";
        mutDoc / "age" = "[]";
    }
    
    // 使用具名方法
    {
        yyjson::MutableDocument mutDoc(R"({"name": "Alice", "age": 30})");
        mutDoc["name"].setObject();
        mutDoc["age"].setArray();
    }
#endif
}

DEF_TAST(api_3_4_4_create_empty_container, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc("{}");
    *mutDoc << "user" << yyjson::kObject;
    *mutDoc << "items" << yyjson::kArray;
    
    // 使用字面量
    {
        yyjson::MutableDocument mutDoc("{}");
        *mutDoc << "user" << "{}";
        *mutDoc << "items" << "[]";
    }
    
    mutDoc / "user" << "name" << "Alice" << "age" << 25;
    mutDoc / "items" << 10, 20, 30;
#endif
}

DEF_TAST(api_3_4_4_set_root_container_type, "example from docs/api.md")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc; // 默认构造是空对象 "{}"
    mutDoc.root() = yyjson::kArray;
    
    mutDoc.root() << 10 << 20 << 30;
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
    ptrVal = doc["name"].get();     // not null
    ptrVal = (doc / "nokey").get(); // nullptr
    ptrVal = (doc["nokey"]).get();  // not null
    
    yyjson_mut_doc* ptrMutDoc = nullptr;
    yyjson_mut_val* ptrMutVal = nullptr;
    ptrMutDoc = mutDoc.get();          // not null
    ptrMutVal = mutDoc["name"].get();     // not null
    ptrMutDoc = mutDoc["name"].getDoc();  // not null
    ptrMutVal = (mutDoc / "nokey").get(); // nullptr
    ptrMutVal = (mutDoc["nokey"]).get();  // not null
    ptrMutVal = (mutDoc / "nokey").get(); // not null
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
    
    if (doc / "nokey" >> ptrVal) { }      // false
    if (mutDoc / "nokey" >> ptrMutVal) {} // false
    if (mutDoc["nokey"] >> ptrMutVal) {}  // true
    if (mutDoc / "nokey" >> ptrMutVal) {} // true
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
    ptrVal = doc["user"].c_val();
    ptrMutDoc = mutDoc.c_doc();
    ptrMutDoc = mutDoc["user"].c_doc();
    ptrMutVal = mutDoc["user"].c_val();
    
    auto ita = doc / "itmes" % 0;
    auto ito = doc / "user" % "";
    auto mita = mutDoc / "itmes" % 0;
    auto mito = mutDoc / "user" % "";
    
    // 以下 c_ 方法也返回非空指针
    if (ita.c_iter() != nullptr) {}
    if (ito.c_iter() != nullptr) {}
    if (mita.c_iter() != nullptr) {}
    if (mito.c_iter() != nullptr) {}
    
    if (ita.c_val() != nullptr) {}
    if (ito.c_val() != nullptr) {}
    if (mita.c_val() != nullptr) {}
    if (mito.c_val() != nullptr) {}
    
    if (ito.c_key() != nullptr) {}
    if (mito.c_key() != nullptr) {}
    
    //! 数组迭代器没有定义 c_key 方法
    //! if (ita.c_key() != nullptr) {}
    //! if (mita.c_key() != nullptr) {}
#endif
}

