/**
 * @file t_conversion.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief conversion functionality test for xyjson
 * */
#include "couttast/couttast.h"
#include "xyjson.h"

DEF_TAST(conversion_methods, "test toString(), toInteger(), and toNumber() conversion methods")
{
    std::string jsonText = R"json({
        "string_value": "hello",
        "int_value": 42,
        "negative_value": -15,
        "double_value": 3.14,
        "bool_true": true,
        "bool_false": false,
        "null_value": null,
        "array_value": [1, 2, 3],
        "object_value": {"key": "value"},
        "empty_array": [],
        "empty_object": {}
    })json";

    DESC("Test with read-only Value");
    {
        yyjson::Document doc(jsonText);
        COUT(doc.hasError(), false);

        // just check the output
        COUT(doc.root().toString());
        COUT(doc.root().toString(true));

        // Test string values
        COUT((doc / "string_value").toString(), "hello");
        COUT((doc / "string_value").toString(true), "\"hello\""); // with quotes
        COUT(-(doc / "string_value"), "hello"); // operator-

        // Test integer values
        COUT((doc / "int_value").toString(), "42");
        COUT((doc / "int_value").toInteger(), 42);
        COUT(+(doc / "int_value"), 42); // operator+
        COUT((doc / "int_value").toNumber(), 42.0);

        // Test negative integer values
        COUT((doc / "negative_value").toString(), "-15");
        COUT((doc / "negative_value").toInteger(), -15);
        COUT(+(doc / "negative_value"), -15);
        COUT((doc / "negative_value").toNumber(), -15.0);

        // Test double values
        COUT((doc / "double_value").toString(), "3.14");
        COUT((doc / "double_value").toInteger(), 3); // integer cast
        COUT(+(doc / "double_value"), 3);
        COUT((doc / "double_value").toNumber(), 3.14);

        // Test boolean values
        COUT((doc / "bool_true").toString(), "true");
        COUT((doc / "bool_true").toInteger(), 1);
        COUT(+(doc / "bool_true"), 1);
        COUT((doc / "bool_true").toNumber(), 0.0);

        COUT((doc / "bool_false").toString(), "false");
        COUT((doc / "bool_false").toInteger(), 0);
        COUT(+(doc / "bool_false"), 0);
        COUT((doc / "bool_false").toNumber(), 0.0);

        // Test null values
        COUT((doc / "null_value").toString(), "null"); // null literal
        COUT((doc / "null_value").toInteger(), 0); // 0 for null
        COUT(+(doc / "null_value"), 0);
        COUT((doc / "null_value").toNumber(), 0.0);

        // Test arrays (should use size for toNumber)
        COUT((doc / "array_value").toString().find("1,2,3") != std::string::npos, true);
        COUT((doc / "array_value").toInteger(), 3); // array size
        COUT(+(doc / "array_value"), 3);
        COUT((doc / "array_value").toNumber(), 0.0);
        COUT(-(doc / "array_value"), "[1,2,3]");

        // Test objects (should use size for toNumber)
        COUT((doc / "object_value").toString().find("key") != std::string::npos, true);
        COUT((doc / "object_value").toInteger(), 1); // object size
        COUT(+(doc / "object_value"), 1);
        COUT((doc / "object_value").toNumber(), 0.0);
        COUT(-(doc / "object_value"), R"({"key":"value"})");

        // Test empty arrays and objects
        COUT((doc / "empty_array").toInteger(), 0);
        COUT((doc / "empty_object").toInteger(), 0);
        COUT((doc / "empty_array").toNumber(), 0.0);
        COUT((doc / "empty_object").toNumber(), 0.0);

        // Test invalid values
        yyjson::Value invalidValue;
        COUT(invalidValue.toString(), "");
        COUT(invalidValue.toInteger(), 0);
        COUT(invalidValue.toNumber(), 0.0);
        COUT(-invalidValue, "");
        COUT(+invalidValue, 0);
    }

    DESC("Test with MutableValue");
    {
        yyjson::MutableDocument doc(jsonText);
        COUT(doc.hasError(), false);

        // Test string values
        COUT((doc / "string_value").toString(), "hello");
        COUT((doc / "string_value").toString(true), "\"hello\"");
        COUT(-(doc / "string_value"), "hello");

        // Test integer values
        COUT((doc / "int_value").toString(), "42");
        COUT((doc / "int_value").toInteger(), 42);
        COUT(+(doc / "int_value"), 42);
        COUT((doc / "int_value").toNumber(), 42.0);

        // Test modification and conversion
        (doc / "string_value") = "world";
        COUT(-(doc / "string_value"), "world");

        (doc / "int_value") = 100;
        COUT(+(doc / "int_value"), 100);
    }

    DESC("Test edge cases for toNumber() conversion");
    {
        yyjson::Document doc(R"json({"large_int": 999999, "large_negative": -999999})json");

        COUT(+(doc / "large_int"), 999999);
        COUT(+(doc / "large_negative"), -999999);
        COUT((doc / "large_int").toNumber(), 999999.0);
        COUT((doc / "large_negative").toNumber(), -999999.0);

        // Test string to number conversion (should use std::stoi)
        yyjson::Document doc2(R"json({"numeric_string": "123", "invalid_string": "abc"})json");

        COUT(+(doc2 / "numeric_string"), 123); // string "123" should convert to 123
        COUT((doc2 / "numeric_string").toNumber(), 0.0);
        COUT(+(doc2 / "invalid_string"), 0); // invalid string should return 0
        COUT((doc2 / "invalid_string").toNumber(), 0.0);
    }

    DESC("Test string content comparison with quotes");
    {
        yyjson::Document doc(R"json({"text": "hello world", "number": 123})json");

        std::string stringResult = (doc / "text").toString(true);
        COUT(stringResult.find("hello world") != std::string::npos, true);
        COUT(stringResult.find('\"') != std::string::npos, true);

        std::string numberResult = (doc / "number").toString(true);
        COUT(numberResult.find("123") != std::string::npos, true);
        COUT(numberResult.find('\"') == std::string::npos, true); // numbers shouldn't be quoted
    }
}

