/**
 * @file t_iterator.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief iterator functionality test for xyjson
 * */
#include "couttast/couttast.h"
#include "xyjson.h"
#include <set>

/* t_iterator.cpp - 迭代器功能测试
 * 包含：
 * - 迭代器功能
 * - 迭代器操作符
 * - 标准begin/end模式
 * - 可变迭代器模式
 */

DEF_TAST(iterator_functionality, "test array and object iterator functionality")
{
    std::string jsonText = R"json({
        "array": [1, 2, 3, 4, 5],
        "object": {"key1": "value1", "key2": "value2", "key3": "value3"},
        "nested_array": [[1, 2], [3, 4]],
        "nested_object": {"obj1": {"a": 1}, "obj2": {"b": 2}}
    })json";

    // Parse JSON using yyjson
    yyjson::Document doc(jsonText);
    COUT(doc.hasError(), false);

    // Test array iterator
    {
        auto arrayVal = doc / "array";
        COUT(arrayVal.isArray(), true);

        auto iter = arrayVal.arrayIter();
        COUT(iter.isValid(), true);

        int expected = 1;
        while (iter.isValid()) {
            int value = iter->value | -1;
            COUT(value, expected++);
            ++iter;
        }
        COUT(expected, 6); // Should have iterated 5 times
    }

    // Test array iterator operator chain
    {
        auto iter = doc / "array" % 0;
        COUT(iter.isValid(), true);
        int expected = 1;
        while (iter) {
            int value = iter->value | -1;
            COUT(value, expected++);
            ++iter;
        }
        COUT(expected, 6); // Should have iterated 5 times
    }

    // Test object iterator
    {
        auto objectVal = doc / "object";
        COUT(objectVal.isObject(), true);

        auto iter = objectVal.objectIter();
        COUT(iter.isValid(), true);

        int keyCount = 0;

        while (iter.isValid()) {
            std::string key = iter->key ? iter->key : "";
            std::string value = iter->value | "";
            COUT(key.empty(), false);
            COUT(value.empty(), false);
            COUT(value.find("value") != std::string::npos, true);

            keyCount++;
            ++iter;
        }
        COUT(keyCount, 3);
    }

    // Test object iterator operator chain
    {
        auto iter = doc / "object" % "";
        COUT(iter.isValid(), true);
        int keyCount = 0;
        while (iter.isValid()) {
            std::string key = iter->key ? iter->key : "";
            std::string value = iter->value | "";
            COUT(key.empty(), false);
            COUT(value.empty(), false);
            COUT(value.find("value") != std::string::npos, true);
            keyCount++;
            ++iter;
        }
        COUT(keyCount, 3);
    }

    // Test iterator on non-array/object (should be invalid)
    {
        auto numberVal = doc / "array" / 0; // First element of array
        COUT(numberVal.isNumber(), true);

        auto arrayIter = numberVal.arrayIter();
        COUT(arrayIter.isValid(), false);

        auto objectIter = numberVal.objectIter();
        COUT(objectIter.isValid(), false);
    }

    // Test nested array iteration
    {
        auto nestedArray = doc / "nested_array";
        COUT(nestedArray.isArray(), true);

        auto outerIter = nestedArray.arrayIter();
        int outerCount = 0;

        while (outerIter.isValid()) {
            auto innerArray = outerIter->value;
            COUT(innerArray.isArray(), true);

            auto innerIter = innerArray.arrayIter();
            int innerCount = 0;
            int expectedInner = 1;

            while (innerIter.isValid()) {
                int value = innerIter->value | -1;
                COUT(value, expectedInner++ + outerCount * 2);
                ++innerIter;
                innerCount++;
            }
            COUT(innerCount, 2);

            ++outerIter;
            outerCount++;
        }
        COUT(outerCount, 2);
    }

    // Test iterator comparison operators
    {
        auto arrayVal = doc / "array";
        auto iter1 = arrayVal.arrayIter();
        auto iter2 = arrayVal.arrayIter();

        COUT(iter1 == iter2, true);
        COUT(iter1 != iter2, false);

        ++iter1;
        COUT(iter1 == iter2, false);
        COUT(iter1 != iter2, true);

        // Create invalid iterator
        auto invalidIter = yyjson::ArrayIterator();
        COUT(iter1 == invalidIter, false);
        COUT(iter1 != invalidIter, true);
        COUT(invalidIter == invalidIter, true);
    }

    // Test range-based for loop simulation (manual iteration)
    {
        auto arrayVal = doc / "array";
        std::vector<int> values;

        for (auto iter = arrayVal.arrayIter(); iter.isValid(); ++iter) {
            values.push_back(iter->value | -1);
        }

        COUT(values.size(), 5);
        for (size_t i = 0; i < values.size(); i++) {
            COUT(values[i], static_cast<int>(i + 1));
        }
    }

    // Test object iterator with range-based for simulation
    {
        auto objectVal = doc / "object";
        std::map<std::string, std::string> keyValuePairs;

        for (auto iter = objectVal.objectIter(); iter.isValid(); ++iter) {
            std::string key = iter->key ? iter->key : "";
            std::string value = iter->value | "";
            keyValuePairs[key] = value;
        }

        COUT(keyValuePairs.size(), 3);
        COUT(keyValuePairs.count("key1") > 0, true);
        COUT(keyValuePairs.count("key2") > 0, true);
        COUT(keyValuePairs.count("key3") > 0, true);
    }
}

