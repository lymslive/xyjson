/**
 * @file t_mutable.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief mutable document functionality test for xyjson
 * */
#include "couttast/couttast.h"
#include "xyjson.h"
#include <set>

/* t_mutable.cpp - 可变模型专属功能测试
 * 包含：
 * - 读写修改工作流
 * - 可变值输入
 * - 赋值和复制行为
 * - 字符串赋值
 * - 字符串引用赋值
 * - 数组追加
 * - 对象插入
 * - 可变文档创建方法
 * - 键值优化
 * - 文档追加
 * - 对象添加文档
 */

DEF_TAST(mutable_read_modify, "test read-modify workflow")
{
    std::string jsonText = R"json({
        "name": "original",
        "value": 100,
        "items": [1, 2, 3],
        "settings": {
            "enabled": false,
            "threshold": 5.5
        }
    })json";

    DESC("Read JSON and create mutable copy");
    yyjson::MutableDocument doc(jsonText);
    COUT(doc.hasError(), false);

    DESC("Modify existing values");
    doc / "name" = "modified";
    doc / "value" = 200;
    doc / "settings" / "enabled" = true;
    doc / "settings" / "threshold" = 7.8;

    DESC("Add new values");
    doc / "new_field" = "added_value"; //! no effect
    doc / "items" << 4 << 5;

    DESC("Verify modifications");
    COUT(doc / "name" | "", "modified");
    COUT(doc / "value" | 0, 200);
    COUT(doc / "settings" / "enabled" | false, true);
    COUT(doc / "settings" / "threshold" | 0.0, 7.8);
    COUT(doc / "new_field" | "", "");
    COUT((doc / "items").size(), 5);
    COUT((doc / "items")[4] | 0, 5);

    // doc / can not insert new key, but doc[] can
    doc["new_field"] = "added_value";
    COUT(doc / "new_field" | "", "added_value");
}

DEF_TAST(mutable_value_input, "test input operator << for mutable value")
{
    DESC("Create array and append values");
    {
        yyjson::MutableDocument doc("[]");

        // Append various types
        doc.root() << 1;
        doc.root() << "two";
        //= doc.root() << 3.14;
        //= doc.root() << false;
        //= doc.root() << std::string("five");
        auto& root = doc.root();
        root << 3.14;
        root << false;
        root << std::string("five");

        DESC("Verify array contents");
        COUT(doc.root().size(), 5);
        COUT(doc.root()[0] | 0, 1);
        COUT(doc.root()[1] | "", "two");
        COUT(doc.root()[2] | 0.0, 3.14);
        COUT(doc.root()[3] | true, false);
        //! COUT(doc.root()[4] | "", "five"); // error to compare c_str
        COUT(doc / 4 | std::string(), "five");

        DESC("Test chained appends");
        //= doc.root() << 6 << 7 << 8;
        +doc << 6 << 7 << 8;
        COUT(doc.root().size(), 8);
        COUT(doc / 7 | 0, 8);
    }

    DESC("Create object and add values");
    {
        yyjson::MutableDocument doc("{}");

        +doc << "first" << 1 << "second" << 2;
        std::string third("third");
        const char* fourth = "fourth";
        +doc << third << 3.14 << fourth << false;

        COUT(doc.root().size(), 4);
        COUT(doc / "first" | 0, 1);
        COUT(doc / "second" | 0, 2);
        COUT(doc / "third" | 0.0, 3.14);
        COUT(doc / "fourth" | true, false);

        // verify key is reference
        auto it = +doc %  "second";
        COUT(it->key, "second");
        ++it;
        COUT(it->key == third, true);
        COUT(it->key == third.c_str(), false);
        ++it;
        COUT(it->key == fourth, false);
        COUT(::strcmp(it->key, fourth), 0);

        // verify value is reference
        +doc << "fifth" << "5th";
        COUT(doc / "fifth" | "", "5th");
    }
}

