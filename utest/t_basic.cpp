/**
 * @file t_basic.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief basic functionality test for xyjson
 * */
#include "couttast/couttast.h"
#include "xyjson.h"

/* t_basic.cpp - 基础功能测试（以只读模型为主）
 * 包含：
 * - 基础数值读取
 * - 基础字符串读取
 * - 错误处理
 * - 文档重新读取
 * - 索引操作符
 * - 比较操作符
 */

DEF_TAST(basic_read_number, "operator read from yyjson value")
{
    std::string jsonText = R"json({
        "aaa": 1, "bbb":2, "AAA": -1,
        "ccc": [3, 4, 5, 6],
        "ddd": {"eee":7, "fff":8.8}
    })json";

    // Parse JSON using yyjson
    yyjson::Document doc(jsonText);
    COUT(doc.hasError(), false);

    // Test path operator and value extraction
    int aaa = doc / "aaa" | 0;
    COUT(aaa, 1);

    int aa2;
    if (doc / "aaa" >> aa2)
    {
        COUT(aa2, 1);
    }
    else
    {
        COUT(false, true);
    }

    // Read "aaa" as uint64_t explicitly
    {
        uint64_t aaaUL = 0;
        aaaUL = doc / "aaa" | aaaUL;
        COUT(aaaUL, 1);
    }
    {
        auto aaaUL = doc / "aaa" | 0UL;
        COUT(aaaUL, 1);
    }

    // "aaa" is uint not sint in yyjson
    {
        int64_t aaaL = 0;
        aaaL = doc /"aaa" | aaaL;
        COUT(aaaL, 0);
    }
    {
        auto aaaL = doc / "aaa" | 0L;
        COUT(aaaL, 0);
    }

    COUT(doc / "AAA" | 0, -1);
    // "AAA" is sint not uint in yyjson
    {
        uint64_t aaaUL = 0;
        aaaUL = doc / "AAA" | aaaUL;
        COUT(aaaUL, 0);
    }
    {
        auto aaaUL = doc / "AAA" | 0UL;
        COUT(aaaUL, 0);
    }

    {
        int64_t aaaL = 0;
        aaaL = doc / "AAA" | aaaL;
        COUT(aaaL, -1);
    }
    {
        auto aaaL = doc / "AAA" | 0L;
        COUT(aaaL, -1);
    }

    auto bbb = doc / "bbb" | 0;
    COUT(bbb, 2);

    auto c0 = doc / "ccc" / 0 | 0;
    COUT(c0, 3);
    auto c1 = doc / "ccc" / 1 | 0;
    COUT(c1, 4);
    auto c2 = doc / "ccc" / 2 | 0;
    COUT(c2, 5);
    auto c3 = doc / "ccc" / 3 | 0;
    COUT(c3, 6);

    auto eee = doc / "ddd" / "eee" | 0;
    COUT(eee, 7);

    auto fff = doc / "ddd" / "fff" | 0.0;
    COUT(fff, 8.8);

    int eeeLeft = 0;
    eeeLeft |= doc / "ddd" / "eee";
    COUT(eeeLeft, 7);

    double fffLeft = 0.0;
    fffLeft |= doc / "ddd" / "fff";
    COUT(fffLeft, 8.8);
}