DEF_TAST(conversion_operators, "test unary operators - and + for conversion")
{
    std::string jsonText = R"json({
        "string": "test",
        "number": 99,
        "float": 7.5,
        "bool": true
    })json";

    DESC("Test unary operators with Value (updated semantics)");
    {
        yyjson::Document doc(jsonText);
        COUT(doc.hasError(), false);

        // Test operator- (should call toString())
        std::string strResult = -(doc / "string");
        COUT(strResult, "test");

        // Test operator+ (should call toNumber())
        int numResult = +(doc / "number");
        COUT(numResult, 99);

        // Test with float (should truncate)
        int floatResult = +(doc / "float");
        COUT(floatResult, 7);

        // Test with boolean
        int boolResult = +(doc / "bool");
        COUT(boolResult, 1);

        std::string boolStrResult = -(doc / "bool");
        COUT(boolStrResult, "true");

        // Test chaining with other operators
        int chainedResult = +(doc / "number") + 1;
        COUT(chainedResult, 100);

        std::string chainedStr = "prefix_" + -(doc / "string");
        COUT(chainedStr, "prefix_test");
    }

    DESC("Test unary operators with MutableValue");
    {
        yyjson::MutableDocument doc(jsonText);
        COUT(doc.hasError(), false);

        // Modify values and test conversion
        (doc / "string") = "modified";
        COUT(-(doc / "string"), "modified");

        (doc / "number") = 200;
        COUT(+(doc / "number"), 200);

        // Test edge case: empty string
        (doc / "string") = "";
        COUT(-(doc / "string"), "");
        COUT(+(doc / "string"), 0); // empty string should return 0
    }

    DESC("Test conversion consistency between methods and operators");
    {
        yyjson::Document doc(jsonText);

        // toString() should be equivalent to operator-
        COUT((doc / "string").toString(), -(doc / "string"));
        COUT((doc / "number").toString(), -(doc / "number"));

        // toInteger() should be equivalent to operator+
        COUT((doc / "number").toInteger(), +(doc / "number"));
        COUT((doc / "float").toInteger(), +(doc / "float"));

        // Test with quoted strings
        COUT((doc / "string").toString(true).find('\"') != std::string::npos, true);
        COUT((-(doc / "string")).find('\"') == std::string::npos, true); // operator- should not quote
    }

    DESC("Test invalid value handling with operators");
    {
        yyjson::Value invalidValue;

        COUT(-invalidValue, ""); // should return empty string
        COUT(+invalidValue, 0);  // should return 0

        // Test with invalid path
        yyjson::Document doc(jsonText);
        auto invalidPath = doc / "nonexistent";

        COUT(-invalidPath, "");
        COUT(+invalidPath, 0);
    }

    DESC("Test array size conversion");
    {
        yyjson::Document doc;
        doc.read("{\"arr\": [1, 2, 3, 4, 5]}");
        COUT(+ (doc / "arr"), 5); // 5-element array

        doc.read("{\"empty_arr\": []}");
        COUT(+ (doc / "empty_arr"), 0); // empty array
    }

    DESC("Test object size conversion");
    {
        yyjson::Document doc;
        doc.read("{\"obj\": {\"a\": 1, \"b\": 2, \"c\": 3}}");
        COUT(+ (doc / "obj"), 3); // 3-key object

        doc.read("{\"empty_obj\": {}}");
        COUT(+ (doc / "empty_obj"), 0); // empty object
    }

    DESC("Test nested structures");
    {
        yyjson::Document doc;
        doc.read("{\"nested\": {\"arr\": [1, 2], \"obj\": {\"x\": 1}}}");
        COUT(+ (doc / "nested" / "arr"), 2); // nested array
        COUT(+ (doc / "nested" / "obj"), 1); // nested object
    }

    DESC("Valid numeric strings");
    {
        yyjson::Document doc;
        doc.read("{\"str_num\": \"42\"}");
        COUT(+ (doc / "str_num"), 42);

        doc.read("{\"str_negative\": \"-15\"}");
        COUT(+ (doc / "str_negative"), -15);

        doc.read("{\"str_zero\": \"0\"}");
        COUT(+ (doc / "str_zero"), 0);
    }

    DESC("Invalid numeric strings");
    {
        yyjson::Document doc;
        doc.read("{\"str_invalid\": \"abc\"}");
        COUT(+ (doc / "str_invalid"), 0); // should return 0 for parsing errors

        doc.read("{\"str_empty\": \"\"}");
        COUT(+ (doc / "str_empty"), 0); // empty string

        doc.read("{\"str_whitespace\": \"  123  \"}");
        COUT(+ (doc / "str_whitespace"), 123); // should handle whitespace
    }

    DESC("Edge cases with floating point strings");
    {
        yyjson::Document doc;
        doc.read("{\"str_float\": \"3.14\"}");
        COUT(+ (doc / "str_float"), 3); // should truncate decimal part
        COUT((doc / "str_float").toNumber(), 0.0);

        doc.read("{\"str_large\": \"999999999\"}");
        COUT(+ (doc / "str_large"), 999999999); // large number
    }
}

