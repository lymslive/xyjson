/**
 * @file t_advanced.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief advanced functionality test for xyjson
 * */
#include "couttast/couttast.h"
#include "xyjson.h"
#include <algorithm>

/* t_advanced.cpp - 高级功能测试
 * 包含：
 * - 管道功能
 * - 类型特征
 */

// Example functions for pipe functionality testing
namespace {

// Function to convert value to uppercase string
std::string toUppercase(const yyjson::Value& value) {
    std::string str = value | "";
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// Function to double numeric values
int doubleValue(const yyjson::Value& value) {
    return (value | 0) * 2;
}

// Function to check if value is truthy
bool isTruthy(const yyjson::Value& value) {
    if (value.isBool()) {
        return value | false;
    }
    if (value.isNumber()) {
        return (value | 0) != 0;
    }
    if (value.isString()) {
        std::string str = value | "";
        return !str.empty() && str != "false" && str != "0";
    }
    return false;
}

// Function to format value with prefix
std::string formatWithPrefix(const yyjson::Value& value, const std::string& prefix) {
    // Using a lambda with capture to demonstrate more complex scenarios
    auto formatter = [&prefix](const yyjson::Value& val) {
        return prefix + (val | std::string());
    };
    return formatter(value);
}

} // anonymous namespace

DEF_TAST(advanced_pipe, "test pipe() method and | operator with functions")
{
    std::string jsonText = R"json({
        "string_value": "hello",
        "int_value": 42,
        "bool_true": true,
        "bool_false": false,
        "zero_value": 0,
        "empty_string": ""
    })json";

    DESC("Test pipe() method with Value");
    {
        yyjson::Document doc(jsonText);
        COUT(doc.hasError(), false);

        // Test pipe with toUppercase function
        std::string upperResult = (doc / "string_value").pipe(toUppercase);
        COUT(upperResult, "HELLO");

        // Test pipe with doubleValue function
        int doubledResult = (doc / "int_value").pipe(doubleValue);
        COUT(doubledResult, 84);

        // Test pipe with isTruthy function
        COUT((doc / "bool_true").pipe(isTruthy), true);
        COUT((doc / "bool_false").pipe(isTruthy), false);
        COUT((doc / "zero_value").pipe(isTruthy), false);
        COUT((doc / "empty_string").pipe(isTruthy), false);
    }

    DESC("Test | operator with function parameters");
    {
        yyjson::Document doc(jsonText);
        
        // Test | operator with function (should call pipe)
        std::string upperResult = doc / "string_value" | toUppercase;
        COUT(upperResult, "HELLO");

        int doubledResult = doc / "int_value" | doubleValue;
        COUT(doubledResult, 84);

        // Test chaining with other operations
        int complexResult = (doc / "int_value" | doubleValue) + 10;
        COUT(complexResult, 94);

        std::string complexString = "Result: " + (doc / "string_value" | toUppercase);
        COUT(complexString, "Result: HELLO");
    }

    DESC("Test pipe() method with MutableValue");
    {
        yyjson::MutableDocument doc(jsonText);
        COUT(doc.hasError(), false);

        // Test pipe with mutable values using lambda that accepts MutableValue
        std::string upperResult = (doc / "string_value").pipe([](const yyjson::MutableValue& value) {
            std::string str = value | "";
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            return str;
        });
        COUT(upperResult, "HELLO");

        // Modify value and test pipe again
        (doc / "string_value") = "world";
        upperResult = (doc / "string_value").pipe([](const yyjson::MutableValue& value) {
            std::string str = value | "";
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            return str;
        });
        COUT(upperResult, "WORLD");
    }

    DESC("Test lambda functions with pipe");
    {
        yyjson::Document doc(jsonText);
        
        // Test with inline lambda
        std::string result1 = (doc / "string_value").pipe([](const yyjson::Value& val) {
            return "prefix_" + (val | std::string());
        });
        COUT(result1, "prefix_hello");

        // Test with lambda that captures variables
        std::string customPrefix = "custom_";
        std::string result2 = (doc / "string_value").pipe([&customPrefix](const yyjson::Value& val) {
            return customPrefix + (val | std::string());
        });
        COUT(result2, "custom_hello");

        // Test numeric transformation
        int result3 = (doc / "int_value").pipe([](const yyjson::Value& val) {
            return (val | 0) * 3;
        });
        COUT(result3, 126);
    }

    DESC("Test error handling with pipe");
    {
        yyjson::Document doc(jsonText);
        
        // Test with invalid path
        auto invalidValue = doc / "nonexistent";
        
        std::string result1 = invalidValue.pipe(toUppercase);
        COUT(result1, ""); // should return empty string
        
        int result2 = invalidValue.pipe(doubleValue);
        COUT(result2, 0); // should return 0
        
        // Test with explicit function for default handling
        std::string result3 = invalidValue.pipe([](const yyjson::Value& val) {
            return val | "default";
        });
        COUT(result3, "default");
    }

    DESC("Test function composition with pipe");
    {
        yyjson::Document doc(jsonText);
        
        // Create a function that composes multiple transformations
        auto complexTransform = [](const yyjson::Value& val) {
            std::string str = val | "";
            if (!str.empty()) {
                std::transform(str.begin(), str.end(), str.begin(), ::toupper);
                return "TRANSFORMED_" + str + "_END";
            }
            return std::string("DEFAULT");
        };
        
        std::string result = (doc / "string_value").pipe(complexTransform);
        COUT(result, "TRANSFORMED_HELLO_END");
        
        // Test with invalid value
        result = (doc / "nonexistent").pipe(complexTransform);
        COUT(result, "DEFAULT");
    }
}

