/**
 * @file t_xyjson.cpp
 * @author lymslive
 * @date 2025-09-15
 * @brief test xyjson operator overrides
 * */
#include "couttast/couttast.h"
#include "xyjson.h"
#include <set>
#include <map>

namespace test
{

struct StringRef
{
    const char* str = nullptr;
    size_t len = 0;

    template <size_t N> 
    StringRef(const char(&value)[N]) : str(value), len(N-1)
    {
    }

//  explicit StringRef(const char* value) : str(value), len(::strlen(value))
//  {
//  }

    operator const char *() const { return str; }

private:
    template <size_t N> 
    StringRef(char(&value)[N]);
};

struct StringCopy
{
    const char* str = nullptr;
    size_t len = 0;

//  template <size_t N> 
//  StringCopy(char(&value)[N]) : str(value), len(N-1)
//  {
//  }

    StringCopy(const char* value) : str(value), len(::strlen(value))
    {
    }

    operator const char *() const { return str; }

private:
    template <size_t N> 
    explicit StringCopy(const char(&value)[N]) : str(value), len(N-1)
    {
    }

};

struct Overload
{
    int foo(int) {return 0; };
    int foo(const char*) { return 1;}
    template <size_t N> 
    int foo(const char(&value)[N]) { return 2; }

    template <typename T>
    int fooForward(const T& value) { return foo(value); }

    int bar(int) {return 0; };
    int bar(const char*) { return 1;}
    int bar(StringRef) { return 2;}

    template <typename T>
    int barForward(const T& value) { return bar(value); }

    int baz(int) {return 0; };
    int baz(StringCopy) { return 1;}
    int baz(StringRef) { return 2;}

    template <typename T>
    int bazForward(const T& value) { return baz(value); }
};

} /* test:: */

DEF_TAST(overload_fun, "test overload rule")
{
    test::Overload overload;
    int i = 0;
    const char* psz = "some string";
    char buffer[] = "buffer char[]";
    const char cbuf[] = "const buffer";

    COUT(overload.foo(i), 0);
    COUT(overload.foo(psz), 1);
    COUT(overload.foo(buffer), 2); // char[N] --> const char[]
    COUT(overload.foo(cbuf), 1);
    COUT(overload.foo("string literal"), 1);
    COUT(overload.fooForward(i), 0);
    COUT(overload.fooForward(psz), 1);
    COUT(overload.fooForward(buffer), 1); // char* --> const char*
    COUT(overload.fooForward(cbuf), 1);
    COUT(overload.fooForward("string literal"), 1);

    COUT(overload.bar(i), 0);
    COUT(overload.bar(psz), 1);
    COUT(overload.bar(buffer), 1); // char[N] --> const char[] --> const char*
    COUT(overload.bar(cbuf), 1);
    COUT(overload.bar("string literal"), 1);
    COUT(overload.bar(test::StringRef("string literal")), 2);
    COUT(overload.barForward(i), 0);
    COUT(overload.barForward(psz), 1);
    COUT(overload.barForward(buffer), 1); // char* --> const char*
    COUT(overload.barForward(cbuf), 1);
    COUT(overload.barForward("string literal"), 1);
    COUT(overload.barForward(test::StringRef("string literal")), 2);

    COUT(overload.baz(i), 0);
    COUT(overload.baz(psz), 1);
//  COUT(overload.baz(buffer), 2); // ambiguous
//  COUT(overload.baz(cbuf), 1);
//  COUT(overload.baz("string literal"), 1);
    COUT(overload.baz(test::StringCopy("string literal")), 1);
    COUT(overload.baz(test::StringRef("string literal")), 2);
    COUT(overload.bazForward(i), 0);
    COUT(overload.bazForward(psz), 1);
//  COUT(overload.bazForward(buffer), 1);
//  COUT(overload.bazForward(cbuf), 1);
//  COUT(overload.bazForward("string literal"), 1);
    COUT(overload.bazForward(test::StringCopy("string literal")), 1);
    COUT(overload.bazForward(test::StringRef("string literal")), 2);

}

DEF_TAST(yyjson_operator_read_number, "operator read from yyjson value")
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

DEF_TAST(yyjson_operator_read_string, "operator read from yyjson value")
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

DEF_TAST(yyjson_operator_error, "handle path operator error")
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

DEF_TAST(yyjson_mutable_read_modify, "test read-modify workflow")
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