DEF_TAST(iterator_operators, "test iterator operators and % operator")
{
    std::string jsonText = R"json({
        "array": [10, 20, 30, 40, 50],
        "object": {"first": "value1", "second": "value2", "third": "value3"},
        "empty_array": [],
        "empty_object": {}
    })json";

    yyjson::Document doc(jsonText);
    COUT(doc.hasError(), false);

    // Test operator bool and operator!
    {
        auto arrayVal = doc / "array";
        auto iter = arrayVal.arrayIter();

        COUT(static_cast<bool>(iter), true);
        COUT(!iter, false);

        // Move to end
        while (iter) {
            ++iter;
        }

        COUT(static_cast<bool>(iter), false);
        COUT(!iter, true);
    }

    // Test % operator for arrays
    {
        auto arrayVal = doc / "array";

        // % 0 should give first element
        auto iter1 = arrayVal % 0;
        COUT(static_cast<bool>(iter1), true);
        COUT(iter1->value | -1, 10);

        // % 2 should give third element
        auto iter2 = arrayVal % 2;
        COUT(static_cast<bool>(iter2), true);
        COUT(iter2->value | -1, 30);

        // % with large number should give invalid iterator
        auto iter3 = arrayVal % 10;
        COUT(static_cast<bool>(iter3), false);
    }

    // Test % operator for objects
    {
        auto objectVal = doc / "object";

        // % "" should give first element
        auto iter1 = objectVal % "";
        COUT(static_cast<bool>(iter1), true);

        // % "second" should position to "second" key
        auto iter2 = objectVal % "second";
        COUT(static_cast<bool>(iter2), true);
        COUT(iter2->key ? std::string(iter2->key) : "", "second");
        COUT(iter2->value | std::string(), "value2");

        // % with non-existent key should give invalid iterator
        auto iter3 = objectVal % "nonexistent";
        COUT(static_cast<bool>(iter3), false);
    }

    // Test advance method
    {
        auto arrayVal = doc / "array";
        auto iter = arrayVal.arrayIter();

        COUT(iter->value | -1, 10);

        iter.advance(2);
        COUT(iter->value | -1, 30);

        iter.advance(1);
        COUT(iter->value | -1, 40);

        // Advance beyond end
        iter.advance(10);
        COUT(static_cast<bool>(iter), false);
    }

    // Test seek method for arrays
    {
        auto arrayVal = doc / "array";
        auto iter = arrayVal.arrayIter();

        iter.seek(3); // Fourth element (0-based)
        COUT(iter->value | -1, 40);

        iter.seek(0, true); // Back to first element
        COUT(iter->value | -1, 10);

        // Seek beyond bounds
        iter.seek(10);
        COUT(static_cast<bool>(iter), false);
    }

    // Test seek method for objects
    {
        auto objectVal = doc / "object";
        auto iter = objectVal.objectIter();

        iter.seek("third");
        COUT(iter->key ? std::string(iter->key) : "", "third");
        COUT(iter->value | std::string(), "value3");

        iter.seek("first", true);
        COUT(iter->key ? std::string(iter->key) : "", "first");
        COUT(iter->value | std::string(), "value1");

        // Seek non-existent key
        iter.seek("nonexistent");
        COUT(static_cast<bool>(iter), false);
    }

    // Test % operator on empty containers
    {
        auto emptyArray = doc / "empty_array";
        auto emptyObj = doc / "empty_object";

        COUT(static_cast<bool>(emptyArray % 0), false);
        COUT(static_cast<bool>(emptyObj % ""), false);
        COUT(static_cast<bool>(emptyObj % "anykey"), false);
    }

    // Test % operator on non-container types
    {
        auto numberVal = doc / "array" / 0; // First element of array
        COUT(static_cast<bool>(numberVal % 0), false);
        COUT(static_cast<bool>(numberVal % ""), false);
    }

    // Test new iterator features: += operator, rewind(), and seek() with reset
    {
        auto arrayVal = doc / "array";
        auto objectVal = doc / "object";

        // Test ArrayIterator += operator
        auto arrayIter = arrayVal.arrayIter();
        COUT(arrayIter->key, 0);
        COUT(arrayIter->value | -1, 10);

        arrayIter += 2; // Advance by 2 steps
        COUT(arrayIter->key, 2);
        COUT(arrayIter->value | -1, 30);

        // Test ArrayIterator rewind()
        arrayIter.rewind();
        COUT(arrayIter->key, 0);
        COUT(arrayIter->value | -1, 10);

        // Test ArrayIterator seek() with reset
        arrayIter.seek(3, true); // Seek to index 3 with reset
        COUT(arrayIter->key, 3);
        COUT(arrayIter->value | -1, 40);

        // Test ObjectIterator += operator
        auto objectIter = objectVal.objectIter();
        COUT(objectIter->key ? std::string(objectIter->key) : "", "first");
        COUT(objectIter->value | std::string(), "value1");

        objectIter += 1; // Advance by 1 step
        COUT(objectIter->key ? std::string(objectIter->key) : "", "second");
        COUT(objectIter->value | std::string(), "value2");

        // Test ObjectIterator rewind()
        objectIter.rewind();
        COUT(objectIter->key ? std::string(objectIter->key) : "", "first");
        COUT(objectIter->value | std::string(), "value1");

        // Test ObjectIterator seek() with reset
        objectIter.seek("third", true); // Seek to key "third" with reset
        COUT(objectIter->key ? std::string(objectIter->key) : "", "third");
        COUT(objectIter->value | std::string(), "value3");

        // Test ObjectIterator seek() without reset (should continue from current position)
        // This should fail since "fourth" doesn't exist
        objectIter.seek("fourth", false); // Seek to key "fourth" without reset
        COUT(objectIter.isValid(), false);

        // Test arrayIter() with start index parameter
        auto arrayIter2 = arrayVal.arrayIter(2); // Start at index 2
        COUT(arrayIter2->key, 2);
        COUT(arrayIter2->value | -1, 30);

        // Test objectIter() with start key parameter
        auto objectIter2 = objectVal.objectIter("third"); // Start at key "third"
        COUT(objectIter2->key ? std::string(objectIter2->key) : "", "third");
        COUT(objectIter2->value | std::string(), "value3");
    }

    // Test %= operator overloads on iterators
    {
        auto arrayVal = doc / "array";
        auto objectVal = doc / "object";

        auto arrayIter = arrayVal.arrayIter();
        auto objectIter = objectVal.objectIter();

        // Test ArrayIterator % operator (calls seek())
        arrayIter %= 3; // Should seek to index 3
        COUT(arrayIter->key, 3);
        COUT(arrayIter->value | -1, 40);

        // Test ObjectIterator % operator (calls seek())
        // This should fail since "fourth" doesn't exist
        objectIter %= "fourth"; // Should seek to key "fourth"
        COUT(objectIter.isValid(), false);
    }

    // Test % operator overloads on iterators
    {
        auto arrayVal = doc / "array";
        auto objectVal = doc / "object";

        auto arrayIter = arrayVal.arrayIter();
        auto objectIter = objectVal.objectIter();

        auto ait = arrayIter % 3; // new iterator seek to index 3
        COUT(ait->key, 3);
        COUT(ait->value | -1, 40);
        COUT(arrayIter->key, 0);
        COUT(arrayIter->value | -1, 10);

        auto oit = objectIter % "fourth";
        COUT(oit.isValid(), false);
        COUT(objectIter.isValid(), true);
        oit = objectIter % "second";
        COUT(oit.isValid(), true);
        COUT(oit->key, std::string("second"));
        COUT(oit->value | "", std::string("value2"));
    }

    // Test + operator overloads on iterators
    {
        auto arrayVal = doc / "array";
        auto objectVal = doc / "object";

        auto arrayIter = arrayVal.arrayIter();
        auto objectIter = objectVal.objectIter();

        auto ait = arrayIter + 3; // new iterator seek to index 3
        COUT(ait->key, 3);
        COUT(ait->value | -1, 40);
        COUT(arrayIter->key, 0);
        COUT(arrayIter->value | -1, 10);

        auto oit = objectIter + 4;
        COUT(oit.isValid(), false);
        COUT(objectIter.isValid(), true);
        oit = objectIter + 2;
        COUT(oit.isValid(), true);
        COUT(oit->key, std::string("third"));
        COUT(oit->value | "", std::string("value3"));
    }

    // Test postfix ++ operator for iterators
    {
        DESC("Test postfix ++ operator for ArrayIterator");
        auto arrayVal = doc / "array";
        auto iter = arrayVal.arrayIter();

        // Test postfix ++: save current state, then increment
        auto savedIter = iter++;
        COUT(savedIter->value | -1, 10);  // Should return original state
        COUT(iter->value | -1, 20);       // Should be incremented

        // Test another postfix ++
        savedIter = iter++;
        COUT(savedIter->value | -1, 20);  // Should return original state
        COUT(iter->value | -1, 30);       // Should be incremented

        // Test at end of array
        iter += 2; // Move to last element
        COUT(iter->value | -1, 50);

        savedIter = iter++;
        COUT(iter.isValid(), false);      // Should be invalid after last
        COUT(savedIter->value | -1, 50);  // Should return last element
    }

    {
        DESC("Test postfix ++ operator for ObjectIterator");
        auto objectVal = doc / "object";
        auto iter = objectVal.objectIter();

        // Test postfix ++: save current state, then increment
        auto savedIter = iter++;
        COUT(savedIter->key ? std::string(savedIter->key) : "", "first");
        COUT(savedIter->value | std::string(), "value1");
        COUT(iter->key ? std::string(iter->key) : "", "second");
        COUT(iter->value | std::string(), "value2");

        // Test another postfix ++
        savedIter = iter++;
        COUT(savedIter->key ? std::string(savedIter->key) : "", "second");
        COUT(iter->key ? std::string(iter->key) : "", "third");

        // Test at end of object
        savedIter = iter++;
        COUT(iter.isValid(), false);      // Should be invalid after last
        COUT(savedIter->key ? std::string(savedIter->key) : "", "third");
    }
}