DEF_TAST(advanced_trait, "test type traits for yyjson wrapper classes")
{
    using namespace yyjson;

    // Test is_value trait
    COUT(trait::is_value<Value>::value, true);
    COUT(trait::is_value<MutableValue>::value, true);
    COUT(trait::is_value<Document>::value, false);
    COUT(trait::is_value<MutableDocument>::value, false);
    COUT(trait::is_value<int>::value, false);
    COUT(trait::is_value<std::string>::value, false);

    // Test is_document trait
    COUT(trait::is_document<Document>::value, true);
    COUT(trait::is_document<MutableDocument>::value, true);
    COUT(trait::is_document<Value>::value, false);
    COUT(trait::is_document<MutableValue>::value, false);
    COUT(trait::is_document<void*>::value, false);

    // Test is_iterator trait
    COUT(trait::is_iterator<ArrayIterator>::value, true);
    COUT(trait::is_iterator<ObjectIterator>::value, true);
    COUT(trait::is_iterator<MutableArrayIterator>::value, true);
    COUT(trait::is_iterator<MutableObjectIterator>::value, true);
    COUT(trait::is_iterator<Value>::value, false);
    COUT(trait::is_iterator<Document>::value, false);

    // Example usage with function templates
    {
        auto check_value_type = [](const auto& obj) {
            return trait::is_value<std::decay_t<decltype(obj)>>::value;
        };

        Value val;
        Document doc;
        
        COUT(check_value_type(val), true);
        COUT(check_value_type(doc), false);
    }

    // Test with conditional compilation
    {
        Value val;
        
        if constexpr (trait::is_value<Value>::value) {
            COUT(val.isValid(), false); // null value should be invalid
        } else {
            COUT(false, true); // This branch should not be taken
        }
    }

    // Test is_key_type function
    {
        // Test supported key types (should return true)
        COUT(trait::is_key_type<const char*>(), true);
        COUT(trait::is_key_type<char*>(), true);
        COUT(trait::is_key_type<std::string>(), true);
        
        // Test string literals (array types)
        COUT(trait::is_key_type<const char[5]>(), true);  // e.g., "test"
        COUT(trait::is_key_type<char[5]>(), true);        // e.g., char arr[5] = "test"
        
        // Test unsupported types (should return false)
        COUT(trait::is_key_type<int>(), false);
        COUT(trait::is_key_type<double>(), false);
        COUT(trait::is_key_type<bool>(), false);
        COUT(trait::is_key_type<Value>(), false);
        COUT(trait::is_key_type<MutableValue>(), false);
        COUT(trait::is_key_type<Document>(), false);
        COUT(trait::is_key_type<void*>(), false);
        
        // Test with decayed types (should handle array-to-pointer decay correctly)
        const char* ptr = "test";
        COUT(trait::is_key_type<decltype(ptr)>(), true);
        
        std::string str = "test";
        COUT(trait::is_key_type<decltype(str)>(), true);
        
        // Test with function that uses is_key_type
        auto test_key_function = [](const auto& key) {
            if constexpr (trait::is_key_type<decltype(key)>()) {
                return std::string("valid_key");
            } else {
                return std::string("invalid_key");
            }
        };
        
        COUT(test_key_function("literal"), "valid_key");
        COUT(test_key_function(std::string("string")), "valid_key");
        COUT(test_key_function(ptr), "valid_key");
        COUT(test_key_function(42), "invalid_key");
        COUT(test_key_function(true), "invalid_key");
    }
}