DEF_TAST(conversion_document, "test Document and MutableDocument conversion methods")
{
    DESC("Test Document::mutate() method");
    {
        yyjson::Document doc("{\"name\":\"Alice\",\"age\":30}");
        COUT(doc.isValid(), true);

        // Convert to mutable document
        yyjson::MutableDocument mut_doc = doc.mutate();
        COUT(mut_doc.isValid(), true);

        // Verify content is the same
        COUT(mut_doc / "name" | std::string(), "Alice");
        COUT(mut_doc / "age" | 0, 30);

        // Modify the mutable document
        mut_doc.root()["age"] = 31;
        COUT(mut_doc / "age" | 0, 31);
        mut_doc["age"] = 32;
        COUT(mut_doc / "age" | 0, 32);
        COUT(mut_doc["age"] | 0, 32);

        // Original document should remain unchanged
        COUT(doc / "age" | 0, 30);
    }

    DESC("Test MutableDocument::freeze() method");
    {
        yyjson::MutableDocument mut_doc("{\"city\":\"Beijing\",\"country\":\"China\"}");
        COUT(mut_doc.isValid(), true);

        // Convert to read-only document
        yyjson::Document doc = mut_doc.freeze();
        COUT(doc.isValid(), true);

        // Verify content is the same
        COUT(doc / "city" | std::string(), "Beijing");
        COUT(doc / "country" | std::string(), "China");

        // Try to modify the mutable document (should work)
        mut_doc.root()["city"] = "Shanghai";
        COUT(mut_doc / "city" | std::string(), "Shanghai");

        // Frozen document should remain unchanged
        COUT(doc / "city" | std::string(), "Beijing");
        COUT(doc["city"] | std::string(), "Beijing");
    }

    DESC("Test unary minus operator for conversion");
    {
        yyjson::Document doc("{\"name\":\"Bob\",\"score\":95}");
        COUT(doc.isValid(), true);

        // Use operator~ to convert to mutable
        yyjson::MutableDocument mut_doc = ~doc;
        COUT(mut_doc.isValid(), true);
        COUT(mut_doc / "name" | std::string(), "Bob");
        COUT(mut_doc / "score" | 0, 95);

        // Modify the mutable document
        mut_doc.root()["score"] = 100;
        COUT(mut_doc / "score" | 0, 100);

        // Use operator~ to convert back to read-only
        yyjson::Document frozen_doc = ~mut_doc;
        COUT(frozen_doc.isValid(), true);
        COUT(frozen_doc / "name" | std::string(), "Bob");
        COUT(frozen_doc / "score" | 0, 100);

        // Original document should remain unchanged
        COUT(doc / "score" | 0, 95);
    }

    DESC("Test conversion between empty documents");
    {
        // Test empty Document
        yyjson::Document empty_doc;
        COUT(empty_doc.isValid(), false);

        yyjson::MutableDocument mut_empty = empty_doc.mutate();
        COUT(mut_empty.isValid(), false);

        // Test empty MutableDocument - default constructor creates valid empty object
        yyjson::MutableDocument mut_empty2;
        COUT(mut_empty2.isValid(), true);

        yyjson::Document frozen_empty = mut_empty2.freeze();
        COUT(frozen_empty.isValid(), true);

        // Test truly empty MutableDocument using explicit null pointer
        yyjson::MutableDocument mut_empty3((yyjson_mut_doc*)nullptr);
        COUT(mut_empty3.isValid(), false);

        yyjson::Document frozen_empty3 = mut_empty3.freeze();
        COUT(frozen_empty3.isValid(), false);
    }

    DESC("Test complex document conversion");
    {
        // Create a complex document
        yyjson::Document original("{\"users\":[{\"name\":\"Alice\",\"age\":25},{\"name\":\"Bob\",\"age\":30}],\"metadata\":{\"version\":1,\"timestamp\":\"2025-01-01\"}}");
        COUT(original.isValid(), true);

        // Convert to mutable
        yyjson::MutableDocument mut_doc = original.mutate();
        COUT(mut_doc.isValid(), true);

        // Verify complex structure
        COUT(mut_doc / "users" / 0 / "name" | std::string(), "Alice");
        COUT(mut_doc / "users" / 1 / "age" | 0, 30);
        COUT(mut_doc / "metadata" / "version" | 0, 1);

        // Modify the structure
        mut_doc.root()["users"][0]["age"] = 26;
        mut_doc.root()["metadata"]["version"] = 2;

        // Convert back to read-only
        yyjson::Document final_doc = mut_doc.freeze();
        COUT(final_doc.isValid(), true);

        // Verify modifications
        COUT(final_doc / "users" / 0 / "age" | 0, 26);
        COUT(final_doc / "metadata" / "version" | 0, 2);

        // Original should remain unchanged
        COUT(original / "users" / 0 / "age" | 0, 25);
        COUT(original / "metadata" / "version" | 0, 1);
    }
}