DEF_TAST(basic_read_string, "operator read from yyjson value")
{
    std::string jsonText = R"json({
    "aaa": "v1", "bbb":false, "BBB": "false",
    "ccc": ["v3", "v4", "v5", "v6"],
    "ddd": {"eee":"v7", "fff":"v8.8"}
})json";

    yyjson::Document doc(jsonText);
    COUT(doc.hasError(), false);

    const char* aaaPtr = doc / "aaa" | "";
    COUT(::strcmp(aaaPtr, "v1"), 0);
    std::string aaaStr = doc / "aaa" | "";
    COUT(aaaStr, "v1");
    {
        std::string tmp;
        std::string aaaStr2 = doc / "aaa" | tmp;
        COUT(aaaStr2, "v1");
    }

    std::string bbbStr = doc / "bbb" | "";
    COUT(bbbStr.empty(), true);

    bool bbb = doc / "bbb" | true;
    COUT(bbb, false);

    bbbStr = doc / "BBB" | "";
    COUT(bbbStr.empty(), false);
    COUT(bbbStr, "false");

    std::string c0 = doc / "ccc" / 0 | "";
    COUT(c0, "v3");
    std::string c1 = doc / "ccc" / 1 | "";
    COUT(c1, "v4");
    std::string c2 = doc / "ccc" / 2 | "";
    COUT(c2, "v5");
    std::string c3 = doc / "ccc" / 3 | "";
    COUT(c3, "v6");
    std::string c4 = doc / "ccc" / 4 | "";
    COUT(c4.empty(), true);

    std::string eee = doc / "ddd" / "eee" | "";
    COUT(eee, "v7");
    std::string fff = doc / "ddd" / "fff" | "";
    COUT(fff, "v8.8");
}

DEF_TAST(basic_error_handling, "handle path operator error")
{
    std::string jsonText = R"json({
        "aaa": 1, "bbb":2,
        "ccc": [3, 4, 5, 6],
        "ddd": {"eee":7, "fff":8.8}
    })json";

    yyjson::Document doc(jsonText);
    COUT(doc.hasError(), false);

    // Test valid path
    auto eee = doc/"ddd"/"eee";
    COUT(!eee, false);
    COUT(eee.isNull(), false);

    // Test invalid path
    auto ggg = doc/"ddd"/"ggg";
    COUT(!ggg, true);
    COUT(ggg.isValid(), false);

    int result;
    if (eee >> result)
    {
        COUT(result, 7);
    }
    else
    {
        COUT(false, true); // should not reach here
    }

    // type dismatch
    std::string strResult;
    if (eee >> strResult)
    {
        COUT(false, true);
    }
    else
    {
        COUT(strResult.empty(), true);
    }

    if (ggg >> result)
    {
        COUT(false, true);
    }

    if (ggg >> strResult)
    {
        COUT(false, true);
    }
}

DEF_TAST(basic_reread, "test re-read document")
{
    DESC("read-only yyjson document");
    {
        yyjson::Document doc(R"({"aaa": 1})");

        int aaa = doc / "aaa" | 0;
        COUT(aaa, 1);

        doc.read(R"({"aaa": 2})");
        aaa = doc / "aaa" | 0;
        COUT(aaa, 2);
    }

    DESC("mutable yyjson document");
    {
        yyjson::MutableDocument doc(R"({"aaa": 1})");

        int aaa = doc / "aaa" | 0;
        COUT(aaa, 1);

        doc.read(R"({"aaa": 2})");
        aaa = doc / "aaa" | 0;
        COUT(aaa, 2);
    }
}