DEF_TAST(iterator_begin_end, "test standard begin/end iterator pattern")
{
    std::string jsonText = R"json({
        "array": [1, 2, 3, 4, 5],
        "object": {"key1": "value1", "key2": "value2", "key3": "value3"},
        "empty_array": [],
        "empty_object": {}
    })json";

    yyjson::Document doc(jsonText);
    COUT(doc.hasError(), false);

    // Test standard array begin/end pattern
    {
        auto arrayVal = doc / "array";
        COUT(arrayVal.isArray(), true);

        auto begin = arrayVal.beginArray();
        auto end = arrayVal.endArray();

        COUT(begin.isValid(), true);
        COUT(end.isValid(), false);
        COUT(begin == end, false);
        COUT(begin != end, true);

        int expected = 1;
        std::vector<int> values;

        // Standard iterator loop
        for (auto it = begin; it != end; ++it) {
            COUT(it.isValid(), true);
            int value = it->value | -1;
            COUT(value, expected++);
            values.push_back(value);
        }

        COUT(values.size(), 5);
        COUT(expected, 6);

        // Verify all values were collected correctly
        for (size_t i = 0; i < values.size(); i++) {
            COUT(values[i], static_cast<int>(i + 1));
        }
    }

    // Test standard object begin/end pattern
    {
        auto objectVal = doc / "object";
        COUT(objectVal.isObject(), true);

        auto begin = objectVal.beginObject();
        auto end = objectVal.endObject();

        COUT(begin.isValid(), true);
        COUT(end.isValid(), false);
        COUT(begin == end, false);
        COUT(begin != end, true);

        std::set<std::string> expectedKeys = {"key1", "key2", "key3"};
        std::set<std::string> collectedKeys;

        // Standard iterator loop
        for (auto it = begin; it != end; ++it) {
            COUT(it.isValid(), true);
            COUT(it->key != nullptr, true);
            COUT(it->value.isValid(), true);

            std::string key = it->key ? it->key : "";
            std::string value = it->value | "";

            COUT(expectedKeys.count(key) > 0, true);
            COUT(value.find("value") != std::string::npos, true);

            collectedKeys.insert(key);
        }

        COUT(collectedKeys.size(), 3);
        COUT(collectedKeys == expectedKeys, true);
    }

    // Test empty array begin/end
    {
        auto emptyArray = doc / "empty_array";
        COUT(emptyArray.isArray(), true);
        COUT(emptyArray.size(), 0);

        auto begin = emptyArray.beginArray();
        auto end = emptyArray.endArray();

        COUT(begin.isValid(), false); // Empty array should have invalid begin
        COUT(end.isValid(), false);   // Empty array should have invalid end
        COUT(begin == end, true);      // Both should be equal (invalid)

        // Loop should not execute
        int loopCount = 0;
        for (auto it = begin; it != end; ++it) {
            loopCount++;
        }
        COUT(loopCount, 0);
    }

    // Test empty object begin/end
    {
        auto emptyObject = doc / "empty_object";
        COUT(emptyObject.isObject(), true);
        COUT(emptyObject.size(), 0);

        auto begin = emptyObject.beginObject();
        auto end = emptyObject.endObject();

        COUT(begin.isValid(), false); // Empty object should have invalid begin
        COUT(end.isValid(), false);   // Empty object should have invalid end
        COUT(begin == end, true);      // Both should be equal (invalid)

        // Loop should not execute
        int loopCount = 0;
        for (auto it = begin; it != end; ++it) {
            loopCount++;
        }
        COUT(loopCount, 0);
    }

    // Test non-array non-object values
    {
        auto numberVal = doc / "array" / 0; // Get first element (number 1)
        COUT(numberVal.isNumber(), true);

        auto beginArray = numberVal.beginArray();
        auto endArray = numberVal.endArray();
        auto beginObject = numberVal.beginObject();
        auto endObject = numberVal.endObject();

        // All should be invalid for non-array/non-object
        COUT(beginArray.isValid(), false);
        COUT(endArray.isValid(), false);
        COUT(beginObject.isValid(), false);
        COUT(endObject.isValid(), false);

        // All end iterators should equal each other
        COUT(beginArray == endArray, true);
        COUT(beginObject == endObject, true);
        // Different iterator types cannot be compared directly
        // COUT(beginArray == endObject, false); // Different types, not comparable
    }

    // Test iterator from invalid/empty value
    {
        yyjson::Value emptyVal;
        COUT(emptyVal.isValid(), false);

        auto beginArray = emptyVal.beginArray();
        auto endArray = emptyVal.endArray();
        auto beginObject = emptyVal.beginObject();
        auto endObject = emptyVal.endObject();

        // All should be invalid
        COUT(beginArray.isValid(), false);
        COUT(endArray.isValid(), false);
        COUT(beginObject.isValid(), false);
        COUT(endObject.isValid(), false);
    }
}

