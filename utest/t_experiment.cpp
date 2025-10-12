/**
 * @file t_experiment.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief experimental test code for xyjson
 * */
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