DEF_TAST(basic_index_operator, "test index method and operator[]")
{
    std::string jsonText = R"json({
        "array": [1, 2, 3, 4, 5],
        "object": {"key1": "value1", "key2": 42, "key3": true},
        "nested": {"arr": [10, 20, 30], "obj": {"a": 100, "b": 200}}
    })json";

    // Test with read-only Value
    yyjson::Document doc(jsonText);
    COUT(doc.hasError(), false);

    // Test array indexing with index() method
    auto arrayVal = doc.root().index("array");
    COUT(arrayVal.isArray(), true);
    COUT(arrayVal.size(), 5);

    COUT(arrayVal.index(0).getor(0), 1);
    COUT(arrayVal.index(1).getor(0), 2);
    COUT(arrayVal.index(4).getor(0), 5);

    // Test array indexing with operator[]
    COUT(arrayVal[0].getor(0), 1);
    COUT(arrayVal[1].getor(0), 2);
    COUT(arrayVal[4].getor(0), 5);

    COUT(arrayVal[0] | 0, 1);
    COUT(arrayVal[1] | 0, 2);
    COUT(arrayVal[4] | 0, 5);

    // Test object indexing with index() method
    auto objectVal = doc.root().index("object");
    COUT(objectVal.isObject(), true);
    COUT(objectVal.size(), 3);

    COUT(objectVal.index("key1").getor(std::string()), "value1");
    COUT(objectVal.index("key2").getor(0), 42);
    COUT(objectVal.index("key3").getor(false), true);

    // Test object indexing with operator[]
    COUT(objectVal["key1"].getor(std::string()), "value1");
    COUT(objectVal["key2"].getor(0), 42);
    COUT(objectVal["key3"].getor(false), true);

    // Test nested access
    COUT(doc.root().index("nested").index("arr").index(1).getor(0), 20);
    COUT(doc.root()["nested"]["arr"][2].getor(0), 30);
    COUT(doc.root().index("nested").index("obj").index("a").getor(0), 100);
    COUT(doc.root()["nested"]["obj"]["b"].getor(0), 200);

    COUT(doc / "nested" / "arr" / 1 | 0, 20);
    COUT(doc / "nested" / "arr" / 2 | 0, 30);
    COUT(doc / "nested" / "obj" / "a" | 0, 100);
    COUT(doc / "nested" / "obj" / "b" | 0, 200);

    // Multi-level path without '/' prefix - not supported in head-only version
    // Must use JSON Pointer format starting with '/' for multi-level paths
    COUT(doc / "nested/arr/1" | 0, 0);  // returns 0 (not found)
    COUT(doc / "nested/arr/2" | 0, 0);  // returns 0 (not found)
    COUT(doc / "nested/obj/a" | 0, 0);  // returns 0 (not found)
    COUT(doc / "nested/obj/b" | 0, 0);  // returns 0 (not found)
    COUT(doc / "/nested/obj/b" | 0, 200);  // JSON Pointer format works

    DESC("use yyjson pointer: must begin with /");
    {
        COUT(yyjson_ptr_get(doc.root().get(), "nested/obj/b") == nullptr, true);
        COUT(yyjson_ptr_get(doc.root().get(), "/nested/obj/b") != nullptr, true);
    }

    // Test error cases
    COUT(arrayVal.index(10).isValid(), false);  // Out of bounds
    COUT(arrayVal[10].isValid(), false);        // Out of bounds
    COUT(objectVal.index("nonexistent").isValid(), false);  // Non-existent key
    COUT(objectVal["nonexistent"].isValid(), false);        // Non-existent key

    // Test with MutableValue (automatic insertion)
    yyjson::MutableDocument mutDoc;
    auto mutObj = mutDoc.createObject();
    
    // Add some values to test indexing
    mutObj["test_key"] = "test_value";
    mutObj["number_key"] = 42;

    // Test index method
    COUT(mutObj.index("test_key").getor(std::string()), "test_value");
    COUT(mutObj.index("number_key").getor(0), 42);
    
    // Test operator[]
    COUT(mutObj["test_key"].getor(std::string()), "test_value");
    COUT(mutObj["number_key"].getor(0), 42);
    
    // Test automatic insertion for non-existent keys
    auto autoInserted = mutObj["auto_inserted_key"];  // Should auto-insert null
    COUT(autoInserted.isNull(), true);
    
    // Verify the key was actually inserted
    COUT(mutObj.size(), 3);  // test_key, number_key, auto_inserted_key
    COUT(mutObj.index("auto_inserted_key").isNull(), true);
    
    // Test array operations with mutable values
    auto mutArray = mutDoc.createArray();
    mutArray.append(1).append(2).append(3);
    
    COUT(mutArray.index(0).getor(0), 1);
    COUT(mutArray[1].getor(0), 2);
    COUT(mutArray.index(2).getor(0), 3);
    
    // Test out of bounds for mutable array
    COUT(mutArray.index(5).isValid(), false);
    COUT(mutArray[5].isValid(), false);
}