DEF_TAST(conversion_doc_unary, "test Document and MutableDocument unary operators")
{
    DESC("Test Document unary operators with object root");
    {
        std::string jsonText = R"json({
            "string_value": "hello",
            "int_value": 42,
            "array_value": [1, 2, 3],
            "object_value": {"key": "value"}
        })json";

        yyjson::Document doc(jsonText);
        
        // Test unary operator* (get root)
        auto root = *doc;
        COUT(root.isValid(), true);
        COUT(root.isObject(), true);
        
        // Test unary operator+ (convert root to number)
        // For object/array, should return size()
        COUT(+doc, 4); // object with 4 keys
        
        // Test unary operator- (convert root to string)
        std::string docString = -doc;
        COUT(docString.find("string_value") != std::string::npos, true);
        COUT(docString.find("hello") != std::string::npos, true);
    }

    DESC("Test Document unary operators with numeric root");
    {
        yyjson::Document doc("123");
        
        // Test unary operator* (get root)
        auto root = *doc;
        COUT(root.isValid(), true);
        COUT(root.isNumber(), true);
        
        // Test unary operator+ (convert root to number)
        COUT(+doc, 123);
        
        // Test unary operator- (convert root to string)
        COUT(-doc, "123");
    }

    DESC("Test Document unary operators with array root");
    {
        yyjson::Document doc("[1, 2, 3, 4, 5]");
        
        // Test unary operator* (get root)
        auto root = *doc;
        COUT(root.isValid(), true);
        COUT(root.isArray(), true);
        
        // Test unary operator+ (convert root to number)
        COUT(+doc, 5); // array with 5 elements
        
        // Test unary operator- (convert root to string)
        std::string docString = -doc;
        COUT(docString.find("1") != std::string::npos, true);
        COUT(docString.find("5") != std::string::npos, true);
    }

    DESC("Test MutableDocument unary operators");
    {
        yyjson::MutableDocument doc("{\"name\": \"Alice\", \"age\": 30}");
        
        // Test unary operator* (get root)
        auto root = *doc;
        COUT(root.isValid(), true);
        COUT(root.isObject(), true);
        
        // Test unary operator+ (convert root to number)
        COUT(+doc, 2); // object with 2 keys
        
        // Test unary operator- (convert root to string)
        std::string docString = -doc;
        COUT(docString.find("Alice") != std::string::npos, true);
        COUT(docString.find("30") != std::string::npos, true);
    }

    DESC("Test MutableDocument unary operators with array root");
    {
        yyjson::MutableDocument doc("[10, 20, 30]");
        
        // Test unary operator* (get root)
        auto root = *doc;
        COUT(root.isValid(), true);
        COUT(root.isArray(), true);
        
        // Test unary operator+ (convert root to number)
        COUT(+doc, 3); // array with 3 elements
        
        // Test unary operator- (convert root to string)
        std::string docString = -doc;
        COUT(docString.find("10") != std::string::npos, true);
        COUT(docString.find("30") != std::string::npos, true);
        
        // Test chaining with stream operator
        *doc << 40; // append to array via root access
        COUT(+doc, 4); // now array has 4 elements
    }

    DESC("Test consistency between Document and Value operators");
    {
        yyjson::Document doc("{\"value\": 123}");
        
        // Document operator+ should be equivalent to doc.root().toNumber()
        COUT(+doc, +(*doc));
        
        // Document operator- should be equivalent to doc.root().toString()
        COUT(-doc, -(*doc));
        
        // Document operator* should return the root value
        COUT(*doc, doc.root());
    }

    DESC("Test invalid Document unary operators");
    {
        // Test with invalid Document
        yyjson::Document invalid_doc;
        COUT(invalid_doc.isValid(), false);
        
        // Invalid document should return default values
        COUT(+invalid_doc, 0);
        COUT(-invalid_doc, "");
        
        // operator* should return invalid value
        auto root = *invalid_doc;
        COUT(root.isValid(), false);
    }

    DESC("Test invalid MutableDocument unary operators");
    {
        // Test with invalid MutableDocument
        yyjson::MutableDocument invalid_doc((yyjson_mut_doc*)nullptr);
        COUT(invalid_doc.isValid(), false);
        
        // Invalid document should return default values
        COUT(+invalid_doc, 0);
        COUT(-invalid_doc, "");
        
        // operator* should return invalid value
        auto root = *invalid_doc;
        COUT(root.isValid(), false);
    }
}

