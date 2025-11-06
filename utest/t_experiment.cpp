/**
 * @file t_experiment.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief experimental test code for xyjson
 */
#include "couttast/couttast.h"
#include "xyjson.h"

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

//! this experiment almost fail: first match const char* then const char&[N]
DEF_TAST(experiment_overload, "test overload rule")
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
//  COUT(overload.baz(buffer), 2); //! ambiguous to construct StringRef or StringCopy
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

DEF_TAST(experiment_docx, "test operator usage")
{
    std::string jsonText = R"({"name": "Alice", "age": 30})";
    yyjson::Document doc(jsonText);

    auto nameNode = doc["name"];
    COUT(nameNode | "", "Alice");
    COUT(doc["age"] | 0, 30);

#ifndef XYJSON_DISABLE_MUTABLE
    auto mutDoc = ~doc;
    auto age = mutDoc / "age";
    COUT(age & 0, true);
    COUT(age & uint64_t(0), true);
    COUT(age & int64_t(0), false);

    age = int64_t(3000);
    COUT(age & 0, true);
    COUT(age & uint64_t(0), false);
    COUT(age & int64_t(0), true);
#endif
}

#if 0
DEF_TAST(experiment_debug, "test debug")
{
    using namespace yyjson;
//  COUT(std::is_invocable<int, int>::value);
    COUT(trait::is_callable_type<int>::value, false);
//!  COUT(::strlen(nullptr), 0); // coredump
}
#endif

DEF_TAST(experiment_large_int, "test yyjson large integer support")
{
    // This test explores the behavior of yyjson's C API for large integers.
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    int64_t sint_val = -1234567890123456789;
    uint64_t uint_val = 12345678901234567890ULL;

    yyjson_mut_obj_add_sint(doc, root, "sint", sint_val);
    yyjson_mut_obj_add_uint(doc, root, "uint", uint_val);
    {
        yyjson_mut_val* sint_node = yyjson_mut_obj_get(root, "sint");
        COUT(yyjson_mut_is_sint(sint_node), true);
        COUT(yyjson_mut_is_uint(sint_node), false);
        COUT(yyjson_mut_is_int(sint_node), true);

        yyjson_mut_val* uint_node = yyjson_mut_obj_get(root, "uint");
        COUT(yyjson_mut_is_sint(uint_node), false);
        COUT(yyjson_mut_is_uint(uint_node), true);
        COUT(yyjson_mut_is_int(uint_node), true);
    }

    const char* json_str = yyjson_mut_write(doc, 0, NULL);
    yyjson_doc* read_doc = yyjson_read(json_str, strlen(json_str), 0);
    free((void*)json_str);

    DESC("Test SINT node (-1234567890123456789)");
    yyjson_val* sint_node = yyjson_obj_get(yyjson_doc_get_root(read_doc), "sint");
    // Analysis: is_int() is true for sint, but get_int() will likely result in overflow/truncation.
    COUT(yyjson_is_sint(sint_node), true);
    COUT(yyjson_is_uint(sint_node), false);
    COUT(yyjson_is_int(sint_node), true); // is_int is a general check for any integer subtype.
    COUT(yyjson_get_sint(sint_node), sint_val);
    DESC("Value from get_int() on a SINT node (expect overflow):");
    COUT(yyjson_get_int(sint_node));
    COUT(yyjson_get_uint(sint_node));

    DESC("Test UINT node (12345678901234567890)");
    yyjson_val* uint_node = yyjson_obj_get(yyjson_doc_get_root(read_doc), "uint");
    // Analysis: is_int() is true for uint, but get_int() will also overflow.
    COUT(yyjson_is_uint(uint_node), true);
    COUT(yyjson_is_sint(uint_node), false);
    COUT(yyjson_is_int(uint_node), true);
    COUT(yyjson_get_uint(uint_node), uint_val);
    DESC("Value from get_int() on a UINT node (expect overflow):");
    COUT(yyjson_get_int(uint_node));
    COUT(yyjson_get_sint(uint_node));

    yyjson_mut_doc_free(doc);
    yyjson_doc_free(read_doc);
}