DEF_TAST(basic_comparison_operators, "test comparison operators")
{
    using namespace yyjson;

    // Test Value comparison
    {
        Document doc1("{\"name\": \"John\", \"age\": 30}");
        Document doc2("{\"name\": \"John\", \"age\": 30}");
        Document doc3("{\"name\": \"Jane\", \"age\": 25}");

        COUT(doc1.root() == doc2.root(), true);
        COUT(doc1.root() == doc3.root(), false);
        COUT(doc1.root() != doc3.root(), true);

        // Test with null values
        Value nullValue;
        COUT(nullValue == doc1.root(), false);
        COUT(nullValue != doc1.root(), true);
    }

    // Test Document comparison
    {
        Document doc1("{\"data\": \"same\"}");
        Document doc2("{\"data\": \"same\"}");
        Document doc3("{\"data\": \"different\"}");

        COUT(doc1 == doc2, true);
        COUT(doc1 == doc3, false);
        COUT(doc1 != doc3, true);

        // Test with invalid documents
        Document invalidDoc;
        COUT(invalidDoc == doc1, false);
        COUT(invalidDoc != doc1, true);
    }

    // Test MutableValue comparison
    {
        MutableDocument doc1("{\"value\": 42}");
        MutableDocument doc2("{\"value\": 42}");
        MutableDocument doc3("{\"value\": 100}");

        COUT(doc1.root() == doc2.root(), true);
        COUT(doc1.root() == doc3.root(), false);
        COUT(doc1.root() != doc3.root(), true);
    }

    // Test MutableDocument comparison
    {
        MutableDocument doc1("{\"config\": \"test\"}");
        MutableDocument doc2("{\"config\": \"test\"}");
        MutableDocument doc3("{\"config\": \"prod\"}");

        COUT(doc1 == doc2, true);
        COUT(doc1 == doc3, false);
        COUT(doc1 != doc3, true);

        // Test deep comparison with nested structures
        MutableDocument nested1("{\"nested\": {\"a\": 1, \"b\": 2}}");
        MutableDocument nested2("{\"nested\": {\"a\": 1, \"b\": 2}}");
        MutableDocument nested3("{\"nested\": {\"a\": 1, \"b\": 3}}");

        COUT(nested1 == nested2, true);
        COUT(nested1 == nested3, false);
    }

    // Test array comparison
    {
        Document array1("[1, 2, 3]");
        Document array2("[1, 2, 3]");
        Document array3("[1, 2, 4]");

        COUT(array1.root() == array2.root(), true);
        COUT(array1.root() == array3.root(), false);
    }

    // Test string comparison
    {
        Document str1("\"hello\"");
        Document str2("\"hello\"");
        Document str3("\"world\"");

        COUT(str1.root() == str2.root(), true);
        COUT(str1.root() == str3.root(), false);
    }
}