DEF_TAST(conversion_user_literals, "test user-defined literals for direct Document creation")
{
    using namespace yyjson;

    DESC("Test basic JSON object creation with user-defined literal");
    {
        // Basic object
        auto doc = "{\"name\": \"Alice\", \"age\": 25}"_xyjson;
        COUT(doc.isValid(), true);
        COUT(doc / "name" | std::string(), "Alice");
        COUT(doc / "age" | 0, 25);
    }

    DESC("Test JSON array creation with user-defined literal");
    {
        // Array
        auto doc = "[1, 2, 3, 4, 5]"_xyjson;
        COUT(doc.isValid(), true);
        COUT(doc.root().size(), 5);
        COUT(doc / 0 | 0, 1);
        COUT(doc / 4 | 0, 5);
    }

    DESC("Test nested JSON structures with user-defined literal");
    {
        // Nested object with array
        auto doc = "{\"users\": [{\"name\": \"Bob\", \"scores\": [90, 85, 95]}, {\"name\": \"Charlie\", \"scores\": [88, 92]}], \"meta\": {\"version\": 1}}"_xyjson;
        COUT(doc.isValid(), true);
        COUT(doc / "users" / 0 / "name" | std::string(), "Bob");
        COUT(doc / "users" / 0 / "scores" / 2 | 0, 95);
        COUT(doc / "users" / 1 / "name" | std::string(), "Charlie");
        COUT(doc / "meta" / "version" | 0, 1);
    }

    DESC("Test scalar values with user-defined literal");
    {
        // String
        auto str_doc = "\"Hello, World!\""_xyjson;
        COUT(str_doc.isValid(), true);
        COUT(*str_doc | std::string(), "Hello, World!");

        // Number
        auto num_doc = "42"_xyjson;
        COUT(num_doc.isValid(), true);
        COUT(*num_doc | 0, 42);

        // Boolean
        auto bool_doc = "true"_xyjson;
        COUT(bool_doc.isValid(), true);
        COUT(*bool_doc | false, true);

        // Null
        auto null_doc = "null"_xyjson;
        COUT(null_doc.isValid(), true);
        COUT(null_doc.root().isNull(), true);
    }

    DESC("Test empty and edge cases with user-defined literal");
    {
        // Empty object
        auto empty_obj = "{}"_xyjson;
        COUT(empty_obj.isValid(), true);
        COUT(empty_obj.root().isObject(), true);
        COUT(empty_obj.root().size(), 0);

        // Empty array
        auto empty_arr = "[]"_xyjson;
        COUT(empty_arr.isValid(), true);
        COUT(empty_arr.root().isArray(), true);
        COUT(empty_arr.root().size(), 0);

        // Invalid JSON (should handle gracefully)
        auto invalid_doc = "{invalid json}"_xyjson;
        COUT(invalid_doc.isValid(), false);
        COUT(invalid_doc.hasError(), true);
    }

    DESC("Test combining with other operations");
    {
        auto doc = "{\"data\": {\"values\": [10, 20, 30]}}"_xyjson;
        
        // Use with path operator
        COUT(doc / "data" / "values" / 1 | 0, 20);
        
        // Use with unary operators
        COUT(+ (doc / "data" / "values"), 3);
        
        // Use with stream output
        std::ostringstream oss;
        oss << doc;
        COUT(oss.str().find("values") != std::string::npos, true);
    }

    DESC("Test namespace qualification");
    {
        // Qualified usage
        auto doc1 = yyjson::operator""_xyjson("{\"test\": \"value\"}", 17);
        COUT(doc1.isValid(), true);
        COUT(doc1 / "test" | std::string(), "value");

        // Compare with unqualified usage
        auto doc2 = "{\"test\": \"value\"}"_xyjson;
        COUT(doc2.isValid(), true);
        COUT(doc2 / "test" | std::string(), "value");
    }
}