DEF_TAST(yyjson_value_input, "test input operator << for mutable value")
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

DEF_TAST(yyjson_reread, "test re-read document")
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

DEF_TAST(yyjson_assign_copy, "test = and copy behavior of yyjson wrapper classes")
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

DEF_TAST(yyjson_assign_string, "test string node in yyjson")
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

DEF_TAST(yyjson_assign_string_ref, "test string node reference in yyjson")
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

DEF_TAST(yyjson_document_stream_ops, "test Document stream operators << and >>")
{
    DESC("Test Document stream operators");
    {
        yyjson::Document doc;
        std::string jsonText = R"json({"name": "test", "value": 123})json";
        std::string output;
        
        // Read JSON using << operator
        doc << jsonText;
        COUT(doc.hasError(), false);
        
        // Extract values to verify
        std::string name = doc / "name" | "";
        int value = doc / "value" | 0;
        COUT(name, "test");
        COUT(value, 123);
        
        // Write JSON using >> operator
        doc >> output;
        COUT(output);
        COUT(output.empty(), false);
        COUT(output.find("\"name\":\"test\"") != std::string::npos, true);
        COUT(output.find("\"value\":123") != std::string::npos, true);
    }

    DESC("Test MutableDocument stream operators");
    {
        yyjson::MutableDocument doc;
        std::string jsonText = R"json({"name": "mutable", "value": 456})json";
        std::string output;

        // Read JSON using << operator
        doc << jsonText;
        COUT(doc.hasError(), false);

        // Extract values to verify
        std::string name = doc / "name" | "";
        int value = doc / "value" | 0;
        COUT(name, "mutable");
        COUT(value, 456);

        // Write JSON using >> operator
        doc >> output;
        COUT(output.empty(), false);
        COUT(output.find("\"name\":\"mutable\"") != std::string::npos, true);
        COUT(output.find("\"value\":456") != std::string::npos, true);
    }

    DESC("Test error handling with invalid JSON");
    {
        yyjson::Document doc;
        std::string invalidJson = "{invalid json}";
        
        doc << invalidJson;
        COUT(doc.hasError(), true);
        COUT(!doc, true);
        
        // Try to write invalid document
        std::string output;
        doc >> output;
        COUT(output.empty(), true);
    }
}