DEF_TAST(mutable_assign_copy, "test = and copy behavior of yyjson wrapper classes")
{
    std::string jsonText = R"json({
        "name": "original",
        "value": 100,
        "items": [1, 2, 3],
        "settings": {
            "enabled": false,
            "threshold": 5.5
        }
    })json";

    // Document cannot copy or assign
    {
        yyjson::Document doc(jsonText);
//!     yyjson::Document doc2 = doc;
//!     yyjson::Document doc2(doc);
        yyjson::Document doc2;
//!     doc2 = doc;
    }

    // MutableDocument cannot copy or assign
    {
        yyjson::MutableDocument doc(jsonText);
//!     yyjson::MutableDocument doc2 = doc;
//!     yyjson::MutableDocument doc2(doc);
        yyjson::MutableDocument doc2;
//!     doc2 = doc;
    }

    // Value copy/assign
    {
        yyjson::Document doc(jsonText);
        yyjson::Value name = doc / "name";
        yyjson::Value name2 = name;
        yyjson::Value name3;
        name3 = name;
        COUT(name | "", std::string("original"));
        COUT(name2 | "", name | "");
        COUT(name3 | "", name | "");

        auto root = doc / "";
        auto settings = doc / "settings";
        auto threshold = settings / "threshold";
        auto setting2 = settings;
        auto enable = setting2 / "enabled";
        COUT(threshold | 0.0, 5.5);
        COUT(enable | true, false);
        COUT(settings / "enabled" | true, false);
    }

    // MutableValue copy/assign
    {
        yyjson::MutableDocument doc(jsonText);
        yyjson::MutableValue name = doc / "name";
        yyjson::MutableValue name2 = name;
        yyjson::MutableValue name3;
        name3 = name;
        COUT(name | "", std::string("original"));
        COUT(name2 | "", name | "");
        COUT(name3 | "", name | "");

        auto root = doc / "";
        auto settings = doc / "settings";
        auto threshold = settings / "threshold";
        auto setting2 = settings;
        auto enable = setting2 / "enabled";
        COUT(threshold | 0.0, 5.5);
        COUT(enable | true, false);
        COUT(settings / "enabled" | true, false);

        // name, name2, name3 all refer to the same json node.
        name = "modified";
        COUT(name | "", "modified");
        COUT(name2 | "", "modified");
        COUT(name3 | "", "modified");

        threshold = 6.5;
        enable = true;
        COUT(settings / "threshold" | 0.0, 6.5);
        COUT(setting2 / "threshold" | 0.0, 6.5);
        COUT(settings / "enabled" | false, true);
        COUT(setting2 / "enabled" | false, true);
    }

    // MutableValue assign new key
    {
        yyjson::MutableDocument doc(jsonText);
        auto root = +doc;

        root / "Name" = "Original";
        COUT((root / "Name").isString(), false);
        root["Name"] = "Original";
        COUT((root / "Name").isString(), true);
        COUT((root / "Name") | std::string(), "Original");
    }
}

