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
        DESC("ERROR: %s at %s:%d", message, __FILE__, __LINE__); \
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

DEF_TAST(iterator_copy_ctor, "iterator basic constructor, copying and assignment")
{
    DESC("default constructed iterators");
    {
        yyjson::ArrayIterator arr_it_def;
        const yyjson_arr_iter* c_arr_it = arr_it_def.c_iter();
        COUT(c_arr_it->idx, (size_t)0);
        COUT(c_arr_it->max, (size_t)0);
        COUT(c_arr_it->cur, nullptr);

        yyjson::ObjectIterator obj_it_def;
        const yyjson_obj_iter* c_obj_it = obj_it_def.c_iter();
        COUT(c_obj_it->idx, (size_t)0);
        COUT(c_obj_it->max, (size_t)0);
        COUT(c_obj_it->cur, nullptr);
        COUT(c_obj_it->obj, nullptr);

        yyjson::MutableArrayIterator marr_it_def;
        const yyjson_mut_arr_iter* c_marr_it = marr_it_def.c_iter();
        COUT(c_marr_it->idx, (size_t)0);
        COUT(c_marr_it->max, (size_t)0);
        COUT(c_marr_it->cur, nullptr);
        COUT(c_marr_it->arr, nullptr);

        yyjson::MutableObjectIterator mobj_it_def;
        const yyjson_mut_obj_iter* c_mobj_it = mobj_it_def.c_iter();
        COUT(c_mobj_it->idx, (size_t)0);
        COUT(c_mobj_it->max, (size_t)0);
        COUT(c_mobj_it->cur, nullptr);
        COUT(c_mobj_it->obj, nullptr);
    }

    DESC("iterator copy =");
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

        // Test Mutable single element array
        auto doc3 = ~doc1;
        auto iter3 = doc3.root().arrayIter();
        COUT(iter3.isValid(), true);
        COUT(iter3.value().toInteger(), 999);
        ++iter3;
        COUT(iter3.isValid(), false);

        // Test Mutable single element object
        auto doc4 = ~doc2;
        auto iter4 = doc4.root().objectIter();
        COUT(iter4.isValid(), true);
        COUT(std::string(iter4.name() ? iter4.name() : ""), "only");
        COUT(*iter4 | false, true);
        ++iter4;
        COUT(iter4.isValid(), false);
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

        // Test Mutable single empty array
        auto doc3 = ~doc1;
        auto iter3 = doc3.root().arrayIter();
        COUT(iter3.isValid(), false);

        int count3 = 0;
        while (iter3.isValid()) {
            count3++;
            ++iter3;
            INF_LOOP_BREAK(count3 > 5, "Should not iterate empty array");
        }
        COUT(count3, 0);

        // Test Mutable single empty object
        auto doc4 = ~doc2;
        auto iter4 = doc4.root().objectIter();
        COUT(iter4.isValid(), false);

        int count4 = 0;
        while (iter4.isValid()) {
            count4++;
            ++iter4;
            INF_LOOP_BREAK(count4 > 5, "Should not iterate empty object");
        }
        COUT(count4, 0);
    }
}

DEF_TAST(iterator_begin_end, "test begin/end pattern")
{
    DESC("readonly array iterator");
    {
        std::string jsonText = "[\"first\", \"second\", \"third\"]";
        yyjson::Document doc(jsonText);
        auto root = doc.root();

        std::vector<std::string> values;
        for (auto it = root.beginArray(); it != root.endArray(); ++it) {
            values.push_back(it.value() | "");
            INF_LOOP_BREAK(values.size() > 10, "Infinite loop in begin/end pattern!");
        }
        COUT(values.size(), 3);
        COUT(values[0], "first");
        COUT(values[1], "second");
        COUT(values[2], "third");
    }

    DESC("readonly object iterator");
    {
        std::string objText = "{\"x\": 100, \"y\": 200}";
        yyjson::Document objDoc(objText);
        auto root = objDoc.root();

        std::set<std::string> keys;
        int count = 0;
        for (auto it = root.beginObject(); it != root.endObject(); ++it, ++count) {
            keys.insert(it.key() | "");
            INF_LOOP_BREAK(count > 5, "Infinite loop in object begin/end pattern!");
        }
        COUT(keys.size(), 2);
        COUT(keys.count("x"), 1);
        COUT(keys.count("y"), 1);
    }

    DESC("mutable array iterator");
    {
        yyjson::MutableDocument mutDoc("[\"A\", \"B\"]");
        auto root = mutDoc.root();
        std::string result;
        for (auto it = root.beginArray(); it != root.endArray(); ++it) {
            result += (*it | "");
        }
        COUT(result, "AB");
    }

    DESC("mutable object iterator");
    {
        yyjson::MutableDocument mutDoc("{\"a\":1, \"b\":2}");
        auto root = mutDoc.root();
        int sum = 0;
        for (auto it = root.beginObject(); it != root.endObject(); ++it) {
            sum += *it | 0;
        }
        COUT(sum, 3);
    }

    DESC("iterator from different containers");
    {
        yyjson::Document doc1("[1,2]");
        yyjson::Document doc2("[1,2]");
        COUT(doc1.root().beginArray() != doc2.root().beginArray(), true);
        COUT(doc1.root().endArray() != doc2.root().endArray(), true);

        int count = 0;
        for (auto it = doc1.root().beginArray(); it != doc2.root().endArray(); ++it)
        {
            count++;
            // expected inf loop
            INF_LOOP_BREAK(count > 10, "Infinite loop expected and detected");
        }
        COUT(count > 2, true);
    }
}