DEF_TAST(yyjson_file_stream_ops, "test FILE* and std::fstream stream operations")
{
    DESC("Test FILE* stream operations");
    {
        // Create a temporary file
        const char* temp_filename = "/tmp/yyjson_test.json";
        
        // Write test data to file
        FILE* write_fp = fopen(temp_filename, "w");
        COUT(write_fp != nullptr, true);
        
        if (write_fp)
        {
            yyjson::Document doc;
            doc << R"json({"file_test": "FILE*", "number": 789})json";
            doc.write(write_fp);
            fclose(write_fp);
        }
        
        // Read from file using FILE*
        FILE* read_fp = fopen(temp_filename, "r");
        COUT(read_fp != nullptr, true);
        
        if (read_fp)
        {
            yyjson::Document doc;
            doc.read(read_fp);
            fclose(read_fp);
            
            COUT(doc.hasError(), false);
            COUT(doc / "file_test" | std::string(), "FILE*");
            COUT(doc / "number" | 0, 789);
        }
        
        // Clean up
        remove(temp_filename);
    }
    
    DESC("Test std::ifstream and std::ofstream operations");
    {
        const char* temp_filename = "/tmp/yyjson_stream_test.json";
        
        // Write to file using std::ofstream
        {
            std::ofstream ofs(temp_filename);
            COUT(ofs.is_open(), true);
            
            if (ofs.is_open())
            {
                yyjson::Document doc;
                doc << R"json({"stream_test": "std::fstream", "value": 999})json";
                doc.write(ofs);
                ofs.close();
            }
        }
        
        // Read from file using std::ifstream
        {
            std::ifstream ifs(temp_filename);
            COUT(ifs.is_open(), true);
            
            if (ifs.is_open())
            {
                yyjson::Document doc;
                doc.read(ifs);
                ifs.close();
                
                COUT(doc.hasError(), false);
                COUT(doc / "stream_test" | std::string(), "std::fstream");
                COUT(doc / "value" | 0, 999);
            }
        }
        
        // Test MutableDocument with file streams
        {
            std::ofstream ofs(temp_filename);
            COUT(ofs.is_open(), true);
            
            if (ofs.is_open())
            {
                yyjson::MutableDocument doc;
                doc << R"json({"mutable_stream": true, "count": 111})json";
                doc.write(ofs);
                ofs.close();
            }
        }
        
        {
            std::ifstream ifs(temp_filename);
            COUT(ifs.is_open(), true);
            
            if (ifs.is_open())
            {
                yyjson::MutableDocument doc;
                doc.read(ifs);
                ifs.close();
                
                COUT(doc.hasError(), false);
                COUT(doc / "mutable_stream" | false, true);
                COUT(doc / "count" | 0, 111);
            }
        }
        
        // Clean up
        remove(temp_filename);
    }
    
    DESC("Test stream operators with FILE* and std::fstream");
    {
        const char* temp_filename = "/tmp/yyjson_operator_test.json";
        
        // Test << operator with std::ifstream
        {
            // First create a test file
            std::ofstream ofs(temp_filename);
            ofs << R"json({"operator_test": "success", "id": 12345})json";
            ofs.close();
            
            std::ifstream ifs(temp_filename);
            yyjson::Document doc;
            doc << ifs; // Use << operator with std::ifstream
            
            COUT(doc.hasError(), false);
            COUT(doc / "operator_test" | std::string(), "success");
            COUT(doc / "id" | 0, 12345);
        }
        
        // Test >> operator with std::ofstream
        {
            yyjson::Document doc;
            doc << R"json({"output_test": "works", "number": 54321})json";
            
            std::ofstream ofs(temp_filename);
            doc >> ofs; // Use >> operator with std::ofstream
            ofs.close();
            
            // Verify file content
            std::ifstream ifs(temp_filename);
            std::string content((std::istreambuf_iterator<char>(ifs)), 
                               std::istreambuf_iterator<char>());
            
            COUT(content.find("\"output_test\":\"works\"") != std::string::npos, true);
            COUT(content.find("\"number\":54321") != std::string::npos, true);
        }
        
        // Clean up
        remove(temp_filename);
    }
}

DEF_TAST(yyjson_std_output_stream, "test standard output stream operators <<")
{
    DESC("Test standard output stream operators <<");
    {
        // Test Document standard output stream - try COUT macro directly
        {
            yyjson::Document doc1("{\"message\": \"Hello World\", \"count\": 42}");
            yyjson::Document doc2("{\"message\": \"Hello World\", \"count\": 42}");
            COUT(doc1, doc2);

            std::string output;
            doc1 >> output;
            std::ostringstream oss;
            oss << doc2;
            COUT(oss.str(), output);
        }
        
        // Test Value standard output stream
        {
            yyjson::Document doc("{\"name\": \"Alice\", \"age\": 25}");
            yyjson::Value val1 = doc.root();
            yyjson::Value val2 = doc.root();
            COUT(val1, val2);
        }
        
        // Test MutableDocument standard output stream
        {
            yyjson::MutableDocument doc1("{\"status\": \"active\", \"level\": 3}");
            yyjson::MutableDocument doc2("{\"status\": \"active\", \"level\": 3}");
            
            COUT(doc1, doc2);
        }
        
        // Test MutableValue standard output stream
        {
            yyjson::MutableDocument doc("\"simple string\"");
            yyjson::MutableValue mval1 = doc.root();
            yyjson::MutableValue mval2 = doc.root();
            
            COUT(mval1, mval2);
        }
        
        // Test array values with standard output stream
        {
            yyjson::Document arrayDoc1("[1, 2, \"three\", true]");
            yyjson::Document arrayDoc2("[1, 2, \"three\", true]");
            
            COUT(arrayDoc1, arrayDoc2);
        }
        
        // Test with nested structures
        {
            yyjson::Document nestedDoc1("{\"data\": {\"items\": [1, 2, 3], \"info\": \"test\"}}");
            yyjson::Document nestedDoc2("{\"data\": {\"items\": [1, 2, 3], \"info\": \"test\"}}");
            
            COUT(nestedDoc1, nestedDoc2);
        }
        
        // Test primitive values
        {
            yyjson::Document numDoc1("123");
            yyjson::Document numDoc2("123");
            
            COUT(numDoc1, numDoc2);
            
            yyjson::Document boolDoc1("true");
            yyjson::Document boolDoc2("true");
            
            COUT(boolDoc1, boolDoc2);
        }
        
        // Test different values should not be equal
        {
            yyjson::Document doc1("{\"a\": 1}");
            yyjson::Document doc2("{\"a\": 2}");
            
            // This should fail if COUT macro works correctly with inequality
            COUT(doc1 != doc2, true);
        }
    }
}