DEF_TAST(advanced_compare_ops, "test comparison operators")
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

    DESC("Test less-than and other comparison operators");
    {
        // comapre numbers, parsed root is number type
        Document val1("123");
        Document val2("45");
        COUT(val1.root() < val2.root(), false);
        COUT(val1.root() > val2.root(), true);
        COUT(val1.root() <= val2.root(), false);
        COUT(val1.root() >= val2.root(), true);

        // compae strings
        Document str1("\"abc\"");
        Document str2("\"abd\"");
        COUT(str1.root() < str2.root(), true);
        COUT(str1.root() > str2.root(), false);

        Document arr1("[1, 2]");
        Document arr2("[1, 2, 3]");
        COUT(arr1.root() < arr2.root(), true);
    }

    // Test JSON value and scalar type direct comparison
    {
        Document doc("{\"int\": 42, \"double\": 3.14, \"string\": \"hello\", \"bool\": true}");
        
        // Test integer comparison
        COUT((doc / "int") == 42, true);
        COUT((doc / "int") == 43, false);
        COUT((doc / "int") != 42, false);
        COUT((doc / "int") != 43, true);
        COUT(42 == (doc / "int"), true);
        COUT(43 == (doc / "int"), false);
        
        // Test double comparison
        COUT((doc / "double") == 3.14, true);
        COUT((doc / "double") == 2.71, false);
        COUT(3.14 == (doc / "double"), true);
        COUT(2.71 == (doc / "double"), false);
        
        // Test string comparison
        COUT((doc / "string") == "hello", true);
        COUT((doc / "string") == "world", false);
        COUT("hello" == (doc / "string"), true);
        COUT("world" == (doc / "string"), false);
        
        // Test std::string comparison
        std::string hello_str = "hello";
        std::string world_str = "world";
        COUT((doc / "string") == hello_str, true);
        COUT((doc / "string") == world_str, false);
        COUT(hello_str == (doc / "string"), true);
        COUT(world_str == (doc / "string"), false);
        
        // Test boolean comparison
        COUT((doc / "bool") == true, true);
        COUT((doc / "bool") == false, false);
        COUT(true == (doc / "bool"), true);
        COUT(false == (doc / "bool"), false);
        
        // Test with MutableValue
        MutableDocument mutDoc("{\"number\": 100}");
        COUT((mutDoc / "number") == 100, true);
        COUT(100 == (mutDoc / "number"), true);
        COUT((mutDoc / "number") != 50, true);
        
        // Test with invalid values
        Value nullValue;
        COUT(nullValue == 42, false);
        COUT(nullValue == "hello", false);
        COUT(42 == nullValue, false);
        COUT("hello" == nullValue, false);
        
        // Test with type mismatches
        COUT((doc / "int") == "hello", false);
        COUT((doc / "string") == 42, false);
        COUT((doc / "bool") == 42, false);
        
        // Test with 64-bit integers
        // yyjson may store positive numbers as uint64_t
        COUT((doc / "int") == uint64_t(42), true);
        COUT(uint64_t(42) == (doc / "int"), true);
        
        // Test negative numbers which should be stored as int64_t
        Document negDoc("{\"negative\": -42}");
        COUT((negDoc / "negative") == int64_t(-42), true);
        COUT(int64_t(-42) == (negDoc / "negative"), true);
    }
}

DEF_TAST(advanced_sort_mixed_array, "sort a mixed array of json values")
{
    using namespace yyjson;

    const char* json_str = R"json(
        [null, true, false, 1, 0, -1, 1.1, 0.0, -1.1, "abc", "abd", "", {}, {"a":1}, []]
    )json";

    Document doc(json_str);
    COUT(doc.hasError(), false);

    std::vector<Value> values;
    for (auto it = doc.root().beginArray(); it.isValid(); it.next())
    {
        values.push_back(it->value);
    }

    std::sort(values.begin(), values.end());

    MutableDocument sorted_doc;
    auto sorted_arr = sorted_doc.create(yyjson::kArray);
    for (const auto& val : values)
    {
        sorted_arr.append(val);
    }

    std::string expected_str = "[null,false,true,-1.1,-1,0,0.0,1,1.1,\"\",\"abc\",\"abd\",[],{},{\"a\":1}]";
    std::string actual_str = sorted_arr.toString();

    // Normalize the output for consistent comparison
    // remove spaces
    auto normalize = [](std::string& s) {
        s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    };

    normalize(expected_str);
    normalize(actual_str);
    
    COUT(actual_str, expected_str);
}