DEF_TAST(mutable_assign_string, "test string node in yyjson")
{
    yyjson::MutableDocument doc("[]");

    std::string five("five");
    char six[] = "six";

    +doc << 1 << "two" << 3.14 << false << five << six;

    const char* getcstr = nullptr;
    getcstr = doc / 1 | "";
    COUT(getcstr, "two");
    COUT(::strcmp(getcstr, "two"), 0);

    // not same poniter, but same content
    getcstr = doc / 4 | "";
    COUT(getcstr == five.c_str(), false);
    COUT(::strcmp(getcstr, five.c_str()), 0);
    getcstr = doc / 5 | "";
    COUT(getcstr == six, false);
    COUT(::strcmp(getcstr, six), 0);

    // modify source string, how about the one in json node
    five.clear();
    getcstr = doc / 4 | "";
    COUT(::strcmp(getcstr, "five"), 0); // Should still be "five", not ""

    six[0] = 'S';
    std::string getSix = doc / 5 | "";
    COUT(getSix, "six"); // Should still be "six", not "Six"

    // assign to existed node also change to string node.
    COUT((doc/0).typeName());
    COUT((doc/2).typeName());
    doc / 0 = five.c_str();
    doc / 2 = six;
    COUT((doc/0).typeName());
    COUT((doc/2).typeName());

    COUT(strcmp(doc / 0 | "", ""), 0);
    COUT(strcmp(doc / 2 | "", six), 0);

    five = "Five";
    COUT(strcmp(doc / 0 | "", ""), 0); // Should still be "", not "Five"
    COUT(doc / 0 | std::string(), "");

    six[2] = 'X';
    COUT(strcmp(doc / 2 | "", "Six"), 0); // Should still be "Six", not "SiX"
    COUT(strcmp(doc / 5 | "", "six"), 0); // Should still be "six", not "SiX"
    COUT(doc / 2 | std::string(), "Six");
    COUT(doc / 5 | std::string(), "six");

    auto item0 = doc / 0;
    auto item2 = doc / 2;
    auto item5 = doc / 5;
    COUT(item0.isString(), true);
    COUT(item2.isString(), true);
    COUT(item5.isString(), true);
}

DEF_TAST(mutable_assign_string_ref, "test string node reference in yyjson")
{
    yyjson::MutableDocument doc("[]");

    // Use append_ref for string literals
    doc.root() << 1;
    doc.root().appendRef("two");
    doc.root() << 3.14 << false;
    doc.root().appendRef("five");
    doc.root().appendRef("six");

    const char* getcstr = nullptr;
    getcstr = doc / 1 | "";
    COUT(getcstr, "two");
    getcstr = doc / 4 | "";
    COUT(getcstr, "five");
    getcstr = doc / 5 | "";
    COUT(getcstr, "six");

    // Test that modifying literal strings would cause undefined behavior
    // (This is just for demonstration, in practice you should not do this)
    // For safety, we'll just verify the values are as expected
    COUT(strcmp(doc / 1 | "", "two"), 0);
    COUT(strcmp(doc / 4 | "", "five"), 0);
    COUT(strcmp(doc / 5 | "", "six"), 0);
    
    // Test setRef and setCopy methods
    yyjson::MutableValue node = doc / 0;
    node.setRef("ref_literal");
    COUT(strcmp(node | "", "ref_literal"), 0);
    
    node = doc / 1;
    node.setCopy("copy_string", strlen("copy_string"));
    COUT(strcmp(node | "", "copy_string"), 0);
    
    // Test that modifying source string affects ref but not copy
    char literal[] = "literal";
    node = doc / 2;
    node.setRef(literal);
    COUT(strcmp(node | "", "literal"), 0);
    
    strcpy(literal, "changed");
    COUT(strcmp(node | "", "changed"), 0); // Ref reflects the change
    
    std::string variable = "variable";
    node = doc / 3;
    node.setCopy(variable.c_str(), variable.size());
    COUT(strcmp(node | "", "variable"), 0);
    
    variable = "changed";
    COUT(strcmp(node | "", "variable"), 0); // Copy does not reflect the change
}