DEF_TAST(yyjson_object_insertion, "test object insertion with KV macro and operator+")
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
    
    DESC("Test {} and [] special handling verification");
    {
        yyjson::MutableDocument doc("{}");
        
        // Test {} creates object, not string
        doc.root().addRef("empty_object", "{}");
        COUT((doc / "empty_object").isObject(), true);
        COUT((doc / "empty_object").isString(), false);
        
        // Test [] creates array, not string  
        doc.root().addRef("empty_array", "[]");
        COUT((doc / "empty_array").isArray(), true);
        COUT((doc / "empty_array").isString(), false);
        
        // Test regular strings still work
        doc.root().add("regular_string", "hello");
        COUT((doc / "regular_string").isString(), true);
        COUT(doc / "regular_string" | str, "hello");
        
        // Test append with {} and []
        doc.root().addRef("array_of_objects", "[]");
        doc / "array_of_objects" << "{}";
        doc / "array_of_objects" << "{}";
        
        COUT((doc / "array_of_objects").size(), 2);
        COUT((doc / "array_of_objects" / 0).isObject(), true);
        COUT((doc / "array_of_objects" / 1).isObject(), true);
    }
}

DEF_TAST(yyjson_mutable_create_methods, "test MutableDocument create methods and * operator")
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
    
    DESC("Test string literal optimization in create methods");
    {
        yyjson::MutableDocument doc;
        
        // Test createRef with literal - should use reference optimization
        char literalBuffer[20] = "test_literal"; // Fixed size buffer for safety
        auto refNode = doc.createRef(literalBuffer);
        COUT(refNode.isString(), true);
        COUT(refNode | std::string(), "test_literal");
        
        // Modify the source buffer safely - refNode should reflect the change
        const char* modifiedLiteral = "modified_lit";
        strncpy(literalBuffer, modifiedLiteral, sizeof(literalBuffer));
        literalBuffer[sizeof(literalBuffer) - 1] = '\0';
        COUT(refNode | std::string(), "modified_lit");
        
        // Test create with variable - should use safe copying
        char copyBuffer[20] = "test_copy";
        auto copyNode = doc.create(copyBuffer);
        COUT(copyNode.isString(), true);
        COUT(copyNode | std::string(), "test_copy");
        
        // Modify the source buffer safely - copyNode should NOT change
        const char* modifiedCopy = "modified_cpy";
        strncpy(copyBuffer, modifiedCopy, sizeof(copyBuffer));
        copyBuffer[sizeof(copyBuffer) - 1] = '\0';
        COUT(copyNode | std::string(), "test_copy"); // Should remain original value
        
        // Test std::string with create - should use safe copying
        std::string stdStr = "test_std_string";
        auto stdNode = doc.create(stdStr);
        COUT(stdNode.isString(), true);
        COUT(stdNode | std::string(), "test_std_string");
        
        // Modify the source string - stdNode should NOT change
        stdStr = "modified_std";
        COUT(stdNode | std::string(), "test_std_string"); // Should remain original value
    }
    
    DESC("Test string literal optimization with * operator");
    {
        yyjson::MutableDocument doc;
        
        // Test * operator with literal - should automatically use reference optimization
        const char literalBuffer[20] = "auto_literal";
        auto literalNode = doc * literalBuffer;
        COUT(literalNode.isString(), true);
        COUT(literalNode | std::string(), "auto_literal");
        
        // Modify source safely - literalNode should reflect changes
        const char* modifiedAuto = "auto_modified";
        strncpy(const_cast<char*>(literalBuffer), modifiedAuto, sizeof(literalBuffer));
//      literalBuffer[sizeof(literalBuffer) - 1] = '\0';
        COUT(literalNode | std::string(), "auto_modified");
        
        // Test * operator with variable pointer - should use safe copying
        char variableBuffer[20] = "variable_string";
        const char* variableStr = variableBuffer;
        auto variableNode = doc * variableStr;
        COUT(variableNode.isString(), true);
        COUT(variableNode | std::string(), "variable_string");
        auto variableNode2 = doc * variableBuffer;
        COUT(variableNode2.isString(), true);
        COUT(variableNode2 | std::string(), "variable_string");
        
        // Modify source safely - variableNode should NOT change
        const char* modifiedVar = "var_modified";
        strncpy(variableBuffer, modifiedVar, sizeof(variableBuffer));
        variableBuffer[sizeof(variableBuffer) - 1] = '\0';
        COUT(variableNode | std::string(), "variable_string"); // Should remain original
        COUT(variableNode2 | std::string(), "variable_string"); // Should remain original
        
        // Test * operator with std::string - should use safe copying
        std::string strVar = "std_var_string";
        auto strNode = doc * strVar;
        COUT(strNode.isString(), true);
        COUT(strNode | std::string(), "std_var_string");
        
        // Modify source - strNode should NOT change
        strVar = "std_modified";
        COUT(strNode | std::string(), "std_var_string"); // Should remain original
    }
}