DEF_TAST(basic_json_pointer, "test JSON Pointer functionality")
{
    using namespace yyjson;
    
    // Test complex nested structure for JSON Pointer
    std::string jsonText = R"json({
        "users": [
            {
                "id": 1,
                "name": "Alice",
                "address": {
                    "street": "123 Main St",
                    "city": "New York"
                }
            },
            {
                "id": 2,
                "name": "Bob",
                "address": {
                    "street": "456 Oak Ave",
                    "city": "Los Angeles"
                }
            }
        ],
        "config": {
            "version": "1.0",
            "settings": {
                "theme": "dark",
                "language": "en"
            }
        }
    })json";

    Document doc(jsonText);
    COUT(doc.hasError(), false);

    DESC("Test basic JSON Pointer functionality");
    
    // Test valid JSON Pointer paths
    COUT(doc / "/users/0/id" | 0, 1);
    COUT(doc / "/users/0/name" | "", "Alice");
    COUT(doc / "/users/0/address/city" | "", "New York");
    COUT(doc / "/users/1/id" | 0, 2);
    COUT(doc / "/users/1/name" | "", "Bob");
    COUT(doc / "/users/1/address/street" | "", "456 Oak Ave");
    COUT(doc / "/config/version" | "", "1.0");
    COUT(doc / "/config/settings/theme" | "", "dark");

    DESC("Test JSON Pointer with special characters in keys");
    
    // Test JSON with special characters in keys
    std::string specialJson = R"json({
        "a/b": "value1",
        "c~d": "value2", 
        "normal": "value3",
        "nested": {
            "e/f": "value4",
            "g~h": "value5",
            "/ij": "value6"
        }
    })json";

    Document docSpecial(specialJson);
    COUT(docSpecial.hasError(), false);

    // Test JSON Pointer with escaped characters
    COUT(docSpecial / "/a~1b" | "", "value1");  // '/' is escaped as '~1'
    COUT(docSpecial / "/c~0d" | "", "value2");  // '~' is escaped as '~0'
    COUT(docSpecial / "/normal" | "", "value3");
    COUT(docSpecial / "/nested/e~1f" | "", "value4");
    COUT(docSpecial / "/nested/g~0h" | "", "value5");
    COUT(docSpecial / "/nested/~1ij" | "", "value6");

    // Use simple path or index with special key.
    {
        auto nested = docSpecial / "nested";
        COUT(nested / "e/f" | "", "value4");
        COUT(nested / "g~h" | "", "value5");
        COUT(nested["e/f"] | "", "value4");
        COUT(nested["g~h"] | "", "value5");
        COUT(nested["/ij"] | "", "value6");
        COUT(nested / "/ij" | "", "");
    }

    DESC("Test JSON Pointer error cases");
    
    // Test invalid JSON Pointer paths
    COUT(!(doc / "/users/10"), true);  // Out of bounds
    COUT(!(doc / "/nonexistent"), true);  // Non-existent key
    COUT(!(doc / "/users/0/invalid"), true);  // Invalid property
    COUT(!(doc / "/config/invalid/key"), true);  // Deep non-existent path

    // Test empty path
    COUT((doc / "").isValid(), true);
    COUT((doc / "") == doc.root(), true);

    DESC("Test JSON Pointer vs single level indexing compatibility");
    
    // Test that single level paths still work (backward compatibility)
    COUT(doc / "users" / 0 / "name" | "", "Alice");
    COUT(doc / "config" / "version" | "", "1.0");

    // Test mixed usage
    auto users = doc / "users";
    COUT(users / "/0/id" | 0, 1);
    COUT(users / "/1/name" | "", "Bob");

    DESC("Test mutable document JSON Pointer functionality");
    
    // Test with mutable document
    MutableDocument mutDoc(jsonText);
    COUT(mutDoc.hasError(), false);

    COUT(mutDoc / "/users/0/id" | 0, 1);
    COUT(mutDoc / "/users/1/name" | "", "Bob");
    COUT(mutDoc / "/config/settings/theme" | "", "dark");

    // Test modification and then query
    auto mutConfig = mutDoc / "config" / "settings";
    mutConfig["theme"] = "light";
    COUT(mutDoc / "/config/settings/theme" | "", "light");

    DESC("Test JSON Pointer with numeric indices in arrays");
    
    // Test array indexing with JSON Pointer
    std::string arrayJson = R"json({
        "matrix": [
            [1, 2, 3],
            [4, 5, 6],
            [7, 8, 9]
        ]
    })json";

    Document docArray(arrayJson);
    COUT(docArray.hasError(), false);

    COUT(docArray / "/matrix/0/0" | 0, 1);
    COUT(docArray / "/matrix/0/1" | 0, 2);
    COUT(docArray / "/matrix/0/2" | 0, 3);
    COUT(docArray / "/matrix/1/0" | 0, 4);
    COUT(docArray / "/matrix/2/2" | 0, 9);

    // Test out of bounds array indices
    COUT(!(docArray / "/matrix/10/0"), true);
    COUT(!(docArray / "/matrix/0/10"), true);
}
