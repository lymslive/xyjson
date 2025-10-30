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

        auto iter4 = doc.root().arrayIter();
        COUT(+iter4, (size_t)0); // Get current index
        COUT(-iter4, nullptr); // No-op for array iterator
        COUT(!~iter4, true); // No-op for array iterator
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

        auto iter2 = doc.root().objectIter();
        COUT(+iter2, (size_t)0); // Get current index
        COUT(std::string(-iter2 ? -iter2 : ""), "name"); // Get current key name
        COUT(!~iter2, false); // Get current key node

        ++iter2;
        COUT(+iter2, (size_t)1); // Get current index
        COUT(std::string(-iter2 ? -iter2 : ""), "value"); // Get current key name
    }

    DESC("mutable array iterator:");
    {
        std::string jsonText = "[100, 200, 300]";
        yyjson::MutableDocument doc(jsonText);

        auto iter = doc.root().arrayIter();

        // Test dereference operator
        COUT((*iter).toInteger(), 100);

        // Test arrow operator
        COUT(iter->toInteger(), 100);

        // Test increment operators
        ++iter;
        COUT(iter->toInteger(), 200);

        auto old = iter++;
        COUT(old->toInteger(), 200);
        COUT(iter->toInteger(), 300);

        auto iter2 = doc.root().arrayIter();
        COUT(+iter2, (size_t)0); // Get current index
        COUT(-iter2, nullptr); // No-op for array iterator
        COUT(!~iter2, true); // No-op for array iterator
    }

    DESC("mutable object iterator:");
    {
        std::string jsonText = "{\"key1\": \"value1\", \"key2\": 999}";
        yyjson::MutableDocument doc(jsonText);

        auto iter = doc.root().objectIter();

        // Test basic functionality
        COUT(std::string(iter.name() ? iter.name() : ""), "key1");
        COUT(iter->toString(), "value1");

        ++iter;
        COUT(std::string(iter.name() ? iter.name() : ""), "key2");
        COUT(iter->toInteger(), 999);

        auto iter2 = doc.root().objectIter();
        COUT(+iter2, (size_t)0); // Get current index
        COUT(std::string(-iter2 ? -iter2 : ""), "key1"); // Get current key name
        COUT((~iter2).toString(), "key1"); // Get current key node value

        ++iter2;
        COUT(+iter2, (size_t)1); // Get current index
        COUT(std::string(-iter2 ? -iter2 : ""), "key2"); // Get current key name
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

    DESC("test iterator end value access");
    {
        // Test MutableArrayIterator c_val() at end
        yyjson::MutableDocument doc1("[1, 2, 3]");
        auto iter1 = doc1.root().arrayIter();

        // Iterate to end
        while (iter1.isValid()) {
            ++iter1;
        }

        // Test that c_val() returns nullptr at end
        COUT(iter1.c_val() == nullptr, true);

        // Test MutableObjectIterator c_key() at end
        yyjson::MutableDocument doc2("{\"a\":1, \"b\":2}");
        auto iter2 = doc2.root().objectIter();

        // Iterate to end
        while (iter2.isValid()) {
            ++iter2;
        }

        // Test that c_key()/c_val() returns nullptr at end
        COUT(iter2.c_key() == nullptr, true);
        COUT(iter2.c_val() == nullptr, true);

        // Test readonly iterators for comparison
        yyjson::Document doc3("[1, 2, 3]");
        auto iter3 = doc3.root().arrayIter();

        while (iter3.isValid()) {
            ++iter3;
        }

        COUT(iter3.c_val() == nullptr, true);

        yyjson::Document doc4("{\"a\":1, \"b\":2}");
        auto iter4 = doc4.root().objectIter();

        while (iter4.isValid()) {
            ++iter4;
        }

        COUT(iter4.c_key() == nullptr, true);
        COUT(iter4.c_val() == nullptr, true);
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

DEF_TAST(iterator_arithmetic, "test iterator arithmetic operators % %= + +=")
{
    DESC("array iterator with % and + operators");
    {
        std::string jsonText = "[100, 200, 300, 400, 500]";
        yyjson::Document doc(jsonText);
        auto root = doc.root();

        // Test json % index creates iterator at specific position
        auto iter1 = root % 2; // Start at index 2 (value 300)
        COUT(iter1.isValid(), true);
        COUT(iter1->toInteger(), 300);
        COUT(+iter1, (size_t)2); // Verify current index

        // Test + operator (creates new iterator at offset)
        auto iter2 = iter1 + 1; // Move forward 1 step
        COUT(iter2.isValid(), true);
        COUT(iter2->toInteger(), 400);
        COUT(+iter2, (size_t)3);

        // Test += operator (modifies iterator in place)
        iter1 += 2; // Move forward 2 steps from index 2
        COUT(iter1.isValid(), true);
        COUT(iter1->toInteger(), 500);
        COUT(+iter1, (size_t)4);

        // Test % operator (moves to specific index)
        auto iter3 = iter1 % 0; // Jump to index 0
        COUT(iter3.isValid(), true);
        COUT(iter3->toInteger(), 100);
        COUT(+iter3, (size_t)0);

        // Test %= operator (modifies iterator to specific index)
        iter3 %= 1; // Jump to index 1
        COUT(iter3.isValid(), true);
        COUT(iter3->toInteger(), 200);
        COUT(+iter3, (size_t)1);

        // Test boundary conditions
        auto iter4 = root % 100; // Invalid index
        COUT(iter4.isValid(), false);

        auto iter5 = root % 4; // Last valid index
        COUT(iter5.isValid(), true);
        COUT(iter5->toInteger(), 500);

        iter5 += 1; // Move beyond end
        COUT(iter5.isValid(), false);
    }

    DESC("object iterator with % and + operators");
    {
        std::string jsonText = "{\"a\": 1, \"b\": 2, \"c\": 3, \"d\": 4}";
        yyjson::Document doc(jsonText);
        auto root = doc.root();

        // Test json % key creates iterator at specific position
        auto iter1 = root % "b"; // Start at key "b"
        COUT(iter1.isValid(), true);
        COUT(iter1->toInteger(), 2);
        COUT(std::string(-iter1 ? -iter1 : ""), "b");

        // Test + operator (moves forward steps)
        auto iter2 = iter1 + 1; // Move forward 1 step
        COUT(iter2.isValid(), true);
        COUT(iter2->toInteger(), 3);
        COUT(std::string(-iter2 ? -iter2 : ""), "c");

        // Test += operator (modifies iterator in place)
        iter1 += 1; // Move forward 1 step from "b"
        COUT(iter1.isValid(), true);
        COUT(iter1->toInteger(), 3);
        COUT(std::string(-iter1 ? -iter1 : ""), "c");

        // Test % operator (moves to specific key)
        auto iter3 = iter1 % "a"; // Jump to key "a"
        COUT(iter3.isValid(), true);
        COUT(iter3->toInteger(), 1);
        COUT(std::string(-iter3 ? -iter3 : ""), "a");

        // Test %= operator (modifies iterator to specific key)
        iter3 %= "d"; // Jump to key "d"
        COUT(iter3.isValid(), true);
        COUT(iter3->toInteger(), 4);
        COUT(std::string(-iter3 ? -iter3 : ""), "d");

        // Test boundary conditions
        auto iter4 = root % "nonexistent"; // Invalid key
        COUT(iter4.isValid(), false);

        auto iter5 = root % "d"; // Last key
        COUT(iter5.isValid(), true);
        COUT(iter5->toInteger(), 4);

        iter5 += 1; // Move beyond end
        COUT(iter5.isValid(), false);
    }

    DESC("mutable array iterator with % and + operators");
    {
        yyjson::MutableDocument doc("[10, 20, 30, 40]");
        auto root = doc.root();

        // Test json % index for mutable array
        auto iter1 = root % 1; // Start at index 1 (value 20)
        COUT(iter1.isValid(), true);
        COUT(iter1->toInteger(), 20);
        COUT(+iter1, (size_t)1);

        // Test + operator
        auto iter2 = iter1 + 2; // Move forward 2 steps
        COUT(iter2.isValid(), true);
        COUT(iter2->toInteger(), 40);
        COUT(+iter2, (size_t)3);

        // Test += operator
        iter1 += 1; // Move forward 1 step
        COUT(iter1.isValid(), true);
        COUT(iter1->toInteger(), 30);
        COUT(+iter1, (size_t)2);

        // Test % operator
        auto iter3 = iter1 % 0; // Jump to index 0
        COUT(iter3.isValid(), true);
        COUT(iter3->toInteger(), 10);
        COUT(+iter3, (size_t)0);

        // Test %= operator
        iter3 %= 3; // Jump to index 3
        COUT(iter3.isValid(), true);
        COUT(iter3->toInteger(), 40);
        COUT(+iter3, (size_t)3);

        // Test boundary conditions
        auto iter4 = root % 10; // Invalid index
        COUT(iter4.isValid(), false);

        auto iter5 = root % 3; // Last valid index
        COUT(iter5.isValid(), true);
        COUT(iter5->toInteger(), 40);

        iter5 += 1; // Move beyond end
        COUT(iter5.isValid(), false);
    }

    DESC("mutable object iterator with % and + operators");
    {
        yyjson::MutableDocument doc("{\"x\": 100, \"y\": 200, \"z\": 300}");
        auto root = doc.root();

        // Test json % key for mutable object
        auto iter1 = root % "y"; // Start at key "y"
        COUT(iter1.isValid(), true);
        COUT(iter1->toInteger(), 200);
        COUT(std::string(-iter1 ? -iter1 : ""), "y");

        // Test + operator
        auto iter2 = iter1 + 1; // Move forward 1 step
        COUT(iter2.isValid(), true);
        COUT(iter2->toInteger(), 300);
        COUT(std::string(-iter2 ? -iter2 : ""), "z");

        // Test += operator
        iter1 += 1; // Move forward 1 step
        COUT(iter1.isValid(), true);
        COUT(iter1->toInteger(), 300);
        COUT(std::string(-iter1 ? -iter1 : ""), "z");

        // Test % operator
        auto iter3 = iter1 % "x"; // Jump to key "x"
        COUT(iter3.isValid(), true);
        COUT(iter3->toInteger(), 100);
        COUT(std::string(-iter3 ? -iter3 : ""), "x");

        // Test %= operator
        iter3 %= "z"; // Jump to key "z"
        COUT(iter3.isValid(), true);
        COUT(iter3->toInteger(), 300);
        COUT(std::string(-iter3 ? -iter3 : ""), "z");

        // Test boundary conditions
        auto iter4 = root % "none"; // Invalid key
        COUT(iter4.isValid(), false);

        auto iter5 = root % "z"; // Last key
        COUT(iter5.isValid(), true);
        COUT(iter5->toInteger(), 300);

        iter5 += 1; // Move beyond end
        COUT(iter5.isValid(), false);
    }
}

DEF_TAST(iterator_fast_seek, "test fast seek functionality with / operator")
{
    DESC("readonly object iterator fast seek");
    {
        std::string jsonText = "{\"name\": \"Alice\", \"age\": 25, \"city\": \"Beijing\"}";
        yyjson::Document doc(jsonText);
        
        auto iter = doc.root().objectIter();
        
        // Test fast seek with string literal
        auto nameValue = iter.seek("name");
        COUT(nameValue.isValid(), true);
        COUT(std::string(nameValue | ""), "Alice");
        
        // Note: yyjson_obj_iter_getn moves iterator to next element after found key
        COUT(iter.index(), size_t(1)); // Iterator moved to next element after "name"
        COUT(std::string(-iter ? -iter : ""), "age"); // Current key is now "age"
        
        // Test fast seek with std::string
        std::string ageKey = "age";
        auto ageValue = iter.seek(ageKey);
        COUT(ageValue.isValid(), true);
        COUT(ageValue.toInteger(), 25);
        
        // Iterator moved to next element after "age"
        COUT(iter.index(), size_t(2)); // Iterator moved to next element after "age"
        COUT(std::string(-iter ? -iter : ""), "city"); // Current key is now "city"
        
        // Test fast seek with / operator
        auto cityValue = iter / "city";
        COUT(cityValue.isValid(), true);
        COUT(std::string(cityValue | ""), "Beijing");
        
        // Iterator moved to next element after "city" (beyond end)
        COUT(iter.isValid(), false); // Iterator is now beyond end
        
        // Test seek with non-existing key
        auto noneValue = iter.seek("nonexistent");
        COUT(noneValue.isValid(), false);
        
        // Test that iterator position is updated after seek
        COUT(iter.isValid(), false); // Iterator remains beyond end
        
        // Reset iterator to test from beginning
        iter.rewind();
        COUT(iter.isValid(), true);
        COUT(std::string(-iter ? -iter : ""), "name"); // Back to first key
        
        // Test sequential seeks
        auto val1 = iter.seek("name");
        COUT(std::string(val1 | ""), "Alice");
        COUT(std::string(-iter ? -iter : ""), "age"); // Moved to next
        
        // Test re-seek with non-existing key, iter stay on
        noneValue = iter.seek("nonexistent");
        COUT(noneValue.isValid(), false);
        COUT(-iter, "age");
        COUT(+iter, size_t(1));

        auto val2 = iter.seek("city");
        COUT(std::string(val2 | ""), "Beijing");
        COUT(iter.isValid(), false); // Moved beyond end after last key
    }
    
    DESC("mutable object iterator fast seek");
    {
        yyjson::MutableDocument doc("{\"id\": 123, \"status\": \"active\", \"score\": 95.5}");
        
        auto iter = doc.root().objectIter();
        
        // Test fast seek with string literal
        auto idValue = iter.seek("id");
        COUT(idValue.isValid(), true);
        COUT(idValue.toInteger(), 123);
        
        // Note: yyjson_mut_obj_iter_getn moves iterator to next element after found key
        COUT(std::string(-iter ? -iter : ""), "status"); // Iterator moved to next key
        
        // Test fast seek with std::string
        std::string statusKey = "status";
        auto statusValue = iter.seek(statusKey);
        COUT(statusValue.isValid(), true);
        COUT(std::string(statusValue | ""), "active");
        
        // Iterator moved to next element after "status"
        COUT(std::string(-iter ? -iter : ""), "score"); // Current key is now "score"
        
        // Test fast seek with / operator
        auto scoreValue = iter / "score";
        COUT(scoreValue.isValid(), true);
        COUT(scoreValue.toNumber(), 95.5);
        
        // Iterator moved beyond end after last key
        COUT(iter.isValid(), false); // Iterator is now beyond end
        
        // Test seek with non-existing key
        auto noneValue = iter.seek("missing");
        COUT(noneValue.isValid(), false);
        
        // Test that iterator position is updated after seek
        COUT(iter.isValid(), false); // Iterator remains beyond end
        
        // Reset iterator to test from beginning
        iter.rewind();
        COUT(iter.isValid(), true);
        COUT(std::string(-iter ? -iter : ""), "id"); // Back to first key
    }
    
    DESC("test array iterator doesn't support fast seek");
    {
        yyjson::Document doc("[10, 20, 30]");
        auto iter = doc.root().arrayIter();
        // Note: Array iterators don't have seek method for keys
        //! auto first = iter / 0;
        //! auto val = iter / "key";
    }
    
    DESC("test seek on empty object iterator");
    {
        yyjson::Document doc("{}");
        auto iter = doc.root().objectIter();
        
        // Test seek on empty object
        COUT(iter.isValid(), false); // Empty object iterator starts invalid
        
        auto noneValue = iter.seek("anykey");
        COUT(noneValue.isValid(), false); // Should not find anything
        
        // Test / operator on empty object
        auto noneValue2 = iter / "otherkey";
        COUT(noneValue2.isValid(), false); // Should not find anything
        
        COUT(iter.isValid(), false); // Iterator should remain invalid
    }
    
    DESC("test seek on default-constructed iterator");
    {
        yyjson::ObjectIterator iter; // Default constructed
        
        // Test seek on default constructed iterator
        COUT(iter.isValid(), false); // Default iterator is invalid
        
        auto noneValue = iter.seek("key");
        COUT(noneValue.isValid(), false); // Should not find anything
        
        // Test / operator on default constructed iterator
        auto noneValue2 = iter / "key";
        COUT(noneValue2.isValid(), false); // Should not find anything
        
        COUT(iter.isValid(), false); // Iterator should remain invalid
    }
    
    DESC("test seek on empty mutable object iterator");
    {
        yyjson::MutableDocument doc("{}");
        auto iter = doc.root().objectIter();
        
        // Test seek on empty mutable object
        COUT(iter.isValid(), false); // Empty object iterator starts invalid
        
        auto noneValue = iter.seek("anykey");
        COUT(noneValue.isValid(), false); // Should not find anything
        
        // Test / operator on empty mutable object
        auto noneValue2 = iter / "otherkey";
        COUT(noneValue2.isValid(), false); // Should not find anything
        
        COUT(iter.isValid(), false); // Iterator should remain invalid
    }
    
    DESC("test seek on default-constructed mutable iterator");
    {
        yyjson::MutableObjectIterator iter; // Default constructed
        
        // Test seek on default constructed mutable iterator
        COUT(iter.isValid(), false); // Default iterator is invalid
        
        auto noneValue = iter.seek("key");
        COUT(noneValue.isValid(), false); // Should not find anything
        
        // Test / operator on default constructed mutable iterator
        auto noneValue2 = iter / "key";
        COUT(noneValue2.isValid(), false); // Should not find anything
        
        COUT(iter.isValid(), false); // Iterator should remain invalid
    }
}

DEF_TAST(iterator_array_insert, "mutable array iterator insert functionality")
{
    DESC("basic insert operation");
    {
        yyjson::MutableDocument doc("[1, 2, 4, 5]");
        auto root = doc.root();
        
        // Test basic insert at position 2 (insert 3 between 2 and 4)
        auto iter = root.arrayIter();
        iter.advance(2); // Move to position 2 (value 4)
        
        // before insert
        COUT(iter.index(), 2);
        COUT(iter->toInteger(), 4);

        // Insert value at current position
        iter.insert(3);

        // after insert
        COUT(iter.index(), 2);
        COUT(iter->toInteger(), 3);

        // Verify the array content
        std::vector<int> values;
        for (auto it = root.arrayIter(); it.isValid(); ++it) {
            values.push_back(*it | 0);
        }
        COUT(root);
        COUT(values.size(), 5);
        COUT(values[0], 1);
        COUT(values[1], 2);
        COUT(values[2], 3); // Newly inserted value
        COUT(values[3], 4);
        COUT(values[4], 5);
    }
    
    DESC("operator << chained insertion");
    {
        yyjson::MutableDocument doc("[1, 2, 3]");
        auto root = doc.root();
        
        // Test operator << for chained insertion
        auto iter = root.arrayIter();
        iter.advance(1); // Move to position 1 (value 2)
        
        // Insert multiple values using operator <<
        iter << "A" << "B" << "C";
        
        // First, let's debug by printing the actual array content
        COUT(root.toString());
        
        // Verify the array content after chained insertion
        std::vector<std::string> values;
        for (auto it = root.arrayIter(); it.isValid(); ++it) {
            values.push_back(it->toString());
        }
        
        COUT(values.size(), 6);
        COUT(values[0], "1");
        COUT(values[1], "A"); // Newly inserted values
        COUT(values[2], "B");
        COUT(values[3], "C");
        COUT(values[4], "2");
        COUT(values[5], "3");
    }
    
    DESC("insert at beginning and end");
    {
        yyjson::MutableDocument doc("[1, 2, 3]");
        auto root = doc.root();
        
        // Test insertion at the beginning
        auto iter1 = root.arrayIter(); // Position 0
        iter1 << "start";
        
        // Verify insertion at beginning
        COUT((*root.arrayIter() | ""), "start");
        
        // Test insertion at the end
        auto iter2 = root.arrayIter();
        iter2.over(); // Move to end
        iter2 << "end";
        
        // Verify last element
        auto iter3 = root.arrayIter();
        iter3.advance(4); // Move to last position (array has 5 elements: indices 0-4)
        COUT((*iter3 | ""), "end");

        COUT(root);

        // move beyond the end.
        auto iter4 = root % 0;
        iter4 += 5;
        COUT(!iter4, true);
        COUT(iter4->toString(), "");
        ++iter4;
        COUT(iter4->toString(), "");
    }
    
    DESC("insert different types");
    {
        yyjson::MutableDocument doc("[1, 2, 3]");
        auto root = doc.root();
        
        // Test insertion with different types
        auto iter = root.arrayIter();
        iter.advance(1);
        iter << 3.14 << true << nullptr;
        
        COUT(root);

        // Verify mixed types insertion
        auto iter2 = root.arrayIter();
        iter2.advance(1);
        COUT((*iter2 | 0.0), 3.14);
        ++iter2;
        COUT((*iter2 | false), true);
        ++iter2;
        COUT(iter2.value().isNull(), true);
    }

    DESC("insert to empty array");
    {
        yyjson::MutableDocument doc("[]");
        auto it = doc % 0;
        // 1 2 int and MutableValue(3), raw yyjson_mut_val*(4)
        it << 1 << 2 << doc*3 << (doc*4).get();
        COUT(-doc, "[1,2,3,4]");
    }
}

DEF_TAST(iterator_array_remove, "mutable array iterator remove functionality")
{
    DESC("basic remove operation");
    {
        yyjson::MutableDocument doc("[1, 2, 3, 4, 5]");
        auto root = doc.root();
        
        // Test removing element at position 2 (value 3)
        auto iter = root.arrayIter();
        iter.advance(2); // Move to position 2
        
        // Remove current element
        auto removed = iter.remove();
        COUT(removed.isValid(), true);
        COUT(removed.toInteger(), 3);
        
        // Verify array content after removal
        COUT(root.toString(), "[1,2,4,5]");
        
        // Verify iterator is still valid after removal
        COUT(iter.isValid(), true);
        COUT(iter.value().toInteger(), 4); // Now points to next element
    }
    
    DESC("remove operator >>");
    {
        yyjson::MutableDocument doc("[1, 2, 3, 4, 5]");
        auto root = doc.root();
        
        auto iter = root.arrayIter();
        iter.advance(1); // Move to position 1 (value 2)
        
        // Test >> operator
        yyjson::MutableValue removed;
        iter >> removed;
        
        COUT(removed.isValid(), true);
        COUT(removed.toInteger(), 2);
        
        // Verify array content
        COUT(root.toString(), "[1,3,4,5]");
        
        // Verify iterator points to next element
        COUT(iter.value().toInteger(), 3);
    }
    
    DESC("chained remove operations");
    {
        yyjson::MutableDocument doc("[1, 2, 3, 4, 5]");
        auto root = doc.root();
        
        auto iter = root.arrayIter();
        iter.advance(1); // Move to position 1 (value 2)
        
        // Remove two elements in sequence
        yyjson::MutableValue removed1, removed2;
        iter >> removed1 >> removed2;
        
        COUT(removed1.toInteger(), 2);
        COUT(removed2.toInteger(), 3);
        
        // Verify array content
        COUT(root.toString(), "[1,4,5]");
        
        // Verify iterator points to next element
        COUT(iter.value().toInteger(), 4);
    }
    
    DESC("remove edge cases");
    {
        yyjson::MutableDocument doc("[1, 2, 3]");
        auto root = doc.root();
        
        // Test removing first element
        auto iter1 = root.arrayIter(); // Position 0
        yyjson::MutableValue removed1;
        iter1 >> removed1;
        COUT(removed1.toInteger(), 1);
        COUT(root.toString(), "[2,3]");
        
        // Test removing last element
        auto iter2 = root.arrayIter();
        iter2.advance(1); // Move to position 1 (value 3)
        yyjson::MutableValue removed2;
        iter2 >> removed2;
        COUT(removed2.toInteger(), 3);
        COUT(root.toString(), "[2]");
        
        // Test removing the only element
        auto iter3 = root.arrayIter();
        yyjson::MutableValue removed3;
        iter3 >> removed3;
        COUT(removed3.toInteger(), 2);
        COUT(root.toString(), "[]");
        
        // Test removing from empty array
        yyjson::MutableValue removed4;
        iter3 >> removed4;
        COUT(removed4.isValid(), false);
    }
    
    DESC("remove and insert combination");
    {
        yyjson::MutableDocument doc("[1, 2, 3, 4, 5]");
        auto root = doc.root();
        
        auto iter = root.arrayIter();
        iter.advance(2); // Move to position 2 (value 3)
        
        // Remove current element
        yyjson::MutableValue removed;
        iter >> removed;
        COUT(removed.toInteger(), 3);
        
        // Insert new element at current position
        iter << "replacement";
        
        // Verify final array content
        COUT(root.toString(), "[1,2,\"replacement\",4,5]");

        // Re-insert to the end.
        // iter.over();  //! not true end state
        iter.advance(20); // 2 is enough
        iter << removed; // copy into
        COUT(root.toString(), "[1,2,\"replacement\",4,5,3]");
        COUT(removed.isValid(), true);

        iter << std::move(removed); // move into
        COUT(root.toString(), "[1,2,\"replacement\",4,5,3,3]");
        COUT(removed.isValid(), false);

        iter << removed; // invalid node, should no effect any more
        COUT(root.toString(), "[1,2,\"replacement\",4,5,3,3]");
        COUT(removed.isValid(), false);
    }
}