DEF_TAST(yyjson_array_append, "test append to MutableValue array")
{
    DESC("Test appending MutableValue to arrays");
    {
        yyjson::MutableDocument doc("[]");
        
        // Create nodes using create methods
        auto intNode = doc.create(42);
        auto stringNode = doc.create("hello");
        auto boolNode = doc.create(true);
        
        // Append the created nodes to the array
        doc.root().append(intNode);
        doc.root().append(stringNode);
        doc.root().append(boolNode);
        
        // Verify the array contents
        COUT(doc.root().size(), 3);
        COUT(doc.root()[0] | 0, 42);
        COUT(doc.root()[1] | std::string(), "hello");
        COUT(doc.root()[2] | false, true);

        doc.root().append(nullptr);
        COUT(doc.root().size(), 4);
        COUT(doc.root()[3].isNull(), true);
    }
    
    DESC("Test inserting MutableValue into objects");
    {
        yyjson::MutableDocument doc("{}");
        
        // Create nodes using create methods
        auto intNode = doc.create(100);
        auto stringNode = doc.create("world");
        auto boolNode = doc.create(false);
        
        // Insert the created nodes into the object
        doc.root().add("number", intNode);
        doc.root().add("text", stringNode);
        doc.root().add("flag", boolNode);
        
        // Verify the object contents
        COUT(doc.root().size(), 3);
        COUT(doc.root()["number"] | 0, 100);
        COUT(doc.root()["text"] | std::string(), "world");
        COUT(doc.root()["flag"] | true, false);
    }
    
    DESC("Test chained operations with create and append/insert");
    {
        yyjson::MutableDocument doc("[]");
        
        // Create and append in one chain
        doc.root().append(doc.create(1))
                 .append(doc.create("two"))
                 .append(doc.create(3.0));
        
        COUT(doc.root().size(), 3);
        COUT(doc.root()[0] | 0, 1);
        COUT(doc.root()[1] | std::string(), "two");
        COUT(doc.root()[2] | 0.0, 3.0);
        
        // Test with * operator
        doc.root().append(doc * 4)
                 .append(doc * "five");
        
        COUT(doc.root().size(), 5);
        COUT(doc.root()[3] | 0, 4);
        COUT(doc.root()[4] | std::string(), "five");
    }
    
    DESC("Test mixed primitive and MutableValue operations");
    {
        yyjson::MutableDocument doc("[]");
        
        // Mix primitive appends with MutableValue appends
        doc.root().append(1)                    // primitive int
                 .append(doc.create("two"))    // MutableValue
                 .append(3.0)                  // primitive double
                 .append(doc.create(false));   // MutableValue
        
        COUT(doc.root().size(), 4);
        COUT(doc.root()[0] | 0, 1);
        COUT(doc.root()[1] | std::string(), "two");
        COUT(doc.root()[2] | 0.0, 3.0);
        COUT(doc.root()[3] | true, false);
    }
    
    DESC("Test mixed operator");
    {
        yyjson::MutableDocument doc("[]");
        
        +doc << 1 << doc*"two" << 3.0 << doc*false;
        
        COUT(doc.root().size(), 4);
        COUT(doc/0 | 0, 1);
        COUT(doc/1 | std::string(), "two");
        COUT(doc/2 | 0.0, 3.0);
        COUT(doc/3 | true, false);
    }
}

