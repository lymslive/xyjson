/**
 * @file t_advanced.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief advanced functionality test for xyjson
 * */
#include "couttast/couttast.h"
#include "xyjson.h"

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
    COUT(is_value<Value>::value, true);
    COUT(is_value<MutableValue>::value, true);
    COUT(is_value<Document>::value, false);
    COUT(is_value<MutableDocument>::value, false);
    COUT(is_value<int>::value, false);
    COUT(is_value<std::string>::value, false);

    // Test is_document trait
    COUT(is_document<Document>::value, true);
    COUT(is_document<MutableDocument>::value, true);
    COUT(is_document<Value>::value, false);
    COUT(is_document<MutableValue>::value, false);
    COUT(is_document<void*>::value, false);

    // Test is_iterator trait
    COUT(is_iterator<ArrayIterator>::value, true);
    COUT(is_iterator<ObjectIterator>::value, true);
    COUT(is_iterator<MutableArrayIterator>::value, true);
    COUT(is_iterator<MutableObjectIterator>::value, true);
    COUT(is_iterator<Value>::value, false);
    COUT(is_iterator<Document>::value, false);

#ifdef __cpp_variable_templates
    // Test C++17 variable templates
    COUT(is_value_v<Value>, true);
    COUT(is_document_v<Document>, true);
    COUT(is_iterator_v<ArrayIterator>, true);
#endif

    // Test enable_if helper aliases (compile-time only)
    {
        // These should compile - testing that the alias types are valid
        using value_type = enable_if_value_t<Value>;
        using doc_type = enable_if_document_t<Document>; 
        using iter_type = enable_if_iterator_t<ArrayIterator>;
        
        // Just verify the types are what we expect
        static_assert(std::is_same<value_type, Value>::value, "enable_if_value_t should yield Value");
        static_assert(std::is_same<doc_type, Document>::value, "enable_if_document_t should yield Document");
        static_assert(std::is_same<iter_type, ArrayIterator>::value, "enable_if_iterator_t should yield ArrayIterator");
    }

    // Example usage with function templates
    {
        auto check_value_type = [](const auto& obj) {
            return is_value<std::decay_t<decltype(obj)>>::value;
        };

        Value val;
        Document doc;
        
        COUT(check_value_type(val), true);
        COUT(check_value_type(doc), false);
    }

    // Test with conditional compilation
    {
        Value val;
        
        if constexpr (is_value<Value>::value) {
            COUT(val.isValid(), false); // null value should be invalid
        } else {
            COUT(false, true); // This branch should not be taken
        }
    }
}