DEF_TAST(iterator_mut_begend, "test mutable standard begin/end iterator pattern")
{
    std::string jsonText = R"json({
        "array": [1, 2, 3, 4, 5],
        "object": {"key1": "value1", "key2": "value2", "key3": "value3"},
        "empty_array": [],
        "empty_object": {}
    })json";

    yyjson::MutableDocument doc(jsonText);
    COUT(doc.hasError(), false);

    // Test standard array begin/end pattern with mutable iterators
    {
        auto arrayVal = doc / "array";
        COUT(arrayVal.isArray(), true);

        auto begin = arrayVal.beginArray();
        auto end = arrayVal.endArray();

        COUT(begin.isValid(), true);
        COUT(end.isValid(), false);
        COUT(begin == end, false);
        COUT(begin != end, true);

        int expected = 1;
        std::vector<int> values;

        // Standard iterator loop with mutable access
        for (auto it = begin; it != end; ++it) {
            COUT(it.isValid(), true);
            int value = it->value | -1;
            COUT(value, expected++);
            values.push_back(value);

            // Test mutable value modification during iteration
            it->value = value * 10; // Modify the value
        }

        COUT(values.size(), 5);
        COUT(expected, 6);

        // Verify all values were modified correctly
        for (size_t i = 0; i < values.size(); i++) {
            COUT(arrayVal[i] | 0, static_cast<int>((i + 1) * 10));
        }
    }

    // Test standard object begin/end pattern with mutable iterators
    {
        auto objectVal = doc / "object";
        COUT(objectVal.isObject(), true);

        auto begin = objectVal.beginObject();
        auto end = objectVal.endObject();

        COUT(begin.isValid(), true);
        COUT(end.isValid(), false);
        COUT(begin == end, false);
        COUT(begin != end, true);

        std::set<std::string> expectedKeys = {"key1", "key2", "key3"};
        std::set<std::string> collectedKeys;

        // Standard iterator loop with mutable access
        for (auto it = begin; it != end; ++it) {
            COUT(it.isValid(), true);
            COUT(it->key != nullptr, true);
            COUT(it->value.isValid(), true);

            std::string key = it->key ? it->key : "";
            std::string value = it->value | "";

            COUT(expectedKeys.count(key) > 0, true);
            COUT(value.find("value") != std::string::npos, true);

            collectedKeys.insert(key);

            // Test mutable value modification during iteration
            it->value = std::string("modified_") + value;
        }

        COUT(collectedKeys.size(), 3);
        COUT(collectedKeys == expectedKeys, true);

        // Verify all values were modified correctly
        for (const auto& key : expectedKeys) {
            std::string modifiedValue = objectVal[key] | "";
            COUT(modifiedValue, std::string("modified_value") + key.substr(3));
        }
    }

    // Test empty array begin/end
    {
        auto emptyArray = doc / "empty_array";
        COUT(emptyArray.isArray(), true);
        COUT(emptyArray.size(), 0);

        auto begin = emptyArray.beginArray();
        auto end = emptyArray.endArray();

        COUT(begin.isValid(), false); // Empty array should have invalid begin
        COUT(end.isValid(), false);   // Empty array should have invalid end
        COUT(begin == end, true);      // Both should be equal (invalid)

        // Loop should not execute
        int loopCount = 0;
        for (auto it = begin; it != end; ++it) {
            loopCount++;
        }
        COUT(loopCount, 0);
    }

    // Test empty object begin/end
    {
        auto emptyObject = doc / "empty_object";
        COUT(emptyObject.isObject(), true);
        COUT(emptyObject.size(), 0);

        auto begin = emptyObject.beginObject();
        auto end = emptyObject.endObject();

        COUT(begin.isValid(), false); // Empty object should have invalid begin
        COUT(end.isValid(), false);   // Empty object should have invalid end
        COUT(begin == end, true);      // Both should be equal (invalid)

        // Loop should not execute
        int loopCount = 0;
        for (auto it = begin; it != end; ++it) {
            loopCount++;
        }
        COUT(loopCount, 0);
    }

    // Test non-array non-object values
    {
        auto numberVal = doc / "array" / 0; // Get first element (number 1)
        COUT(numberVal.isNumber(), true);

        auto beginArray = numberVal.beginArray();
        auto endArray = numberVal.endArray();
        auto beginObject = numberVal.beginObject();
        auto endObject = numberVal.endObject();

        // All should be invalid for non-array/non-object
        COUT(beginArray.isValid(), false);
        COUT(endArray.isValid(), false);
        COUT(beginObject.isValid(), false);
        COUT(endObject.isValid(), false);

        // All end iterators should equal each other
        COUT(beginArray == endArray, true);
        COUT(beginObject == endObject, true);
    }

    // Test iterator from invalid/empty value
    {
        yyjson::MutableValue emptyVal;
        COUT(emptyVal.isValid(), false);

        auto beginArray = emptyVal.beginArray();
        auto endArray = emptyVal.endArray();
        auto beginObject = emptyVal.beginObject();
        auto endObject = emptyVal.endObject();

        // All should be invalid
        COUT(beginArray.isValid(), false);
        COUT(endArray.isValid(), false);
        COUT(beginObject.isValid(), false);
        COUT(endObject.isValid(), false);
    }

    // Test % operator for mutable iterators
    {
        auto arrayVal = doc / "array";
        auto iter = arrayVal % 2; // Start from index 2
        COUT(iter.isValid(), true);
        COUT(iter->key, 2); // Should be index 2 (0-based)
        COUT(iter->value | 0, 30); // Should be modified value (3 * 10)

        auto objectVal = doc / "object";
        auto objIter = objectVal % "key2"; // Seek to key2
        COUT(objIter.isValid(), true);
        COUT(objIter->key, std::string("key2"));
        COUT(objIter->value | "", std::string("modified_value2"));
    }
}