DEF_TAST(mutable_object_insertion, "test object insertion with KV macro and operator+")
{
    // temp str for operator|
    std::string str;

    DESC("Test KV macro and operator+ for object insertion");
    {
        yyjson::MutableDocument doc("{}");
        COUT(doc.hasError(), false);
        
        // Use KV macro with operator+ to insert key-value pairs
        doc.root() << KV("name", "test") << KV("value", 123) << KV("enabled", true);
        
        // Verify inserted values
        COUT(doc / "name" | str, "test");
        COUT(doc / "value" | 0, 123);
        COUT(doc / "enabled" | false, true);
        
        // Test string values with KV
        +doc << KV("string_value", "hello") << KV("number_value", 42.5);
        
        COUT(doc / "string_value" | str, "hello");
        COUT(doc / "number_value" | 0.0, 42.5);
        
        // Test complex nested object creation using "{}" special handling
        doc.root().addRef("nested", "{}");
        doc / "nested" << KV("level1", 1) << KV("level2", "deep");
        
        COUT(doc / "nested" / "level1" | 0, 1);
        COUT(doc / "nested" / "level2" | str, "deep");
    }
    
    DESC("Test direct insert method calls");
    {
        yyjson::MutableDocument doc("{}");
        
        // Test direct insert method calls
        doc.root().add("direct_int", 999);
        doc.root().add("direct_string", "direct");
        doc.root().add("direct_bool", true);
        
        COUT(doc / "direct_int" | 0, 999);
        COUT(doc / "direct_string" | str, "direct");
        COUT(doc / "direct_bool" | false, true);
        
        // Test insertRef for string literals
        doc.root().addRef("literal_key", "literal_value");
        COUT(doc / "literal_key" | "", "literal_value");
        
        // Test insertCopy for safety
        std::string temp_str = "temporary";
        doc.root().add("safe_key", temp_str.c_str());
        COUT(doc / "safe_key" | str, "temporary");
        
        // Modify original string to verify copy worked
        temp_str = "modified";
        COUT(doc / "safe_key" | str, "temporary"); // Should still be "temporary"
    }
    
    DESC("Test mixed array and object operations");
    {
        yyjson::MutableDocument doc("{}");
        
        // Create an array using "[]" special handling and add objects to it
        doc.root().addRef("items", "[]");
        
        // Array fail to add key-val
        doc / "items" << KV("name", "item1") << KV("id", 1);
        doc / "items" << KV("name", "item2") << KV("id", 2);
        COUT((doc / "items").size(), 0);

        doc / "items" << "item1" << 1 << "item2" << 2;
        COUT((doc / "items").size(), 4);
        COUT(doc / "items" / 0 | "", "item1");
        COUT(doc / "items" / 1 | 0, 1);
        COUT(doc / "items" / 2 | "", "item2");
        COUT(doc / "items" / 3 | 0, 2);
    }
}

DEF_TAST(mutable_array_append, "test append to MutableValue array")
{
    yyjson::MutableDocument doc("[]");
    
    DESC("Append various types to array");
    {
        // Test array append operations
        doc.root() << 1 << "two" << 3.14 << true;
        
        COUT(doc.root().size(), 4);
        COUT(doc / 0 | 0, 1);
        COUT(doc / 1 | "", "two");
        COUT(doc / 2 | 0.0, 3.14);
        COUT(doc / 3 | false, true);
        
        // Test chained appends
        doc.root() << "five" << 6 << 7.8 << false;
        COUT(doc.root().size(), 8);
        COUT(doc / 4 | "", "five");
        COUT(doc / 5 | 0, 6);
        COUT(doc / 6 | 0.0, 7.8);
        COUT(doc / 7 | true, false);
    }
    
    DESC("Test nested array append");
    {
        // Create a new array document
        yyjson::MutableDocument arrDoc("[]");
        
        // Append values to nested array
        arrDoc.root() << "nested" << 42 << 3.14;
        COUT(arrDoc.root().size(), 3);
        COUT(arrDoc / 0 | "", "nested");
        COUT(arrDoc / 1 | 0, 42);
        COUT(arrDoc / 2 | 0.0, 3.14);
        
        // Append the nested array to original array
        doc.root() << arrDoc.root();
        COUT(doc.root().size(), 9);
        
        // Verify nested array is appended correctly
        auto nested = doc / 8;
        COUT(nested.size(), 3);
        COUT(nested / 0 | "", "nested");
        COUT(nested / 1 | 0, 42);
        COUT(nested / 2 | 0.0, 3.14);
    }
    
    DESC("Test object append to array");
    {
        // Create an object and append to array
        yyjson::MutableDocument objDoc("{}");
        objDoc.root() << KV("name", "object") << KV("value", 999);
        
        doc.root() << objDoc.root();
        COUT(doc.root().size(), 10);
        
        // Verify object is appended correctly
        auto obj = doc / 9;
        COUT(obj / "name" | "", "object");
        COUT(obj / "value" | 0, 999);
    }
    
    DESC("Test array append with complex types");
    {
        // Test with std::string
        std::string strVal = "string_value";
        doc.root() << strVal;
        COUT(doc.root().size(), 11);
        COUT(doc / 10 | "", "string_value");
        
        // Test with const char*
        const char* cstrVal = "cstring_value";
        doc.root() << cstrVal;
        COUT(doc.root().size(), 12);
        COUT(doc / 11 | "", "cstring_value");
    }
}