DEF_TAST(advanced_string_optimization, "test string creation optimization strategies")
{
    using namespace yyjson;
    
    DESC("Test automatic string literal optimization in create function");
    
    MutableDocument doc;
    
    // Test 1: String literal should be optimized (reference)
    {
        auto literal_node = yyjson::util::create(doc.get(), "string_literal");
        MutableValue literal_val(literal_node, doc.get());
        
        const char* result = literal_val | "";
        COUT(result == "string_literal", true); // Should be same pointer
        COUT(result, "string_literal"); // Value check
    }
    
    // Test 2: const char* should be copied
    {
        const char* ptr = "pointer_string";
        auto ptr_node = yyjson::util::create(doc.get(), ptr);
        MutableValue ptr_val(ptr_node, doc.get());
        
        const char* result = ptr_val | "";
        COUT(result == ptr, false); // Should be different pointer (copied)
        COUT(result, "pointer_string"); // Value check
    }
    
    // Test 3: char* should be copied
    {
        char buffer[] = "char_star_buffer";
        char* ptr = buffer;
        auto ptr_node = yyjson::util::create(doc.get(), ptr);
        MutableValue ptr_val(ptr_node, doc.get());
        
        const char* result = ptr_val | "";
        COUT(result == ptr, false); // Should be different pointer (copied)
        COUT(::strcmp(result, ptr), 0); // Value check
        COUT(ptr_val.typeName(), "string"); // Type check
    }
    
    // Test 4: char[N] array should be copied, not special
    {
        char buffer[] = "char_array_buffer";
        // Directly pass the array (not pointer) - should be copied
        auto arr_node = yyjson::util::create(doc.get(), buffer);
        MutableValue arr_val(arr_node, doc.get());
        
        const char* result = arr_val | "";
        COUT(result == buffer, false); // Should be different pointer (copied)
        COUT(::strcmp(result, buffer), 0); // Value check
        COUT(arr_val.typeName(), "string"); // Type check
    }
    
    // Test 5: char[N] with special content should be string, not special container
    {
        char obj_buffer[] = "{}";
        auto obj_node = yyjson::util::create(doc.get(), obj_buffer);
        MutableValue obj_val(obj_node, doc.get());
        
        COUT(obj_val.typeName(), "string"); // Should be string, not object
        COUT(obj_val.isString(), true); // Should be string
        COUT(obj_val.isObject(), false); // Should not be object
        
        char arr_buffer[] = "[]";
        auto arr_node = yyjson::util::create(doc.get(), arr_buffer);
        MutableValue arr_val(arr_node, doc.get());
        
        COUT(arr_val.typeName(), "string"); // Should be string, not array
        COUT(arr_val.isString(), true); // Should be string
        COUT(arr_val.isArray(), false); // Should not be array
    }
    
    // Test 6: std::string should be copied
    {
        std::string str = "std_string";
        auto str_node = yyjson::util::create(doc.get(), str);
        MutableValue str_val(str_node, doc.get());
        
        const char* result = str_val | "";
        COUT(result == str.c_str(), false); // Should be different pointer (copied)
        COUT(result, "std_string"); // Value check
    }
    
    // Test 7: StringRef should be optimized
    {
        StringRef ref("ref_string");
        auto ref_node = yyjson::util::create(doc.get(), ref);
        MutableValue ref_val(ref_node, doc.get());
        
        const char* result = ref_val | "";
        COUT(result == "ref_string", true); // Should be same pointer
        COUT(result, "ref_string"); // Value check
    }
    
    // Test 8: Special literal "{}" should create empty object
    {
        auto obj_node = yyjson::util::create(doc.get(), "{}");
        MutableValue obj_val(obj_node, doc.get());
        
        COUT(obj_val.typeName(), "object"); // Should be object type
        COUT(obj_val.size(), 0); // Empty object should have size 0
        COUT(obj_val.isObject(), true); // Should be object
        COUT(obj_val.isArray(), false); // Should not be array
    }
    
    // Test 9: Special literal "[]" should create empty array
    {
        auto arr_node = yyjson::util::create(doc.get(), "[]");
        MutableValue arr_val(arr_node, doc.get());
        
        COUT(arr_val.typeName(), "array"); // Should be array type
        COUT(arr_val.size(), 0); // Empty array should have size 0
        COUT(arr_val.isArray(), true); // Should be array
        COUT(arr_val.isObject(), false); // Should not be object
    }
    
    // Test 10: Regular string literals should not be treated as special
    {
        auto reg_node = yyjson::util::create(doc.get(), "{not_special}");
        MutableValue reg_val(reg_node, doc.get());
        
        COUT(reg_val.typeName(), "string"); // Should be string type
        COUT(reg_val.isString(), true); // Should be string
        COUT(reg_val.isObject(), false); // Should not be object
        COUT(reg_val.isArray(), false); // Should not be array
        
        const char* result = reg_val | "";
        COUT(result, "{not_special}"); // Should preserve content
    }
}
