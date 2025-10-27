/**
 * @file t_iterator.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief iterator functionality test for xyjson
 * */
#include "couttast/couttast.h"
#include "xyjson.h"
#include <set>

/**
 * @brief Infinite loop detection macro
 * @param condition Condition to check for infinite loop
 * @param message Error message to display
 * 
 * Usage: INF_LOOP_BREAK(count > 10, "Infinite loop detected")
 */
#define INF_LOOP_BREAK(condition, message) \
    if (condition) { \
        std::cerr << "ERROR: " << message << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        break; \
    }

DEF_TAST(iterator_basic_loop, "basic loop with four iterator")
{
    DESC("readonly array iterator:");
    {
        std::string jsonText = "[1, 2, 3, 4, 5]";

        yyjson::Document doc(jsonText);
        COUT(doc.hasError(), false);

        // Test basic array iterator loop
        auto arrayVal = doc.root();
        COUT(arrayVal.isArray(), true);

        auto iter = arrayVal.arrayIter();
        COUT(iter.isValid(), true);

        int count = 0;
        int expected = 1;
        while (iter.isValid())
        {
            int value = iter.value() | -1;
            COUT(value, expected++);
            count++;
            ++iter;

            INF_LOOP_BREAK(count > 10, "Infinite loop detected in readonly array iterator");
        }
        COUT(count, 5);
    }

    DESC("readonly object iterator:");
    {
        std::string jsonText = "{\"a\": 1, \"b\": 2, \"c\": 3, \"d\": 4, \"e\": 5}";

        yyjson::Document doc(jsonText);
        COUT(doc.hasError(), false);

        // Test basic object iterator loop
        auto objVal = doc.root();
        COUT(objVal.isObject(), true);

        auto iter = objVal.objectIter();
        COUT(iter.isValid(), true);

        int count = 0;
        std::set<std::string> expectedKeys = {"a", "b", "c", "d", "e"};
        std::set<std::string> actualKeys;

        while (iter.isValid()) {
            std::string key = iter.key() | "";
            int value = iter.value() | -1;

            COUT(expectedKeys.find(key) != expectedKeys.end(), true); // Key should be in expected set
            COUT(value, count + 1); // Values should be 1,2,3,4,5

            actualKeys.insert(key);
            count++;
            ++iter;

            INF_LOOP_BREAK(count > 10, "Infinite loop detected in readonly object iterator");
        }

        COUT(count, 5); // Should have exactly 5 key-value pairs
        COUT(actualKeys.size(), 5); // Should have exactly 5 unique keys

        // Verify all expected keys were found
        for (const auto& key : expectedKeys) {
            COUT(actualKeys.find(key) != actualKeys.end(), true);
        }
    }

    DESC("mutalbe array iterator:");
    {
        std::string jsonText = "[1, 2, 3, 4, 5]";

        yyjson::Document doc(jsonText);
        COUT(doc.hasError(), false);

        auto mutableDoc = ~doc; // Convert to mutable document
        auto arrayVal = mutableDoc.root();
        COUT(arrayVal.isArray(), true);

        auto iter = arrayVal.arrayIter();
        COUT(iter.isValid(), true);

        int count = 0;
        int expected = 1;
        while (iter.isValid())
        {
            int value = iter.value() | -1;
            COUT(value, expected++);
            count++;
            ++iter;

            INF_LOOP_BREAK(count > 10, "Infinite loop detected in mutable array iterator");
        }
        COUT(count, 5);
    }

    DESC("mutalbe object iterator:");
    {
        std::string jsonText = "{\"a\": 1, \"b\": 2, \"c\": 3, \"d\": 4, \"e\": 5}";

        yyjson::Document doc(jsonText);
        COUT(doc.hasError(), false);

        // Test basic mutable object iterator loop
        auto mutableDoc = ~doc; // Convert to mutable document
        auto objVal = mutableDoc.root();
        COUT(objVal.isObject(), true);

        auto iter = objVal.objectIter();
        COUT(iter.isValid(), true);

        int count = 0;
        std::set<std::string> expectedKeys = {"a", "b", "c", "d", "e"};
        std::set<std::string> actualKeys;

        while (iter.isValid()) {
            std::string key = iter.key() | "";
            int value = iter.value() | -1;

            COUT(expectedKeys.find(key) != expectedKeys.end(), true); // Key should be in expected set
            COUT(value, count + 1); // Values should be 1,2,3,4,5

            actualKeys.insert(key);
            count++;
            ++iter;

            INF_LOOP_BREAK(count > 10, "Infinite loop detected in mutable object iterator");
        }

        COUT(count, 5); // Should have exactly 5 key-value pairs
        COUT(actualKeys.size(), 5); // Should have exactly 5 unique keys

        // Verify all expected keys were found
        for (const auto& key : expectedKeys) {
            COUT(actualKeys.find(key) != actualKeys.end(), true);
        }
    }
}