DEF_TAST(yyjson_keyvalue_optimization, "test KeyValue optimization for object insertion")
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

DEF_TAST(yyjson_index_operator, "test index method and operator[]")
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

    COUT(doc / "nested/arr/1" | 0, 20);
    COUT(doc / "nested/arr/2" | 0, 30);
    COUT(doc / "nested/obj/a" | 0, 100);
    COUT(doc / "nested/obj/b" | 0, 200);
    COUT(doc / "/nested/obj/b" | 0, 200);

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
//= mutObj.add("test_key", mutDoc.create("test_value"));
//= mutObj.add("number_key", mutDoc.create(42));
    mutObj["test_key"] = "test_value";
    mutObj["number_key"] = 42;

    // the next assignment not work!
//! mutObj["test_key"] = mutDoc.create("test_value");
    
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

DEF_TAST(yyjson_file_operations, "test file read/write operations for Document and MutableDocument")
{
    // Create a test JSON content
    std::string testJson = R"json({
        "name": "test",
        "value": 42,
        "active": true,
        "items": [1, 2, 3],
        "settings": {
            "enabled": false,
            "threshold": 5.5
        }
    })json";
    
    const char* testFilePath = "/tmp/test_json_file.json";
    const char* testFileMutPath = "/tmp/test_json_file_mut.json";
    
    DESC("Test Document file operations");
    {
        // Create original document from string
        yyjson::Document originalDoc(testJson);
        COUT(originalDoc.hasError(), false);
        
        // Write to file
        COUT(originalDoc.writeFile(testFilePath), true);
        
        // Read from file
        yyjson::Document readDoc;
        COUT(readDoc.readFile(testFilePath), true);
        COUT(readDoc.hasError(), false);
        
        
        // Verify content matches
        COUT(readDoc / "name" | std::string(), "test");
        COUT(readDoc / "value" | 0, 42);
        COUT(readDoc / "active" | false, true);
        COUT(readDoc / "items" / 0 | 0, 1);
        COUT(readDoc / "settings" / "threshold" | 0.0, 5.5);
        
        // Test write to string for comparison
        std::string originalStr, readStr;
        originalDoc.write(originalStr);
        readDoc.write(readStr);
        COUT(originalStr == readStr, true);
    }
    
    DESC("Test MutableDocument file operations");
    {
        // Create original mutable document from string
        yyjson::MutableDocument originalMutDoc(testJson);
        COUT(originalMutDoc.hasError(), false);
        
        // Modify some values
        originalMutDoc / "value" = 100;
        originalMutDoc / "active" = false;
        originalMutDoc / "settings" / "enabled" = true;
        originalMutDoc / "items" << 4;
        
        // Write to file
        COUT(originalMutDoc.writeFile(testFileMutPath), true);
        
        // Read from file
        yyjson::MutableDocument readMutDoc;
        COUT(readMutDoc.readFile(testFileMutPath), true);
        COUT(readMutDoc.hasError(), false);
        
        // Verify modified content
        COUT(readMutDoc / "value" | 0, 100);
        COUT(readMutDoc / "active" | true, false);
        COUT(readMutDoc / "settings" / "enabled" | false, true);
        COUT(readMutDoc / "items" / 3 | 0, 4);
        COUT((readMutDoc / "items").size(), 4);
        
        // Test write to string for comparison
        std::string originalStr, readStr;
        originalMutDoc.write(originalStr);
        readMutDoc.write(readStr);
        COUT(originalStr == readStr, true);
    }
    
    DESC("Test error handling for file operations");
    {
        yyjson::Document doc;
        yyjson::MutableDocument mutDoc;
        
        // Test reading non-existent file
        COUT(doc.readFile("/non/existent/path.json"), false);
        COUT(mutDoc.readFile("/non/existent/path.json"), false);
        
        // Test writing to invalid path
        COUT(doc.writeFile("/invalid/path/"), false);
        COUT(mutDoc.writeFile("/invalid/path/"), false);
        
        // Test with nullptr paths
        COUT(doc.readFile(nullptr), false);
        COUT(mutDoc.readFile(nullptr), false);
        COUT(doc.writeFile(nullptr), false);
        COUT(mutDoc.writeFile(nullptr), false);
    }
    
    // Clean up test files
    remove(testFilePath);
    remove(testFileMutPath);
}

DEF_TAST(yyjson_iterator_functionality, "test array and object iterator functionality")
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