DEF_TAST(mutable_create_methods, "test MutableDocument create methods and * operator")
{
    DESC("Test create() methods for various types");
    {
        yyjson::MutableDocument doc;
        
        // Verify default construction creates an empty object
        COUT(doc.isValid(), true);
        COUT(doc.root().isObject(), true);
        COUT(doc.root().size(), 0);
        
        // Test creating different types of nodes
        auto nullNode = doc.create();
        COUT(nullNode.isNull(), true);
        auto nullNode2 = doc.create(nullptr);
        COUT(nullNode2.isNull(), true);
        
        auto boolNode = doc.create(true);
        COUT(boolNode.isBool(), true);
        COUT(boolNode | false, true);
        
        auto intNode = doc.create(42);
        COUT(intNode.isNumber(), true);
        COUT(intNode | 0, 42);
        
        auto doubleNode = doc.create(3.14);
        COUT(doubleNode.isNumber(), true);
        COUT(doubleNode | 0.0, 3.14);
        
        auto stringNode = doc.createRef("hello");
        COUT(stringNode.isString(), true);
        COUT(stringNode | std::string(), "hello");
        // also compare pointer, the node refers literal string.
        COUT(stringNode | "", "hello");
        
        auto stdStringNode = doc.create(std::string("world"));
        COUT(stdStringNode.isString(), true);
        COUT(stdStringNode | std::string(), "world");

        std::string stdStr("World");
        auto stdStringNode2 = doc.create(stdStr);
        // compare string content
        COUT(stdStringNode2 | "", stdStr);
        const char* jsonString = stdStringNode2 | "";
        // compare string pointer
        COUT(jsonString == stdStr.c_str(), false);
    }
    
    DESC("Test multiplication operator for quick node creation");
    {
        yyjson::MutableDocument doc;
        
        // Test * operator for quick creation
        auto nullNode = doc * 1; // Should invoke create(int)
        COUT(nullNode.isNumber(), true);
        COUT(nullNode | 0, 1);
        
        auto boolNode = doc * true;
        COUT(boolNode.isBool(), true);
        COUT(boolNode | false, true);
        
        // Test with string literals (should use reference optimization)
        auto literalNode = doc * "string_literal";
        COUT(literalNode.isString(), true);
        COUT(literalNode | std::string(), "string_literal");
        COUT(literalNode | "", "string_literal");
    }
}