DEF_TAST(conversion_explicit, "test explicit type conversion functions")
{
    using namespace yyjson;

    // Test existing unary operators
    Document doc("{\"intVal\":42, \"strVal\":\"hello\", \"floatVal\":3.14}");
    COUT(doc.hasError(), false);

    Value intVal = doc["intVal"];
    Value strVal = doc["strVal"];
    Value floatVal = doc["floatVal"];

    // Test current unary + operator (converts to int)
    COUT(+intVal, 42);
    COUT(+floatVal, 3);  // float should be truncated to int

    // Test current unary - operator (converts to string)
    COUT(-strVal, std::string("hello"));
    COUT(-intVal, std::string("42"));    // int should convert to string "42"

    // Test new explicit type conversion functions for Value
    COUT(int(intVal), 42);
    COUT((int)intVal, 42);
    COUT(double(floatVal), 3.14);
    COUT((double)floatVal, 3.14);
    COUT(std::string(strVal), std::string("hello"));
    COUT((std::string)strVal, std::string("hello"));

    // Test explicit conversion for numeric types
    COUT(int(floatVal), 3);              // float truncated to int
    COUT(double(intVal), 42.0);           // int to double
    COUT(std::string(intVal), std::string("42")); // int to string

//! compile error: refuse implicit conversion to int
//  COUT(intVal + 1, 43);
//  COUT(1 + intVal, 43);

    // Test Document and MutableDocument explicit conversions
    MutableDocument mutDoc = ~doc;  // Use ~ operator for mutable conversion
    COUT(mutDoc.hasError(), false);

    // Test explicit conversions between Document and MutableDocument
    Document doc2(mutDoc);           // Document from MutableDocument
    COUT(doc2.hasError(), false);
    COUT(doc2["intVal"] | 0, 42);

    MutableDocument mutDoc2(doc);    // MutableDocument from Document
    COUT(mutDoc2.hasError(), false);
    COUT(mutDoc2["intVal"] | 0, 42);

    // Test C-style cast syntax
    Document doc3 = (Document)mutDoc;
    COUT(doc3.hasError(), false);
    
    MutableDocument mutDoc3 = (MutableDocument)doc;
    COUT(mutDoc3.hasError(), false);
}