DEF_TAST(iterator_copy_assignment, "test iterator copying and assignment")
{
    std::string jsonText = "[1, 2, 3]";
    yyjson::Document doc(jsonText);
    
    auto iter1 = doc.root().arrayIter();
    COUT(iter1->toInteger(), 1);
    
    // Test copy constructor
    auto iter2 = iter1;
    COUT(iter2->toInteger(), 1);
    
    // Test assignment
    auto iter3 = doc.root().arrayIter();
    ++iter3; // Move to second element
    iter1 = iter3;
    COUT(iter1->toInteger(), 2);
    COUT(iter2->toInteger(), 1); // Should not affect iter2
}

DEF_TAST(iterator_operators, "test iterator operators and methods")
{
    DESC("readonly array iterator:");
    {
        std::string jsonText = "[10, 20, 30]";
        yyjson::Document doc(jsonText);

        // Test dereference operator
        auto iter = doc.root().arrayIter();
        COUT((*iter).toInteger(), 10);

        // Test arrow operator
        COUT(iter->toInteger(), 10);

        // Test prefix increment
        ++iter;
        COUT(iter->toInteger(), 20);

        // Test postfix increment
        auto old = iter++;
        COUT(old->toInteger(), 20);
        COUT(iter->toInteger(), 30);

        // Test equality
        auto iter2 = doc.root().arrayIter();
        ++iter2;
        COUT(iter.equal(iter2), false);
        COUT(iter2.equal(iter), false);

        auto iter3 = doc.root().arrayIter();
        ++iter3; ++iter3;
        COUT(iter.equal(iter3), true);
        COUT(iter3.equal(iter), true);
    }

    DESC("readonly object iterator:");
    {
        std::string jsonText = "{\"name\": \"test\", \"value\": 42.5}";
        yyjson::Document doc(jsonText);

        auto iter = doc.root().objectIter();

        // Test name() method
        COUT(std::string(iter.name() ? iter.name() : ""), "name");
        COUT(iter->toString(), "test");

        ++iter;
        COUT(std::string(iter.name() ? iter.name() : ""), "value");
        COUT(iter->toNumber(), 42.5);
    }
}

DEF_TAST(iterator_edge_cases, "test iterator edge cases")
{
    DESC("single element:");
    {
        // Test single element array
        std::string single = "[999]";
        yyjson::Document doc1(single);

        auto iter1 = doc1.root().arrayIter();
        COUT(iter1.isValid(), true);
        COUT(iter1.value().toInteger(), 999);
        ++iter1;
        COUT(iter1.isValid(), false);

        // Test single element object
        std::string singleObj = "{\"only\": true}";
        yyjson::Document doc2(singleObj);

        auto iter2 = doc2.root().objectIter();
        COUT(iter2.isValid(), true);
        COUT(std::string(iter2.name() ? iter2.name() : ""), "only");
        COUT(*iter2 | false, true);
        ++iter2;
        COUT(iter2.isValid(), false);
    }
    
    DESC("empty container:");
    {
        // Test empty array
        std::string emptyArray = "[]";
        yyjson::Document doc1(emptyArray);

        auto iter1 = doc1.root().arrayIter();
        COUT(iter1.isValid(), false);

        int count1 = 0;
        while (iter1.isValid()) {
            count1++;
            ++iter1;
            INF_LOOP_BREAK(count1 > 5, "Should not iterate empty array");
        }
        COUT(count1, 0);

        // Test empty object
        std::string emptyObject = "{}";
        yyjson::Document doc2(emptyObject);

        auto iter2 = doc2.root().objectIter();
        COUT(iter2.isValid(), false);

        int count2 = 0;
        while (iter2.isValid()) {
            count2++;
            ++iter2;
            INF_LOOP_BREAK(count2 > 5, "Should not iterate empty object");
        }
        COUT(count2, 0);
    }
}

#if 1
DEF_TAST(iterator_begin_end, "test begin/end pattern")
{
    std::string jsonText = "[\"first\", \"second\", \"third\"]";
    yyjson::Document doc(jsonText);

    // Test beginArray/endArray pattern
    std::vector<std::string> values;
    for (auto it = doc.root().beginArray(); it != doc.root().endArray(); ++it) {
        values.push_back(it.value() | "");
        INF_LOOP_BREAK(values.size() > 10, "Infinite loop in begin/end pattern!");
    }
    COUT(values.size(), 3);
    COUT(values[0], "first");
    COUT(values[1], "second");
    COUT(values[2], "third");

    // Test object begin/end pattern
    std::string objText = "{\"x\": 100, \"y\": 200}";
    yyjson::Document objDoc(objText);

    std::set<std::string> keys;
    int count = 0;
    for (auto it = objDoc.root().beginObject(); it != objDoc.root().endObject(); ++it, ++count) {
        keys.insert(it.key() | "");
        INF_LOOP_BREAK(count > 5, "Infinite loop in object begin/end pattern!");
    }
    COUT(keys.size(), 2);
    COUT(keys.find("x") != keys.end(), true);
    COUT(keys.find("y") != keys.end(), true);
}
#endif