DEF_TAST(mutable_append_doc, "MutableValue array append with Document and MutableDocument")
{
    DESC("Append Document to array");
    {
        yyjson::MutableDocument target; // ("[]");
        target.root().setArray();

        yyjson::Document doc1("{\"name\":\"Alice\",\"age\":30}");
        COUT(doc1.isValid(), true);

        target.root().append(doc1);
        COUT(target.root().size() == 1, true);

        auto first = target / 0;
        COUT(first / "name" | std::string(), "Alice");
        COUT(first / "age" | 0, 30);

        +target << doc1;
        COUT(target.root().size() == 2, true);
        auto second = target / 1;
        COUT(second / "name" | std::string(), "Alice");
        COUT(second / "age" | 0, 30);
    }

    DESC("Append MutableDocument to array");
    {
        yyjson::MutableDocument target("[]");

        yyjson::MutableDocument doc2("{}");
        doc2.root().add("name", "Bob");
        doc2.root().add("age", 25);

        target.root().append(doc2);
        COUT(target.root().size(), 1);

        auto first = target / 0;
        COUT(first / "name" | std::string(), "Bob");
        COUT(first / "age" | 0, 25);

        +target << doc2;
        COUT(target.root().size() == 2, true);
        auto second = target / 1;
        COUT(second / "name" | std::string(), "Bob");
        COUT(second / "age" | 0, 25);
    }
}

DEF_TAST(mutable_objadd_doc, "MutableValue object add with Document and MutableDocument")
{
    DESC("Add Document to object");
    {
        yyjson::MutableDocument target;
        target.root().setObject();

        yyjson::Document doc1("{\"city\":\"Beijing\",\"country\":\"China\"}");
        COUT(doc1.isValid(), true);

        target.root().add("address", doc1);
        COUT(target.root().size(), 1);

        auto addr = target / "address";
        COUT(addr / "city" | std::string(), "Beijing");
        COUT(addr / "country" | std::string(), "China");

        +target << target * doc1 * "Addr";
        COUT(target.root().size(), 2);
        auto addr2 = target / "address";
        COUT(addr2 / "city" | std::string(), "Beijing");
        COUT(addr2 / "country" | std::string(), "China");
    }

    DESC("Add MutableDocument to object");
    {
        yyjson::MutableDocument target("{}");
        target.root().setObject();

        yyjson::MutableDocument doc2;
        doc2.root().setObject();
        doc2.root().add("street", "Main St");
        doc2.root().add("number", 123);

        target.root().add("location", doc2);
        COUT(target.root().size(), 1);

        auto loc = target.root()["location"];
        COUT(loc["street"] | std::string(), "Main St");
        COUT(loc["number"] | 0, 123);

        +target << target * doc2 * "Location";
        COUT(target.root().size(), 2);
        auto loc2 = target.root()["Location"];
        COUT(loc2.typeName());
        COUT(loc2["street"] | std::string(), "Main St");
        COUT(loc2["number"] | 0, 123);
    }
}