DEF_TAST(yyjson_iterator_operators, "test iterator operators and % operator")
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

DEF_TAST(yyjson_iterator_begin_end, "test standard begin/end iterator pattern")
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

DEF_TAST(yyjson_mutable_iterator_begin_end, "test mutable standard begin/end iterator pattern")
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

DEF_TAST(yyjson_conversion_methods, "test toString() and toNumber() conversion methods")
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

        // Test string values
        COUT((doc / "string_value").toString(), "hello");
        COUT((doc / "string_value").toString(true), "\"hello\""); // with quotes
        COUT(-(doc / "string_value"), "hello"); // operator-

        // Test integer values
        COUT((doc / "int_value").toString(), "42");
        COUT((doc / "int_value").toNumber(), 42);
        COUT(+(doc / "int_value"), 42); // operator+

        // Test negative integer values
        COUT((doc / "negative_value").toString(), "-15");
        COUT((doc / "negative_value").toNumber(), -15);
        COUT(+(doc / "negative_value"), -15);

        // Test double values
        COUT((doc / "double_value").toString(), "3.14");
        COUT((doc / "double_value").toNumber(), 3); // should truncate to int
        COUT(+(doc / "double_value"), 3);

        // Test boolean values
        COUT((doc / "bool_true").toString(), "true");
        COUT((doc / "bool_true").toNumber(), 1);
        COUT(+(doc / "bool_true"), 1);

        COUT((doc / "bool_false").toString(), "false");
        COUT((doc / "bool_false").toNumber(), 0);
        COUT(+(doc / "bool_false"), 0);

        // Test null values
        COUT((doc / "null_value").toString(), "null"); // null literal
        COUT((doc / "null_value").toNumber(), 0); // 0 for null
        COUT(+(doc / "null_value"), 0);

        // Test arrays (should use size for toNumber)
        COUT((doc / "array_value").toString().find("1,2,3") != std::string::npos, true);
        COUT((doc / "array_value").toNumber(), 3); // array size
        COUT(+(doc / "array_value"), 3);
        COUT(-(doc / "array_value"), "[1,2,3]");

        // Test objects (should use size for toNumber)
        COUT((doc / "object_value").toString().find("key") != std::string::npos, true);
        COUT((doc / "object_value").toNumber(), 1); // object size
        COUT(+(doc / "object_value"), 1);
        COUT(-(doc / "object_value"), R"({"key":"value"})");

        // Test empty arrays and objects
        COUT((doc / "empty_array").toNumber(), 0);
        COUT((doc / "empty_object").toNumber(), 0);

        // Test invalid values
        yyjson::Value invalidValue;
        COUT(invalidValue.toString(), "");
        COUT(invalidValue.toNumber(), 0);
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
        COUT((doc / "int_value").toNumber(), 42);
        COUT(+(doc / "int_value"), 42);

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

        // Test string to number conversion (should use std::stoi)
        yyjson::Document doc2(R"json({"numeric_string": "123", "invalid_string": "abc"})json");

        COUT(+(doc2 / "numeric_string"), 123); // string "123" should convert to 123
        COUT(+(doc2 / "invalid_string"), 0); // invalid string should return 0
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

DEF_TAST(yyjson_conversion_operators, "test unary operators - and + for conversion")
{
    std::string jsonText = R"json({
        "string": "test",
        "number": 99,
        "float": 7.5,
        "bool": true
    })json";

    DESC("Test unary operators with Value");
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

        // toNumber() should be equivalent to operator+
        COUT((doc / "number").toNumber(), +(doc / "number"));
        COUT((doc / "float").toNumber(), +(doc / "float"));

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

        doc.read("{\"str_large\": \"999999999\"}");
        COUT(+ (doc / "str_large"), 999999999); // large number
    }
}

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

DEF_TAST(yyjson_pipe_functionality, "test pipe() method and | operator with functions")
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

DEF_TAST(yyjson_append_doc, "MutableValue array append with Document and MutableDocument")
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

DEF_TAST(yyjson_objadd_doc, "MutableValue object add with Document and MutableDocument")
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

DEF_TAST(yyjson_document_conversion, "test Document and MutableDocument conversion methods")
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


DEF_TAST(yyjson_comparison_operators, "test comparison operators")
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

DEF_TAST(yyjson_type_traits, "test type traits for yyjson wrapper classes")
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