DEF_TAST(mutable_keyvalue_add, "test KeyValue optimization for object insertion")
{
    DESC("Test basic KeyValue creation and insertion");
    {
        yyjson::MutableDocument doc("{}");
        
        // Create a value and tag it with a key
        auto value = doc.create(42);
        auto kv = value.tag("number_key");
        
        // Insert the KeyValue into the object
        doc.root().add(kv);
        
        // Verify the insertion worked
        COUT(doc.root().size(), 1);
        COUT(doc.root()["number_key"] | 0, 42);
    }
    
    DESC("Test KeyValue with * operator chaining");
    {
        yyjson::MutableDocument doc("{}");
        
        // Chain * operators to create and tag in one expression
        doc.root().add(doc.create("hello") * "greeting");
        doc.root().add(doc.create(3.14) * "pi");
        doc.root().add(doc.create(true) * "flag");
        
        // Verify all insertions worked
        COUT(doc.root().size(), 3);
        COUT(doc.root()["greeting"] | std::string(), "hello");
        COUT(doc.root()["pi"] | 0.0, 3.14);
        COUT(doc.root()["flag"] | false, true);
    }
    
    DESC("Test string literal optimization in KeyValue");
    {
        yyjson::MutableDocument doc("{}");
        
        // For testing reference optimization, use actual string literals
        auto refKV = doc.create("test_value").tagRef("literal_key");
        doc.root().add(refKV);
        
        // For testing copy safety, use a variable
        char copyBuffer[20] = "copy_key";
        auto copyKV = doc.create("copy_value").tag(copyBuffer);
        doc.root().add(copyKV);
        
        // Modify the variable - the key should NOT change due to copying
        const char* modifiedCopy = "modified_key";
        strncpy(copyBuffer, modifiedCopy, sizeof(copyBuffer));
        copyBuffer[sizeof(copyBuffer) - 1] = '\0';
        
        // Verify both keys work correctly
        COUT(doc.root().size(), 2);
        COUT(doc.root()["literal_key"] | std::string(), "test_value");
        COUT(doc.root()["copy_key"] | std::string(), "copy_value"); // Should remain original
    }
    
    DESC("Test string literal KeyValue with operator*");
    {
        yyjson::MutableDocument doc("{}");

        char copyBuffer[20] = "copy_key";
        +doc << doc * "test_value" * "literal_key"
             << doc * "copy_value" * copyBuffer;

        // Modify the variable - the key should NOT change due to copying
        const char* modifiedCopy = "modified_key";
        strncpy(copyBuffer, modifiedCopy, sizeof(copyBuffer));
        copyBuffer[sizeof(copyBuffer) - 1] = '\0';

        // Verify both keys work correctly
        COUT(doc.root().size(), 2);
        COUT(doc.root()["literal_key"] | std::string(), "test_value");
        COUT(doc.root()["copy_key"] | std::string(), "copy_value"); // Should remain original
    }

    DESC("Test + operator with KeyValue");
    {
        yyjson::MutableDocument doc("{}");
        
        // Use + operator with KeyValue for concise syntax
        doc.root() << (doc.create("value1") * "key1")
                 << (doc.create(100) * "key2")
                 << (doc.create(false) * "key3");
        
        // Verify all insertions worked
        COUT(doc.root().size(), 3);
        COUT(doc.root()["key1"] | std::string(), "value1");
        COUT(doc.root()["key2"] | 0, 100);
        COUT(doc.root()["key3"] | true, false);
    }
    
    DESC("Test mixed KeyValue and traditional insertion");
    {
        yyjson::MutableDocument doc("{}");
        
        // Mix KeyValue optimization with traditional insertion
        doc.root() << (doc.create("optimized") * "key1"); // KeyValue optimization
        doc.root().add("key2", "traditional");        // Traditional insertion
        doc.root() << (doc.create(123) * "key3");         // KeyValue optimization
        
        // Verify all methods work together
        COUT(doc.root().size(), 3);
        COUT(doc.root()["key1"] | std::string(), "optimized");
        COUT(doc.root()["key2"] | std::string(), "traditional");
        COUT(doc.root()["key3"] | 0, 123);
    }
    
    DESC("Test proper object creation before KeyValue insertion");
    {
        yyjson::MutableDocument doc("{}");
        
        // First ensure the object exists
        doc.root().addRef("nested", "{}");
        
        // Now use KeyValue optimization on the nested object
        doc / "nested" << (doc.create("nested_value") * "nested_key");
        
        // Verify the insertion worked
        COUT((doc / "nested").size(), 1);
        COUT(doc / "nested" / "nested_key" | std::string(), "nested_value");

        std::string str("hello");
        // () is not necessary, operator* is higher priority than +.
        doc / "nested" << (doc * 123 * "num_key") << doc * str * "str_key";

        COUT((doc / "nested").size(), 3);
        COUT(doc / "nested" / "num_key" | 0, 123);
        COUT(doc / "nested" / "str_key" | std::string(), str);

        // exchange order of *
        doc / "nested" << "pre_key" * (doc * str);
        COUT((doc / "nested").size(), 4);
        COUT(doc / "nested" / "pre_key" | std::string(), str);

        //! not support, otherwise doc * doc maybe ambiguous
//      doc / "nested" << "pre_key2" * (str * doc);
//      COUT((doc / "nested").size(), 5);
//      COUT(doc / "nested" / "pre_key2" | std::string(), str);
    }
}

