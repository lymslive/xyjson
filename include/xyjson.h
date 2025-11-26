/**
 * @file xyjson.h
 * @author lymslive
 * @date 2025-09-15 / 2025-11-24
 * @version v1.0.0
 *
 * @brief C++ wrapper for yyjson library with advanced operator overloading
 *
 * @details
 * This header provides a comprehensive C++ wrapper for the yyjson library,
 * featuring extensive operator overloading for intuitive JSON manipulation.
 * 
 * Key features include:
 * - Read-only and mutable JSON models
 * - Path access using '/' operator
 * - Value extraction using '|' operator
 * - Assignment and input operators for mutable JSON
 * - Iterator support with standard C++ interface
 * - Template metaprogramming for type safety
 * 
 * The design prioritizes both performance and usability, maintaining
 * compatibility with the underlying yyjson library while providing
 * a more natural C++ interface.
 *
 * @note For best performance, use the read-only models when modification
 *       is not required. Mutable operations involve additional memory
 *       management overhead.
 *
 * @see yyjson library documentation for underlying implementation details
 */
#ifndef XYJSON_H__
#define XYJSON_H__

#include <string>
#include <type_traits>
#include <fstream>
#include <cstdio>

#include <yyjson.h>
#include <functional>

/* Table of Content (TOC) */
/* ======================================================================== */
/* Part 1: Front Definitions                                       Line: 97 */
/*   Section 1.1: Conditional Compilation Macros                  Line: 100 */
/*   Section 1.2: Forward Class Declarations                      Line: 124 */
/*   Section 1.3: Type and Operator Constants                     Line: 152 */
/*   Section 1.4: Type Traits                                     Line: 195 */
/* Part 2: Class Definitions                                      Line: 342 */
/*   Section 2.1: Read-only Json Model                            Line: 345 */
/*   Section 2.2: Mutable Json Model                              Line: 630 */
/*   Section 2.3: Helper Class for Mutable Json                  Line: 1050 */
/*   Section 2.4: Iterator for Json Container                    Line: 1117 */
/*   Section 2.5: Container-specific Wrapper Classes             Line: 1465 */
/* Part 3: Non-Class Functions                                   Line: 1535 */
/*   Section 3.1: Underlying mut_val Creation                    Line: 1548 */
/*   Section 3.2: Conversion Helper Functions                    Line: 1758 */
/* Part 4: Class Implementations                                 Line: 1916 */
/*   Section 4.1: Value Methods                                  Line: 1919 */
/*     Group 4.1.1: get and getor                                Line: 1922 */
/*     Group 4.1.2: size and index/path                          Line: 2028 */
/*     Group 4.1.3: create iterator                              Line: 2061 */
/*     Group 4.1.4: others                                       Line: 2129 */
/*   Section 4.2: Document Methods                               Line: 2179 */
/*     Group 4.2.1: primary manage                               Line: 2182 */
/*     Group 4.2.2: read and write                               Line: 2224 */
/*   Section 4.3: MutableValue Methods                           Line: 2318 */
/*     Group 4.3.1: get and getor                                Line: 2321 */
/*     Group 4.3.2: size and index/path                          Line: 2433 */
/*     Group 4.3.3: assignment set                               Line: 2507 */
/*     Group 4.3.4: array and object add                         Line: 2629 */
/*     Group 4.3.5: smart input and tag                          Line: 2675 */
/*     Group 4.3.6: create iterator                              Line: 2779 */
/*     Group 4.3.7: others                                       Line: 2847 */
/*   Section 4.4: MutableDocument Methods                        Line: 2897 */
/*     Group 4.4.1: primary manage                               Line: 2900 */
/*     Group 4.4.2: read and write                               Line: 2952 */
/*     Group 4.4.3: create mutable value                         Line: 3057 */
/*   Section 4.5: ArrayIterator Methods                          Line: 3082 */
/*   Section 4.6: ObjectIterator Methods                         Line: 3125 */
/*   Section 4.7: MutableArrayIterator Methods                   Line: 3189 */
/*   Section 4.8: MutableObjectIterator Methods                  Line: 3309 */
/* Part 5: Operator Interface                                    Line: 3532 */
/*   Section 5.1: Primary Path Access                            Line: 3535 */
/*   Section 5.2: Conversion Unary Operator                      Line: 3642 */
/*   Section 5.3: Comparison Operator                            Line: 3690 */
/*   Section 5.4: Create and Bind KeyValue                       Line: 3776 */
/*   Section 5.5: Stream and Input Operator                      Line: 3808 */
/*   Section 5.6: Iterator Creation and Operation                Line: 3879 */
/*   Section 5.7: Document Forward Root Operator                 Line: 4054 */
/*   Section 5.8: User-defined Literal Operator                  Line: 4119 */
/* Part 6: Last Definitions                                      Line: 4131 */
/* ======================================================================== */

namespace yyjson
{

/* @Part 1: Front Definitions */
/* ======================================================================== */

/* @Section 1.1: Conditional Compilation Macros */
/* ------------------------------------------------------------------------ */

/**
 * @brief Disable chained input operators (<<)
 * 
 * When defined, disables chained input syntax like "json << key << value".
 * This reduces sizeof MutableValue and improve efficiency.
 * 
 * @note Chained input is only available for mutable JSON objects.
 */
#ifndef XYJSON_DISABLE_CHAINED_INPUT
#endif

/**
 * @brief Disable all mutable JSON features
 * 
 * When defined, completely disables mutable JSON functionality, including
 * MutableValue, MutableDocument, and all write operations. This significantly
 * reduces binary size when only read-only JSON processing is needed.
 */
#ifndef XYJSON_DISABLE_MUTABLE
#endif

/* @Section 1.2: Forward Class Declarations */
/* ------------------------------------------------------------------------ */
/**
 * @brief Forward declarations of all major classes in the library
 * 
 * These declarations provide the basic class structure without exposing
 * implementation details, enabling proper dependency management and
 * compilation efficiency.
 */

class Value;           //< Read-only json node proxy
class Document;        //< Read-only json document tree
class MutableValue;    //< Mutable json node proxy
class MutableDocument; //< Mutable json document tree

class ArrayIterator;         //< Iterator for json array of Value type
class ObjectIterator;        //< Iterator for json object of Value type
class MutableArrayIterator;  //< Iterator for json array of MutableValue type
class MutableObjectIterator; //< Iterator for json object of MutableValue type

class KeyValue;  //< Helper type to insert/remove key-value pair in object
class StringRef; //< Helper type to optimized literal reference

class ConstArray;    //< Specialized array in Value type
class ConstObject;   //< Specialized object in Value type
class MutableArray;  //< Specialized array in MutableValue type
class MutableObject; //< Specialized object in MutableValue type

/* @Section 1.3: Type and Operator Constants */
/* ------------------------------------------------------------------------ */

// Special representation type
struct ZeroNumber {};  //< Type flag for json number
struct EmptyString {}; //< Type flag for json string
struct EmptyArray {};  //< Type flag for json array
struct EmptyObject {}; //< Type flag for json object

/// Type representative constants in operand.
constexpr std::nullptr_t kNull = nullptr;
constexpr bool kBool = false;
constexpr int kInt = 0;
constexpr int64_t kSint = 0L;
constexpr uint64_t kUint = 0uL;
constexpr double kReal = 0.0;
constexpr ZeroNumber kNumber;
constexpr EmptyString kString;
constexpr EmptyArray kArray;
constexpr EmptyObject kObject;

// Type representative constants for underlying pointers
constexpr yyjson_val* kNode = nullptr;
constexpr yyjson_mut_val* kMutNode = nullptr;
constexpr yyjson_mut_doc* kMutDoc = nullptr;

// Operator name constants (ok stands for operator const)
constexpr const char* okExtract = "|";   //< extract scalar from json node
constexpr const char* okPipe = "|";      //< pipe json node to custome function
constexpr const char* okType = "&";      //< check json type
constexpr const char* okPath = "/";      //< access json node by path
constexpr const char* okIndex = "[]";    //< access json node by single index
constexpr const char* okAssign = "=";    //< modify scalar json node
constexpr const char* okInput = "<<";    //< read in json document
constexpr const char* okOutput = ">>";   //< write out json document
constexpr const char* okPush = "<<";     //< push to json container at end
constexpr const char* okPop = ">>";      //< pop from json container at end
constexpr const char* okInsert = "<<";   //< insert to json container iterator
constexpr const char* okRemove = ">>";   //< remove from json container iterator
constexpr const char* okEqual = "==";    //< check equality
constexpr const char* okCreate = "*";    //< create mutable json node
constexpr const char* okRoot = "*";      //< access root node from document
constexpr const char* okBind = "*";      //< bind mutable json node with key
constexpr const char* okNumberify = "+"; //< cast any json to interger
constexpr const char* okStringify = "-"; //< cast any json to string
constexpr const char* okConvert = "~";   //< convert between (Mutable)Document
constexpr const char* okIterator = "%";  //< create json iterator
constexpr const char* okIncreace = "++"; //< iterator next forward
constexpr const char* okDecreace = "--"; //< iterator prev backward

/* @Section 1.4: Type Traits */
/* ------------------------------------------------------------------------ */

/**
 * @brief Type traits and metaprogramming utilities
 * 
 * This namespace contains type traits used for template metaprogramming,
 * SFINAE techniques, and compile-time type checking throughout the library.
 */
namespace trait
{

/**
 * @brief Check if type T is a JSON value wrapper
 * 
 * Specialized for both Value and MutableValue types.
 */
template<typename T> struct is_value : std::false_type {};
template<> struct is_value<Value> : std::true_type {};
template<> struct is_value<MutableValue> : std::true_type {};

/**
 * @brief Check if type T is a JSON document wrapper
 * 
 * Specialized for both Document and MutableDocument types.
 */
template<typename T> struct is_document : std::false_type {};
template<> struct is_document<Document> : std::true_type {};
template<> struct is_document<MutableDocument> : std::true_type {};

/**
 * @brief Check if type T is a JSON iterator
 * 
 * Specialized for all iterator types (both read-only and mutable).
 */
template<typename T> struct is_iterator : std::false_type {};
template<> struct is_iterator<ArrayIterator> : std::true_type {};
template<> struct is_iterator<ObjectIterator> : std::true_type {};
template<> struct is_iterator<MutableArrayIterator> : std::true_type {};
template<> struct is_iterator<MutableObjectIterator> : std::true_type {};

/**
 * @brief Check if type T can be used as an object key
 * 
 * Valid key types include C-style strings, std::string, and their
 * various decayed forms (const char[N], char[N], etc.).
 */
template<typename T> struct is_key : std::false_type {};
template<> struct is_key<const char*> : std::true_type {}; // includes decayed const char[N]
template<> struct is_key<char*> : std::true_type {};       // includes decayed char[N]
template<> struct is_key<std::string> : std::true_type {};

template<typename T>
constexpr bool is_key_v = is_key<std::decay_t<T>>::value;

/**
 * @brief Check if type T is a C-style string
 * 
 * Handles both const char* and char* types, including array decay.
 */
template<typename T>
constexpr bool is_cstr_type() {
    using decayed_t = std::decay_t<T>;
    return std::is_same<decayed_t, const char*>::value || std::is_same<decayed_t, char*>::value;
}

/**
 * @brief Check if type T is a supported scalar type
 * 
 * Scalar types are the basic JSON value types that can be extracted
 * from JSON values using get() and getor() methods.
 */
template<typename T> struct is_scalar : std::false_type {};
template<> struct is_scalar<bool> : std::true_type {};
template<> struct is_scalar<int> : std::true_type {};
template<> struct is_scalar<int64_t> : std::true_type {};
template<> struct is_scalar<uint64_t> : std::true_type {};
template<> struct is_scalar<double> : std::true_type {};
template<> struct is_scalar<const char*> : std::true_type {};
template<> struct is_scalar<std::string> : std::true_type {};

template<typename T>
constexpr bool is_scalar_v = is_scalar<std::decay_t<T>>::value;

/**
 * @brief Check if type T is eligible for getor(default) operations
 * 
 * This trait determines which types can be used as default values
 * in the getor() method, including all scalar types, pointer types,
 * and special empty types.
 */
template<typename T>
struct enable_getor : std::bool_constant<
    is_scalar<std::decay_t<T>>::value ||
    std::is_same<std::decay_t<T>, yyjson_val*>::value ||
    std::is_same<std::decay_t<T>, yyjson_mut_val*>::value ||
    std::is_same<std::decay_t<T>, yyjson_mut_doc*>::value ||
    std::is_same<std::decay_t<T>, std::nullptr_t>::value ||
    std::is_same<std::decay_t<T>, EmptyString>::value ||
    std::is_same<std::decay_t<T>, ZeroNumber>::value ||
    std::is_same<std::decay_t<T>, EmptyArray>::value ||
    std::is_same<std::decay_t<T>, EmptyObject>::value
> {};

template<typename T>
constexpr bool enable_getor_v = enable_getor<T>::value;

/**
 * @brief Check if function type can be called with supported scalar types
 * 
 * Used for pipe() operations to determine if a callable can accept
 * JSON-extracted scalar values.
 */
template<typename funcT>
struct is_callable_with_scalar {
    static constexpr bool value =
        std::is_invocable_v<funcT, const char*> ||
        std::is_invocable_v<funcT, std::string> ||
        std::is_invocable_v<funcT, int> ||
        std::is_invocable_v<funcT, double> ||
        std::is_invocable_v<funcT, int64_t> ||
        std::is_invocable_v<funcT, uint64_t> ||
        std::is_invocable_v<funcT, bool>;
};

template<typename funcT>
constexpr bool is_callable_with_scalar_v = is_callable_with_scalar<funcT>::value;

/**
 * @brief Detect callable types (lambda, std::function, function pointer)
 * 
 * Generic callable detection that works with various function-like types
 * including lambdas, function objects, and function pointers.
 */
template<typename F>
struct is_callable_type : std::bool_constant<
    std::is_class<std::decay_t<F>>::value ||
    std::is_function<std::remove_pointer_t<std::decay_t<F>>>::value ||
    (std::is_pointer<std::decay_t<F>>::value &&
     std::is_function<std::remove_pointer_t<std::decay_t<F>>>::value)
> {};

template<typename F>
constexpr bool is_callable_type_v = is_callable_type<F>::value;

} /* end of namespace yyjson::trait */

/* @Part 2: Class Definitions */
/* ======================================================================== */

/* @Section 2.1: Read-only Json Model */
/* ------------------------------------------------------------------------ */

/**
 * @brief Read-only JSON value wrapper with comprehensive operator overloading
 * 
 * This class provides a read-only interface to yyjson_val pointers with
 * extensive operator overloading for intuitive JSON manipulation. It serves
 * as the foundation for read-only JSON operations throughout the library.
 * 
 * @note This wrapper maintains const-correctness - all operations are
 *       read-only. Use MutableValue for write operations.
 * 
 * @see MutableValue for the writable counterpart
 * @see Document for document-level operations
 * 
 * Supported operators:
 * - Access: json[index] (index access), json / path (path access)
 * - Extraction: json | defaultValue (value extraction), dest |= json (assignment)
 * - Pipe: json | func (custom transformation)
 * - Type: json & type (check type)
 * - Scalar output: json >> value
 * - Stream output: os << json
 * - Boolean: !json, bool(json) (error checking)
 * - Comparison: ==, != (calls equal())
 * - Unary: +json (convert to number), -json (convert to string)  
 * - Iterator: json % index (array), json % key (object)
 */
class Value
{
public:
    using array_iterator = ArrayIterator;
    using object_iterator = ObjectIterator;
    
    Value() : m_val(nullptr) {}
    explicit Value(yyjson_val* val) : m_val(val) {}
    
    // Access underlying yyjson value. yyjson api use non-const pointer.
    yyjson_val* get() const { return m_val; }
    
    // Set underlying yyjson value pointer directly
    void set(yyjson_val* val) { m_val = val; }
    
    // Error checking
    bool isValid() const { return m_val != nullptr; }
    bool hasError() const { return !isValid(); }
    bool operator!() const { return hasError(); }
    explicit operator bool() const { return isValid(); }

    // Type checking
    bool isNull() const { return yyjson_is_null(m_val); }
    bool isBool() const { return yyjson_is_bool(m_val); }
    bool isNumber() const { return yyjson_is_num(m_val); }
    bool isInt() const { return yyjson_is_int(m_val); }
    bool isSint() const { return yyjson_is_sint(m_val); }
    bool isUint() const { return yyjson_is_uint(m_val); }
    bool isReal() const { return yyjson_is_real(m_val); }
    bool isString() const { return yyjson_is_str(m_val); }
    bool isArray() const { return yyjson_is_arr(m_val); }
    bool isObject() const { return yyjson_is_obj(m_val); }
    const char* typeName() const { return yyjson_get_type_desc(m_val); }
    yyjson_type getType() const { return yyjson_get_type(m_val); }
    
    // Type checking methods with template parameters
    bool isType(int) const { return isInt(); }
    bool isType(int64_t) const { return isSint(); }
    bool isType(uint64_t) const { return isUint(); }
    bool isType(double) const { return isReal(); }
    bool isType(bool) const { return isBool(); }
    bool isType(std::nullptr_t) const { return isNull(); }
    bool isType(const std::string& type) const { return isString(); }
    bool isType(ZeroNumber) const { return isNumber(); }
    bool isType(EmptyString) const { return isString(); }
    bool isType(EmptyArray) const { return isArray(); }
    bool isType(EmptyObject) const { return isObject(); }
    bool isType(const char* type) const { 
        if (yyjson_likely(type != nullptr)) {
            if (::strcmp(type, "{}") == 0) return isObject();
            if (::strcmp(type, "[]") == 0) return isArray();
        }
        return isString(); 
    }
    
    bool isType(yyjson_val*) const { return isValid(); }
    
    // Value extraction with result reference
    bool get(bool& result) const;
    bool get(int& result) const;
    bool get(int64_t& result) const;
    bool get(uint64_t& result) const;
    bool get(double& result) const;
    bool get(const char*& result) const;
    bool get(std::string& result) const;
    bool get(yyjson_val*& result) const;
    
    // Convenience method template with default value
    // Literals const char&[N] decay to const char*
    template<typename T>
    std::decay_t<T> getor(T&& defaultValue) const;

    // Special overloads for sentinels
    const char* getor(EmptyString) const;
    double getor(ZeroNumber) const;
    ConstArray getor(EmptyArray) const;
    ConstObject getor(EmptyObject) const;
    
    // Pipe function for custom transformations
    template<typename funcT>
    auto pipe(funcT&& func) const;

    // Array/Object size and access by index or key
    size_t size() const;
    Value index(size_t idx) const;
    Value index(int idx) const { return index(static_cast<size_t>(idx)); }

    Value index(const char* key, size_t len) const;
    Value index(const std::string& key) const {
        return index(key.c_str(), key.size());
    }
    template <size_t N>
    Value index(const char(&key)[N]) const { return index(key, N-1); }
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), Value>::type
    index(T key) const { return index(key, key ? ::strlen(key) : 0); }
    
    // Array and object access operator
    template <typename T>
    Value operator[](T&& index) const {
        return this->index(std::forward<T>(index));
    }
    
    // Path operations. 
    // Like index in the simplest case, but can resolve deep path.
    Value pathto(size_t idx) const { return index(idx); }
    Value pathto(int idx) const { return index(idx); }
    Value pathto(const char* path, size_t len) const;
    Value pathto(const std::string& path) const {
        return pathto(path.c_str(), path.size());
    }
    template <size_t N>
    Value pathto(const char(&path)[N]) const { return pathto(path, N-1); }
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), Value>::type
    pathto(T path) const {
        return pathto(path, path ? ::strlen(path) : 0);
    }
    
    // Unified iterator creation method
    ArrayIterator iterator(size_t startIndex) const;
    ArrayIterator iterator(int startIndex) const;
    ObjectIterator iterator(const char* startKey) const;
    ArrayIterator iterator(EmptyArray) const;
    ObjectIterator iterator(EmptyObject) const;

    // Standard iterator pattern methods
    ArrayIterator beginArray() const;
    ArrayIterator endArray() const;
    ObjectIterator beginObject() const;
    ObjectIterator endObject() const;

    // Container-specific wrapper methods for standard iteration
    ConstArray array() const;
    ConstObject object() const;

    // Match iterator method
    Value* operator->() { return this; }
    yyjson_val* c_val() const { return m_val; }
    
    // Conversion methods
    std::string toString(bool pretty = false) const;
    int toInteger() const;
    double toNumber() const;
    
    // Explicit type conversion operators
    explicit operator int() const { return toInteger(); }
    explicit operator double() const { return toNumber(); }
    explicit operator std::string() const { return toString(); }
    
    // Comparison method
    bool equal(const Value& other) const;
    bool less(const Value& other) const;

private:
    yyjson_val* m_val = nullptr;
};

/**
 * @brief Read-only JSON document wrapper with comprehensive operator support
 * 
 * This class provides document-level operations for read-only JSON data,
 * wrapping yyjson_doc with intuitive operator overloading. It manages
 * the lifecycle of JSON documents and provides root value access.
 * 
 * @note Documents own their underlying yyjson_doc resources and handle
 *       proper cleanup automatically. Copy operations are disabled to
 *       prevent double-free issues.
 * 
 * @see Value for value-level operations
 * @see MutableDocument for writable document operations
 * 
 * Supported operators:
 * - Unary: +doc (convert root to number), -doc (convert root to string), *doc (access root), ~doc (convert to mutable)
 * - Path: doc / "key" (path access), doc[index] (index access)
 * - Stream: doc << input, doc >> output (serialization), os << doc
 * - Boolean: if (doc), !doc (error checking)
 * - Comparison: ==, != (calls root.equal())
 */
class Document
{
public:
    using value_type = Value;
    
    // Constructors.
    Document() : m_doc(nullptr) {}
    explicit Document(yyjson_doc* doc);
    explicit Document(const char* str, size_t len = 0);
    explicit Document(const std::string& str)
        : Document(str.c_str(), str.size()) {}
    
    // Conversion from MutableDocument
#ifndef XYJSON_DISABLE_MUTABLE
    explicit Document(const MutableDocument& other);
#endif
    
    // Free the document
    void free();
    ~Document() { free(); }
    
    // Disable copy operations to prevent double-free issues
    Document(const Document&) = delete;
    Document& operator=(const Document&) = delete;
    
    // Enable move operations
    Document(Document&& other) noexcept : m_doc(other.m_doc) {
        other.m_doc = nullptr;
    }
    Document& operator=(Document&& other) noexcept {
        if (yyjson_likely(this != &other)) {
            free();
            m_doc = other.m_doc;
            other.m_doc = nullptr;
        }
        return *this;
    }
    
    // Error checking
    bool isValid() const { return m_doc != nullptr; }
    bool hasError() const { return !isValid(); }
    bool operator!() const { return hasError(); }
    explicit operator bool() const { return isValid(); }

    // Access underlying yyjson document.
    yyjson_doc* get() const { return m_doc; }
    yyjson_doc* c_doc() const { return m_doc; }

    // Access root value (returns new Value instance)
    Value root() const { return Value(yyjson_doc_get_root(m_doc)); }

    // Read from various source, string, file.
    bool read(const char* str, size_t len = 0);
    bool read(const std::string& str) { return read(str.c_str(), str.size()); }
    bool read(FILE* fp);
    bool read(std::ifstream& ifs);
    bool readFile(const char* path);
    
    // Write to various target, string, file.
    bool write(std::string& output) const;
    bool write(FILE* fp) const;
    bool write(std::ofstream& ofs) const;
    bool writeFile(const char* path) const;

    // Index access
    template <typename T>
    Value operator[](T&& index) const
    {
        return root().index(std::forward<T>(index));
    }

    // Convert to mutable document
    MutableDocument mutate() const;
    
private:
    yyjson_doc* m_doc = nullptr;
};

/* @Section 2.2: Mutable Json Model */
/* ------------------------------------------------------------------------ */
#ifndef XYJSON_DISABLE_MUTABLE

/**
 * @brief Writable JSON value wrapper with comprehensive read-write operations
 * 
 * This class extends the Value interface with full read-write capabilities,
 * providing mutable operations on yyjson_mut_val pointers. It supports
 * value modification, container manipulation, and complex data building.
 * 
 * @note MutableValue instances must be associated with a MutableDocument
 *       for memory allocation. Use the assignment operators carefully to
 *       avoid unintended modifications.
 * 
 * @see Value for read-only operations
 * @see MutableDocument for document-level writable operations
 * 
 * Supported operators:
 * - Access: json[index] (index access), json / path (path access)
 * - Unary: +json (convert to number), -json (convert to string)
 * - Extraction: json | defaultValue (value extraction), dest |= json (assignment)
 * - Type: json & type (check type)
 * - Assignment: json = value (set value)
 * - Pipe: json | func (custom transformation)
 * - Key-value: json * key, key * json (KeyValue creation)
 * - Input: json << value (smart input)
 * - Scalar output: json >> value
 * - Stream output: os << json
 * - Boolean: !json, bool(json) (error checking)
 * - Comparison: ==, != (calls equal())
 * - Iterator: json % index (array), json % key (object)
 */
class MutableValue
{
public:
    using array_iterator = MutableArrayIterator;
    using object_iterator = MutableObjectIterator;
    
    MutableValue() : m_val(nullptr), m_doc(nullptr) {}
    MutableValue(yyjson_mut_val* val, yyjson_mut_doc* doc) : m_val(val), m_doc(doc) {}
    
    // Error checking
    bool isValid() const { return m_val != nullptr; }
    bool hasError() const { return !isValid(); }
    bool operator!() const { return hasError(); }
    explicit operator bool() const { return isValid(); }

    // Type checking (same as read-only)
    bool isNull() const { return yyjson_mut_is_null(m_val); }
    bool isBool() const { return yyjson_mut_is_bool(m_val); }
    bool isNumber() const { return yyjson_mut_is_num(m_val); }
    bool isInt() const { return yyjson_mut_is_int(m_val); }
    bool isSint() const { return yyjson_mut_is_sint(m_val); }
    bool isUint() const { return yyjson_mut_is_uint(m_val); }
    bool isReal() const { return yyjson_mut_is_real(m_val); }
    bool isString() const { return yyjson_mut_is_str(m_val); }
    bool isArray() const { return yyjson_mut_is_arr(m_val); }
    bool isObject() const { return yyjson_mut_is_obj(m_val); }
    const char* typeName() const { return yyjson_mut_get_type_desc(m_val); }
    yyjson_type getType() const { return yyjson_mut_get_type(m_val); }
    
    // Type checking methods with template parameters
    bool isType(int) const { return isInt(); }
    bool isType(int64_t) const { return isSint(); }
    bool isType(uint64_t) const { return isUint(); }
    bool isType(double) const { return isReal(); }
    bool isType(bool) const { return isBool(); }
    bool isType(std::nullptr_t) const { return isNull(); }
    bool isType(const std::string& type) const { return isString(); }
    bool isType(ZeroNumber) const { return isNumber(); }
    bool isType(EmptyString) const { return isString(); }
    bool isType(EmptyArray) const { return isArray(); }
    bool isType(EmptyObject) const { return isObject(); }
    bool isType(const char* type) const 
    { 
        if (yyjson_likely(type != nullptr)) {
            if (strcmp(type, "{}") == 0) return isObject();
            if (strcmp(type, "[]") == 0) return isArray();
        }
        return isString(); 
    }
    
    bool isType(yyjson_mut_val*) const { return isValid(); }
    bool isType(yyjson_mut_doc*) const { return m_doc != nullptr; }
    
    // Access underlying yyjson value. yyjson api use non-const pointer.
    yyjson_mut_val* get() const { return m_val; }
    
    // Access the document this value belongs to
    yyjson_mut_doc* getDoc() const { return m_doc; }

    // Set underlying yyjson value pointer directly
    void set(yyjson_mut_val* val) { m_val = val; }
    
    // Mark as moved, set internal pointers to nullptr
    void setMoved() { m_val = nullptr; m_doc = nullptr; }

    // Value extraction with result reference
    bool get(bool& result) const;
    bool get(int& result) const;
    bool get(int64_t& result) const;
    bool get(uint64_t& result) const;
    bool get(double& result) const;
    bool get(const char*& result) const;
    bool get(std::string& result) const;
    // Underlying C pointer extraction
    bool get(yyjson_mut_val*& result) const;
    bool get(yyjson_mut_doc*& result) const;
    
    // Convenience method template with default value
    template<typename T>
    std::decay_t<T> getor(T&& defaultValue) const;
    // Special overloads for sentinels
    const char* getor(EmptyString) const;
    double getor(ZeroNumber) const;
    MutableArray getor(EmptyArray) const;
    MutableObject getor(EmptyObject) const;
    
    // Pipe function for custom transformations
    template<typename funcT>
    auto pipe(funcT&& func) const;

    // Array/Object size and access by index or key
    size_t size() const;

    // Clear content based on type
    void clear();

    MutableValue index(size_t idx) const;
    MutableValue index(int idx) const { return index(static_cast<size_t>(idx)); }

    MutableValue index(const char* key, size_t len) const;
    MutableValue index(const std::string& key) const {
        return index(key.c_str(), key.size()); 
    }
    template <size_t N>
    MutableValue index(const char(&key)[N]) const { return index(key, N-1); }
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), MutableValue>::type
    index(T key) const { return index(key, key ? ::strlen(key) : 0); }

    // non-const version for automatic object insertion
    MutableValue index(size_t idx);
    MutableValue index(int idx) { return index(static_cast<size_t>(idx)); }

    MutableValue index(const char* key, size_t len);
    MutableValue index(const std::string& key) {
        return index(key.c_str(), key.size()); 
    }
    template <size_t N>
    MutableValue index(const char(&key)[N]) { return index(key, N-1); }
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), MutableValue>::type
    index(T key) { return index(key, key ? ::strlen(key) : 0); }

    // Array and object access operator (const version)
    template <typename T>
    MutableValue operator[](const T& index) const {
        return this->index(index);
    }
    
    // Array and object access operator (non-const version)
    template <typename T>
    MutableValue operator[](const T& index) {
        return this->index(index);
    }

    // Path operations
    MutableValue pathto(size_t idx) const { return index(idx); }
    MutableValue pathto(int idx) const { return index(idx); }
    MutableValue pathto(const char* path, size_t len) const;
    MutableValue pathto(const std::string& path) const {
        return pathto(path.c_str(), path.size());
    }
    template <size_t N>
    MutableValue pathto(const char(&path)[N]) const { return pathto(path, N-1); }
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), MutableValue>::type
    pathto(T path) const {
        return pathto(path, path ? ::strlen(path) : 0);
    }
    
    // Copy assignment
    MutableValue& set(const MutableValue& other);

    // Assignment methods to native scalar types.
    // Should not be used on an existing object or array.
    MutableValue& set(std::nullptr_t value);
    MutableValue& set(bool value);
    MutableValue& set(int value);
    MutableValue& set(int64_t value);
    MutableValue& set(uint64_t value);
    MutableValue& set(double value);
    MutableValue& set(const std::string& value);
    MutableValue& set(StringRef value);
    MutableValue& setCopy(const char* value, size_t len);
    MutableValue& setArray();
    MutableValue& setObject();
    // Set using empty-type sentinels
    MutableValue& set(EmptyArray) { return setArray(); }
    MutableValue& set(EmptyObject) { return setObject(); }
    MutableValue& set(EmptyString) { return set(""); }
    MutableValue& set(ZeroNumber) { return set(0.0); }

    // String literal optimization
    template <size_t N>
    MutableValue& set(const char(&value)[N]);
    
    // Non-const char array (mutable buffer) - use copy semantics
    template <size_t N>
    MutableValue& set(char(&value)[N]);
    
    // C-Style string: const char* value
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), MutableValue&>::type
    set(T value);

    // Assignment operators
    template <typename T>
    MutableValue& operator=(T&& value)
    {
        return set(std::forward<T>(value));
    }

    // Append methods for array.
    MutableValue& append(yyjson_mut_val* value);
    template <typename T>
    MutableValue& append(T&& value);
    
    // Insert methods for object.
    MutableValue& add(yyjson_mut_val* key, yyjson_mut_val* value);
    MutableValue& add(KeyValue&& kv);
    template<typename keyT, typename valT>
    MutableValue& add(keyT&& key, valT&& value);
    
    // KeyValue creation methods for optimized object insertion
    KeyValue tag(MutableValue&& key) &&;
    template <typename keyT>
    KeyValue tag(keyT&& key) &&;

    MutableValue& input(KeyValue&& kv);

    // Smart push method, append for array, add for object
    template <typename T> MutableValue& push(T&& value);
    MutableValue& push(KeyValue&& kv);

#ifndef XYJSON_DISABLE_CHAINED_INPUT
    // Push pending key, only support string type.
    template <typename keyT> bool pushKey(keyT&& key);
    // Push value after pending key.
    template <typename T> bool pushValue(T&& value);
#endif

    // Pop methods for removing elements from containers
    MutableValue& pop(MutableValue& result);
    MutableValue& pop(KeyValue& result);

    // Unified iterator creation method
    MutableArrayIterator iterator(size_t startIndex) const;
    MutableArrayIterator iterator(int startIndex) const;
    MutableObjectIterator iterator(const char* startKey) const;
    MutableArrayIterator iterator(EmptyArray) const;
    MutableObjectIterator iterator(EmptyObject) const;

    // Standard iterator pattern methods
    MutableArrayIterator beginArray() const;
    MutableArrayIterator endArray() const;
    MutableObjectIterator beginObject() const;
    MutableObjectIterator endObject() const;

    // Container-specific wrapper methods for standard iteration
    MutableArray array() const;
    MutableObject object() const;
    
    // Match iterator method
    MutableValue* operator->() { return this; }
    yyjson_mut_val* c_val() const { return m_val; }
    yyjson_mut_doc* c_doc() const { return m_doc; }

    // Conversion methods
    std::string toString(bool pretty = false) const;
    int toInteger() const;
    double toNumber() const;
    
    // Explicit type conversion operators
    explicit operator int() const { return toInteger(); }
    explicit operator double() const { return toNumber(); }
    explicit operator std::string() const { return toString(); }
    
    // Comparison method
    bool equal(const MutableValue& other) const;
    bool less(const MutableValue& other) const;
    
private:
    yyjson_mut_val* m_val = nullptr;
    yyjson_mut_doc* m_doc = nullptr; // Needed for memory allocation

#ifndef XYJSON_DISABLE_CHAINED_INPUT
    // For chained operator << key << value, save the key
    yyjson_mut_val* m_pendingKey = nullptr;
#endif

    friend class MutableDocument;
};

/**
 * @brief Mutable JSON document wrapper with comprehensive read-write operations
 * 
 * This class provides document-level read-write operations, managing
 * yyjson_mut_doc resources with full lifecycle control. It serves as
 * the foundation for building and modifying JSON data structures.
 * 
 * @note MutableDocument owns the underlying yyjson_mut_doc and handles
 *       all memory allocation. Copy operations are disabled to prevent
 *       resource management issues.
 * 
 * @see Document for read-only document operations
 * @see MutableValue for value-level writable operations
 * 
 * Supported operators:
 * - Unary: +doc (convert root to number), -doc (convert root to string), *doc (access root), ~doc (convert to read-only)
 * - Path: doc / "key" (path access), doc[index] (index access)
 * - Stream: doc << input, doc >> output (serialization)
 * - Create: doc * value (quick node creation)
 * - Boolean: if (doc), !doc (error checking)
 * - Comparison: ==, != (calls root.equal())
 */
class MutableDocument
{
public:
    using value_type = MutableValue;
    
    // Constructors.
    MutableDocument() : MutableDocument("{}") {}
    explicit MutableDocument(yyjson_mut_doc* doc);
    explicit MutableDocument(const char* str, size_t len = 0);
    explicit MutableDocument(const std::string& str) : MutableDocument(str.c_str(), str.size()) {}
    
    // Conversion from Document
    explicit MutableDocument(const Document& other);
    
    // Free the document
    void free();
    ~MutableDocument() { free(); }

    // Disable copy operations to prevent double-free issues
    MutableDocument(const MutableDocument&) = delete;
    MutableDocument& operator=(const MutableDocument&) = delete;
    
    // Enable move operations
    MutableDocument(MutableDocument&& other) noexcept : m_doc(other.m_doc) {
        other.m_doc = nullptr;
    }
    MutableDocument& operator=(MutableDocument&& other) noexcept {
        if (yyjson_likely(this != &other)) {
            free();
            m_doc = other.m_doc;
            other.m_doc = nullptr;
        }
        return *this;
    }

    // Error checking
    bool isValid() const { return m_doc != nullptr; }
    bool hasError() const { return !isValid(); }
    bool operator!() const { return hasError(); }
    explicit operator bool() const { return isValid(); }

    // Access underlying yyjson document.
    yyjson_mut_doc* get() const { return m_doc; }
    yyjson_mut_doc* c_doc() const { return m_doc; }

    // Access root value
    MutableValue root() const { return MutableValue(yyjson_mut_doc_get_root(m_doc), m_doc); }

    // Set root value
    void setRoot(MutableValue val);

    // Read from various source, string, file.
    bool read(const char* str, size_t len = 0);
    bool read(const std::string& str) { return read(str.c_str(), str.size()); }
    bool read(FILE* fp);
    bool read(std::ifstream& ifs);
    bool readFile(const char* path);
    
    // Write to various target, string, file.
    bool write(std::string& output) const;
    bool write(FILE* fp) const;
    bool write(std::ofstream& ofs) const;
    bool writeFile(const char* path) const;
    
    // Create methods for various types to create JSON nodes
    MutableValue create(yyjson_mut_val* value) const;
    MutableValue create() const;       // null
    // T: bool | int | string | (Mutable)Document | (Mutable)Value
    template <typename T> MutableValue create(T&& value) const;

    // Index access - const version for read-only access
    template <typename T>
    MutableValue operator[](T&& index) const
    {
        return root().index(std::forward<T>(index));
    }
    
    // Index access - non-const version for automatic insertion
    template <typename T>
    MutableValue operator[](T&& index)
    {
        return root().index(std::forward<T>(index));
    }

    // Convert to read-only document
    Document freeze() const;
    
private:
    yyjson_mut_doc* m_doc = nullptr;
};
#endif // XYJSON_DISABLE_MUTABLE

/* @Section 2.3: Helper Class for Mutable Json */
/* ------------------------------------------------------------------------ */
#ifndef XYJSON_DISABLE_MUTABLE

/**
 * @brief String reference wrapper for optimized literal handling
 * 
 * This structure provides efficient string handling for JSON creation,
 * particularly optimizing string literals by avoiding unnecessary
 * copying when the string has static lifetime.
 * 
 * @note The private constructor for mutable char arrays prevents
 *       accidental use of mutable strings without proper copying.
 */
struct StringRef
{
    const char* str = nullptr;  ///< Pointer to string data
    size_t len = 0;             ///< Length of string data

    template <size_t N> 
    StringRef(const char(&value)[N]) : StringRef(value, N-1) { }

    explicit
    StringRef(const char* value, size_t n) : str(value), len(n) { }
    
    explicit
    StringRef(const char* value) : StringRef(value, ::strlen(value)) { }
    
    explicit
    StringRef(const std::string& value) : StringRef(value.c_str(), value.size()) { }

    operator const char *() const { return str; }

private:
    template <size_t N> 
    StringRef(char(&value)[N]);
};

/** 
 * @brief Key-Value pair structure for optimized object insertion
 * 
 * This simple structure holds key and value pointers for efficient
 * object insertion operations. It provides validation methods and
 * supports boolean context checking.
 * 
 * @note KeyValue instances are typically created using operator*
 *       and consumed by object insertion methods.
 */
struct KeyValue
{
    yyjson_mut_val* key = nullptr;    ///< The key for object insertion
    yyjson_mut_val* value = nullptr;  ///< The value for object insertion
    
    KeyValue() : key(nullptr), value(nullptr) {}
    
    KeyValue(yyjson_mut_val* k, yyjson_mut_val* v) 
        : key(k), value(v) {}
    
    /// Check if KeyValue contains valid key and value pointers
    bool isValid() const { return key && value; }
    
    explicit operator bool() const { return isValid(); }
    
    bool operator!() const { return !isValid(); }
};
#endif // XYJSON_DISABLE_MUTABLE

/* @Section 2.4: Iterator for Json Container */
/* ------------------------------------------------------------------------ */

/**
 * @brief Four iterator from Value/MutableValue array/object.
 * 
 * These iterators provides efficient forward iteration over JSON array elements
 * using the underlying yyjson's iterator structure. It supports standard
 * iterator operations and provides value access through operator overloading.
 * 
 * Supported operators:
 * - Dereference: *iter, iter-> (access current item)
 * - Increment: ++iter, iter++ (calls next())
 * - Advance: iter + n, iter += n (calls advance(n))
 * - Position: iter % n , iter %= n (calls advance(n))
 * - Unary: +iter (get current index), -iter (no-op), ~iter (no-op)
 * - Comparison: ==, != (calls equal())
 * - Boolean: if (iter), !iter
 * - Mutable iterator support << >> for insert and remove
 * - Object iterator support / for fast seek
 */

class ArrayIterator
{
public:
    static constexpr bool for_object = false;
    static constexpr bool for_mutable = false;
    using json_type = Value;
    
    // Standard iterator traits
    using iterator_category = std::forward_iterator_tag;
    using value_type = Value;
    using difference_type = std::ptrdiff_t;
    using pointer = Value;
    using reference = Value;

    ArrayIterator() : m_arr(nullptr), m_iter({0}) {}
    explicit ArrayIterator(yyjson_val* root);
    
    // Check if iterator is valid (current element exists)
    bool isValid() const { return m_iter.idx < m_iter.max; }
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }

    // Set to end state (default: fast to end)
    ArrayIterator& end() { m_iter.idx = m_iter.max; return *this; }
    // Cycle to true end (traverse to the actual end)
    ArrayIterator& end(bool cycle) {
        if (yyjson_unlikely(!cycle)) return end();
        while (isValid()) next();
        return *this;
    }

    // Check if two iterators are equal
    bool equal(const ArrayIterator& other) const {
        return m_arr == other.m_arr && m_iter.idx == other.m_iter.idx;
    }

    // Get underlying C API iterator and value structure pointer
    yyjson_arr_iter* c_iter() { return &m_iter; }
    const yyjson_arr_iter* c_iter() const { return &m_iter; }
    yyjson_val* c_val() const { return isValid() ? m_iter.cur : nullptr; }

    // Get current Proxy Value
    Value value() const { return Value(c_val()); }

    // Get current index and key name (for array iterators, key returns default Value)
    size_t index() const { return m_iter.idx; }
    Value key() const { return Value(); } // Empty value for array iterators
    const char* name() const { return nullptr; } // nullptr for array iterators

    // Get current value (for dereference/array operator)
    Value operator*()  const { return value(); }
    Value operator->() const { return value(); }

    // Position manipulation
    ArrayIterator& next(); // prefix ++, move to nexe element
    ArrayIterator& prev(); // prefix --, move to previous element
    ArrayIterator& advance(size_t steps = 1); // +n, move n steps
    ArrayIterator& advance(const char* key) { return end(); } // No-op for array iterator
    ArrayIterator& begin(); // Reset iterator to beginning

private:
    /// Native yyjson array iterator
    yyjson_arr_iter m_iter;
    /// Current array to iterate (not include in m_iter)
    yyjson_val* m_arr = nullptr;
};

class ObjectIterator
{
public:
    static constexpr bool for_object = true;
    static constexpr bool for_mutable = false;
    using json_type = Value;
    
    // Standard iterator traits
    using iterator_category = std::forward_iterator_tag;
    using value_type = Value;
    using difference_type = std::ptrdiff_t;
    using pointer = Value;
    using reference = Value;
    
    ObjectIterator() : m_iter({0}) {}
    explicit ObjectIterator(yyjson_val* root);
    
    // Check if iterator is valid
    bool isValid() const { return m_iter.idx < m_iter.max; }
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }

    // Set to end state (default: fast to end)
    ObjectIterator& end() { m_iter.idx = m_iter.max; return *this; }
    // Cycle to true end (traverse to the actual end)
    ObjectIterator& end(bool cycle) {
        if (yyjson_unlikely(!cycle)) return end();
        while (isValid()) next();
        return *this;
    }

    // Check if two iterators are equal
    bool equal(const ObjectIterator& other) const {
        return m_iter.obj == other.m_iter.obj && m_iter.idx == other.m_iter.idx;
    }

    // Get underlying C API iterator and key/val structure pointer
    yyjson_obj_iter* c_iter() { return &m_iter; }
    const yyjson_obj_iter* c_iter() const { return &m_iter; }
    yyjson_val* c_key() const { return isValid() ? m_iter.cur : nullptr; }
    yyjson_val* c_val() const { return isValid() && m_iter.cur ? (m_iter.cur + 1) : nullptr; }

    // Get current key/val Proxy Value
    Value key() const { return Value(c_key()); }
    Value value() const { return Value(c_val()); }

    // Get current index and key name
    size_t index() const { return m_iter.idx; }
    const char* name() const { return yyjson_get_str(c_key()); }

    // Get current value (for dereference/array operator)
    Value operator*()  const { return value(); }
    Value operator->() const { return value(); }

    // Position manipulation
    ObjectIterator& next(); // prefix ++
    ObjectIterator& prev(); // prefix --, move to previous element
    ObjectIterator& advance(size_t steps = 1); // +n
    ObjectIterator& advance(const char* key); // Jump to specific key
    ObjectIterator& begin(); // Reset iterator to beginning

    // Seek to specific key using fast yyjson API and return Value
    Value seek(const char* key, size_t key_len);
    Value seek(const std::string& key) { return seek(key.c_str(), key.size()); }
    template<size_t N>
    Value seek(const char(&key)[N]) { return seek(key, N-1); }
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), Value>::type
    seek(T key) { return seek(key, key ? ::strlen(key) : 0); }
    
private:
    /// Native yyjson object iterator
    yyjson_obj_iter m_iter;
};

#ifndef XYJSON_DISABLE_MUTABLE
class MutableArrayIterator
{
public:
    static constexpr bool for_object = false;
    static constexpr bool for_mutable = true;
    using json_type = MutableValue;
    
    // Standard iterator traits
    using iterator_category = std::forward_iterator_tag;
    using value_type = MutableValue;
    using difference_type = std::ptrdiff_t;
    using pointer = MutableValue;
    using reference = MutableValue;

    MutableArrayIterator() : m_doc(nullptr), m_iter({0}) {}
    explicit MutableArrayIterator(yyjson_mut_val* root, yyjson_mut_doc* doc);

    // Check if iterator is valid (current element exists)
    bool isValid() const { return m_iter.idx < m_iter.max; }
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }

    // Set to end state (default: fast to end)
    MutableArrayIterator& end() { m_iter.idx = m_iter.max; return *this; }
    // Cycle to true end (traverse to the actual end)
    MutableArrayIterator& end(bool cycle) {
        if (yyjson_unlikely(!cycle)) return end();
        while (isValid()) next();
        return *this;
    }

    // Check if two iterators are equal
    bool equal(const MutableArrayIterator& other) const {
        return m_iter.arr == other.m_iter.arr && m_iter.idx == other.m_iter.idx;
    }

    // Get underlying C API iterator and value structure pointer
    yyjson_mut_arr_iter* c_iter() { return &m_iter; }
    const yyjson_mut_arr_iter* c_iter() const { return &m_iter; }
    yyjson_mut_val* c_val() const {
        return isValid() ? m_iter.cur : nullptr;
    }

    // Get current Proxy Value
    MutableValue value() const { return MutableValue(c_val(), m_doc); }

    // Get current index and key name (for array iterators, key returns default MutableValue)
    size_t index() const { return m_iter.idx; }
    MutableValue key() const { return MutableValue(); } // Empty value for array iterators
    const char* name() const { return nullptr; } // nullptr for array iterators

    // Get current value (for dereference/array operator)
    MutableValue operator*()  const { return value(); }
    MutableValue operator->() const { return value(); }

    // Position manipulation
    MutableArrayIterator& next(); // prefix ++
    MutableArrayIterator& prev(); // prefix --, move to previous element
    MutableArrayIterator& advance(size_t steps = 1); // +n
    MutableArrayIterator& advance(const char* key) { return end(); } // No-op for array iterator
    MutableArrayIterator& begin(); // Reset iterator to beginning

    // Insert methods
    bool insert(yyjson_mut_val* value); // Insert raw pointer, return success status
    template<typename T>
    bool insert(T&& value); // Insert various types, return success status
    
    // Remove method
    MutableValue remove(); // Remove current element and return it

private:
    /// Native yyjson mutable array iterator
    yyjson_mut_arr_iter m_iter;
    /// Document for context (needed for mutation)
    yyjson_mut_doc* m_doc = nullptr;
};

class MutableObjectIterator
{
public:
    static constexpr bool for_object = true;
    static constexpr bool for_mutable = true;
    using json_type = MutableValue;
    
    // Standard iterator traits
    using iterator_category = std::forward_iterator_tag;
    using value_type = MutableValue;
    using difference_type = std::ptrdiff_t;
    using pointer = MutableValue;
    using reference = MutableValue;
    
    MutableObjectIterator() : m_doc(nullptr), m_iter({0})
#ifndef XYJSON_DISABLE_CHAINED_INPUT
        , m_pendingKey(nullptr)
#endif
    {}
    explicit MutableObjectIterator(yyjson_mut_val* root, yyjson_mut_doc* doc);
    
    // Check if iterator is valid
    bool isValid() const { return m_iter.idx < m_iter.max; }
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }

    // Set to end state (default: fast to end)
    MutableObjectIterator& end() { m_iter.idx = m_iter.max; return *this; }
    // Cycle to true end (traverse to the actual end)
    MutableObjectIterator& end(bool cycle) {
        if (yyjson_unlikely(!cycle)) return end();
        while (isValid()) next();
        return *this;
    }

    // Check if two iterators are equal
    bool equal(const MutableObjectIterator& other) const {
        return m_iter.obj == other.m_iter.obj && m_iter.idx == other.m_iter.idx;
    }

    // Get underlying C API iterator and key/val structure pointer
    yyjson_mut_obj_iter* c_iter() { return &m_iter; }
    const yyjson_mut_obj_iter* c_iter() const { return &m_iter; }
    yyjson_mut_val* c_key() const {
        return isValid() ? m_iter.cur : nullptr;
    }
    yyjson_mut_val* c_val() const {
        return (isValid() && m_iter.cur) ? m_iter.cur->next : nullptr;
    }

    // Get current key/val Proxy Value
    MutableValue key() const { return MutableValue(c_key(), m_doc); }
    MutableValue value() const { return MutableValue(c_val(), m_doc); }

    // Get current index and key name
    size_t index() const { return m_iter.idx; }
    const char* name() const { return yyjson_mut_get_str(c_key()); }

    // Get current value (for dereference/array operator)
    MutableValue operator*()  const { return value(); }
    MutableValue operator->() const { return value(); }

    // Position manipulation
    MutableObjectIterator& next(); // prefix ++, move to next key-value pair
    MutableObjectIterator& prev(); // prefix --, move to previous key-value pair
    MutableObjectIterator& advance(size_t steps = 1); // +n to next n pairs
    MutableObjectIterator& advance(const char* key); // jump to specific key
    MutableObjectIterator& begin(); // Reset iterator to beginning

    // Seek to specific key using fast yyjson API and return MutableValue
    MutableValue seek(const char* key, size_t key_len);
    MutableValue seek(const std::string& key) { return seek(key.c_str(), key.size()); }
    template<size_t N>
    MutableValue seek(const char(&key)[N]) { return seek(key, N-1); }
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), MutableValue>::type
    seek(T key) { return seek(key, key ? ::strlen(key) : 0); }
    
    // Insertion methods
    bool insert(yyjson_mut_val* key, yyjson_mut_val* val);
    template<typename K, typename V> bool insert(K&& key, V&& value);
    bool insert(KeyValue&& kv_pair);
#ifndef XYJSON_DISABLE_CHAINED_INPUT
    // Chained insertion support
    template<typename keyT> bool insertKey(keyT&& key);
    template<typename valT> bool insertValue(valT&& value);
    // Insert method for chained insertion - handles key/value logic internally
    template<typename T> bool insert(T&& arg);
#endif

    // Remove current key-value pair and return it
    KeyValue remove();
    
private:
    /// Native yyjson mutable object iterator
    yyjson_mut_obj_iter m_iter;
    /// Document for context (needed for mutation)
    yyjson_mut_doc* m_doc = nullptr;
#ifndef XYJSON_DISABLE_CHAINED_INPUT
    /// Pending key for chained insertion
    yyjson_mut_val* m_pendingKey = nullptr;
#endif
};

#endif // XYJSON_DISABLE_MUTABLE

/* @Section 2.5: Container-specific Wrapper Classes */
/* ------------------------------------------------------------------------ */

/**
 * @brief Four container-specific wrapper for Value/Mutable array/object
 * that provides standard iterator interface of begin() and end() methods.
 */

class ConstArray : public Value
{
public:
    using iterator = ArrayIterator;
    
    ConstArray() : Value() {}
    explicit ConstArray(const Value& val) : Value(val) 
    {
        if (yyjson_unlikely(!val.isArray())) { set(nullptr); }
    }
    
    iterator begin() const { return Value::beginArray(); }
    iterator end() const { return Value::endArray(); }
};

class ConstObject : public Value
{
public:
    using iterator = ObjectIterator;
    
    ConstObject() : Value() {}
    explicit ConstObject(const Value& val) : Value(val) 
    {
        if (yyjson_unlikely(!val.isObject())) { set(nullptr); }
    }
    
    iterator begin() const { return Value::beginObject(); }
    iterator end() const { return Value::endObject(); }
};

#ifndef XYJSON_DISABLE_MUTABLE
class MutableArray : public MutableValue
{
public:
    using iterator = MutableArrayIterator;
    
    MutableArray() : MutableValue() {}
    explicit MutableArray(const MutableValue& val) : MutableValue(val) 
    {
        if (yyjson_unlikely(!val.isArray())) { set(nullptr); }
    }
    
    iterator begin() const { return MutableValue::beginArray(); }
    iterator end() const { return MutableValue::endArray(); }
};

class MutableObject : public MutableValue
{
public:
    using iterator = MutableObjectIterator;
    
    MutableObject() : MutableValue() {}
    explicit MutableObject(const MutableValue& val) : MutableValue(val) 
    {
        if (yyjson_unlikely(!val.isObject())) { set(nullptr); }
    }
    
    iterator begin() const { return MutableValue::beginObject(); }
    iterator end() const { return MutableValue::endObject(); }
};
#endif // XYJSON_DISABLE_MUTABLE

/* @Part 3: Non-Class Functions */
/* ======================================================================== */

/**
 * @brief Utility functions for JSON value creation and manipulation
 * 
 * This namespace contains helper functions that support the main class
 * functionality, providing value creation, type conversion, and other
 * utility operations used throughout the library.
 */
namespace util
{

/* @Section 3.1: Underlying mut_val Creation */
/* ------------------------------------------------------------------------ */
/**
 * @brief JSON value creation utilities for mutable operations
 * 
 * These functions create yyjson_mut_val instances from various C++ types,
 * handling type conversion and memory allocation through the provided
 * document. They form the foundation for mutable JSON operations.
 */

#ifndef XYJSON_DISABLE_MUTABLE

inline yyjson_mut_val* create(yyjson_mut_doc* doc, std::nullptr_t = nullptr)
{
    return yyjson_mut_null(doc);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, bool value)
{
    return yyjson_mut_bool(doc, value);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, int value)
{
    return yyjson_mut_int(doc, value);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, int64_t value)
{
    return yyjson_mut_sint(doc, value);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, uint64_t value)
{
    return yyjson_mut_uint(doc, value);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, double value)
{
    return yyjson_mut_real(doc, value);
}

inline yyjson_mut_val* createObject(yyjson_mut_doc* doc)
{
    return yyjson_mut_obj(doc);
}

inline yyjson_mut_val* createArray(yyjson_mut_doc* doc)
{
    return yyjson_mut_arr(doc);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, const char* value, size_t len)
{
    return yyjson_mut_strncpy(doc, value, len);
}

template<typename T>
inline typename std::enable_if<trait::is_cstr_type<T>(), yyjson_mut_val*>::type
create(yyjson_mut_doc* doc, T value)
{
    return create(doc, value, ::strlen(value));
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, const std::string& value)
{
    return create(doc, value.c_str(), value.size());
}

// For string literals, use reference optimization (yyjson_mut_strn)
// since literals have static lifetime and don't need copying.
// Special handling for empty object and array literals
template <size_t N>
inline yyjson_mut_val* create(yyjson_mut_doc* doc, const char(&value)[N])
{
    if (N == 3)
    {
        if (::strcmp(value, "{}") == 0) { return createObject(doc); }
        if (::strcmp(value, "[]") == 0) { return createArray(doc); }
    }
    return yyjson_mut_strn(doc, value, N-1);
}

// For char arrays (mutable), use copy instead of reference optimization
template <size_t N>
inline yyjson_mut_val* create(yyjson_mut_doc* doc, char(&value)[N])
{
    // Always copy mutable arrays, may have '\0' in middle
    return yyjson_mut_strcpy(doc, value);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, StringRef value)
{
    if (value.len == 2)
    {
        if (::strcmp(value, "{}") == 0) { return createObject(doc); }
        if (::strcmp(value, "[]") == 0) { return createArray(doc); }
    }

    return yyjson_mut_str(doc, value);
}

// Overloads for empty-type sentinels
inline yyjson_mut_val* create(yyjson_mut_doc* doc, EmptyArray)
{
    return createArray(doc);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, EmptyObject)
{
    return createObject(doc);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, EmptyString)
{
    return yyjson_mut_strncpy(doc, "", 0);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, ZeroNumber)
{
    return yyjson_mut_real(doc, 0.0);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, yyjson_val* src)
{
    return yyjson_val_mut_copy(doc, src);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, yyjson_doc* src)
{
    return create(doc, yyjson_doc_get_root(src));
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, const Value& src)
{
    return create(doc, src.get());
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, const Document& src)
{
    return create(doc, src.root());
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, yyjson_mut_val* src)
{
    return yyjson_mut_val_mut_copy(doc, src);
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, yyjson_mut_doc* src)
{
    return create(doc, yyjson_mut_doc_get_root(src));
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, const MutableValue& src)
{
    return create(doc, src.get());
}

// Move semantics overload for MutableValue&&
inline yyjson_mut_val* create(yyjson_mut_doc* doc, MutableValue&& src)
{
    // Check if this MutableValue can be moved (same document)
    if (src.getDoc() == doc) {
        yyjson_mut_val* result = src.get();
        src.setMoved();
        return result;
    }
    return create(doc, src.get());
}

inline yyjson_mut_val* create(yyjson_mut_doc* doc, const MutableDocument& src)
{
    return create(doc, src.root());
}

// Create key node for object insertion, with string literal optimization
template<typename T>
inline typename std::enable_if<trait::is_key_v<T>, yyjson_mut_val*>::type
createKey(yyjson_mut_doc* doc, T&& key)
{
    return create(doc, std::forward<T>(key));
}

template<typename T>
inline typename std::enable_if<!trait::is_key_v<T>, yyjson_mut_val*>::type
createKey(yyjson_mut_doc* doc, T&& key)
{
    return nullptr;
}

// Specialization for MutableValue key
inline yyjson_mut_val* createKey(yyjson_mut_doc* doc, const MutableValue& key)
{
    if (key.isString()) {
        return create(doc, key);
    }
    return nullptr;
}

// Move semantics overload for MutableValue&& key
inline yyjson_mut_val* createKey(yyjson_mut_doc* doc, MutableValue&& key)
{
    if (key.isString()) {
        return create(doc, std::move(key));
    }
    return nullptr;
}

#endif // XYJSON_DISABLE_MUTABLE

/* @Section 3.2: Conversion Helper Functions */
/* ------------------------------------------------------------------------ */
/**
 * @brief Type conversion and comparison utilities
 * 
 * These functions provide type conversion and comparison operations
 * that work with both Value and MutableValue types, enabling unified
 * handling of JSON values regardless of mutability.
 */

/**
 * @brief Compare two JSON values using hybrid comparison logic
 * 
 * This function provides a comprehensive comparison of JSON values
 * following these rules:
 * 1. Invalid values are considered less than valid values
 * 2. Values of different types are compared by type order
 * 3. Values of the same type are compared by their actual values
 * 4. Containers are compared by size first, then by pointer
 * 
 * @tparam T JSON value type (Value or MutableValue)
 * @param lhs Left-hand side value
 * @param rhs Right-hand side value
 * @return bool True if lhs is less than rhs
 */
template<typename T>
inline typename std::enable_if<trait::is_value<T>::value, bool>::type
lessCompare(const T& lhs, const T& rhs)
{
    if (yyjson_unlikely(!lhs.isValid())) {
        return rhs.isValid(); // an invalid value is less than a valid one
    }
    if (yyjson_unlikely(!rhs.isValid())) {
        return false;
    }

    auto lhs_type = lhs.getType();
    auto rhs_type = rhs.getType();

    if (lhs_type != rhs_type) {
        return lhs_type < rhs_type;
    }

    switch (lhs_type) {
        case YYJSON_TYPE_NULL:
            return false; // nulls are equal

        case YYJSON_TYPE_BOOL:
            return (lhs.getor(false)) < (rhs.getor(false));

        case YYJSON_TYPE_NUM: {
            return lhs.toNumber() < rhs.toNumber();
        }
        case YYJSON_TYPE_STR: {
            const char* lhs_str = lhs | kString;
            const char* rhs_str = rhs | kString;
            return ::strcmp(lhs_str, rhs_str) < 0;
        }
        case YYJSON_TYPE_ARR:
        case YYJSON_TYPE_OBJ: {
            size_t lhs_size = lhs.size();
            size_t rhs_size = rhs.size();
            if (lhs_size != rhs_size) {
                return lhs_size < rhs_size;
            }
            // Fallback to pointer comparison for containers of the same size.
            return lhs.get() < rhs.get();
        }
        default: // RAW, or any other type
            return lhs.get() < rhs.get();
    }
}

/**
 * @brief Convert JSON values to integers with type-specific logic
 * 
 * This function converts JSON values to integers using appropriate
 * conversion rules for each JSON type:
 * - Strings: parsed using atoi()
 * - Numbers: direct conversion
 * - Booleans: true=1, false=0
 * - Containers: returns size
 * - Invalid/null: returns 0
 * 
 * @tparam jsonT JSON value type
 * @param val JSON value to convert
 * @return int Converted integer value
 */
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, int>::type
toIntegerCast(const jsonT& val)
{
    if (yyjson_unlikely(!val.isValid())) return 0;
    
    if (val.isString()) {
        const char* str = val | "";
        return ::atoi(str);
    }
    
    if (val.isInt()) {
        return val.getor(0);
    }
    if (val.isBool()) {
        return val.getor(false);
    }
    if (val.isReal()) {
        return static_cast<int>(val.getor(0.0));
    }
    
    return 0;
}

/**
 * @brief Apply function to JSON value with intelligent parameter mapping
 * 
 * This function provides the implementation for pipe() operations,
 * automatically mapping JSON values to appropriate function parameters:
 * 1. Prefer direct JSON value parameter
 * 2. Map to supported scalar types based on function signature
 * 3. Fallback to getor() with function as default value
 * 
 * @tparam jsonT JSON value type
 * @tparam funcT Function or callable type
 * @param json JSON value to process
 * @param func Function to apply
 * @return auto Result of function application
 * 
 * @throws static_assert if function signature is unsupported
 */
template<typename jsonT, typename funcT>
inline auto pipeApply(const jsonT& json, funcT&& func)
{
    if constexpr (std::is_invocable_v<funcT, const jsonT&>) {
        return std::forward<funcT>(func)(json);
    } else if constexpr (std::is_invocable_v<funcT, const char*>) {
        return std::forward<funcT>(func)(json.getor(kString));
    } else if constexpr (std::is_invocable_v<funcT, std::string>) {
        return std::forward<funcT>(func)(json.getor(std::string{}));
    } else if constexpr (std::is_invocable_v<funcT, double>) {
        return std::forward<funcT>(func)(json.getor(kNumber));
    } else if constexpr (std::is_invocable_v<funcT, int>) {
        return std::forward<funcT>(func)(json.getor(0));
    } else if constexpr (std::is_invocable_v<funcT, int64_t>) {
        return std::forward<funcT>(func)(json.getor(int64_t{}));
    } else if constexpr (std::is_invocable_v<funcT, uint64_t>) {
        return std::forward<funcT>(func)(json.getor(uint64_t{}));
    } else if constexpr (std::is_invocable_v<funcT, bool>) {
        return std::forward<funcT>(func)(json.getor(false));
    } else if constexpr (trait::enable_getor<funcT>::value) {
        // Fallback: treat func as default value for getor
        return json.getor(std::forward<funcT>(func));
    } else {
        static_assert(!std::is_same_v<funcT, funcT>, "Unsupported pipe() callable parameter type");
    }
}

} /* end of namespace yyjson::util */

/* @Part 4: Class Implementations */
/* ======================================================================== */

/* @Section 4.1: Value Methods */
/* ------------------------------------------------------------------------ */

/* @Group 4.1.1: get and getor */
/* ************************************************************************ */

inline bool Value::get(bool& result) const
{
    if (yyjson_likely(isBool())) {
        result = unsafe_yyjson_get_bool(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(int& result) const
{
    if (yyjson_likely(isInt())) {
        result = unsafe_yyjson_get_int(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(int64_t& result) const
{
    if (isSint()) {
        result = unsafe_yyjson_get_sint(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(uint64_t& result) const
{
    if (isUint()) {
        result = unsafe_yyjson_get_uint(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(double& result) const
{
    if (yyjson_likely(isReal())) {
        result = unsafe_yyjson_get_real(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(const char*& result) const
{
    if (yyjson_likely(isString())) {
        result = unsafe_yyjson_get_str(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(std::string& result) const
{
    if (yyjson_likely(isString())) {
        result = unsafe_yyjson_get_str(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(yyjson_val*& result) const
{
    if (yyjson_likely(isValid())) { result = m_val; return true; }
    return false;
}

template<typename T>
inline std::decay_t<T> Value::getor(T&& defaultValue) const
{
    using R = std::decay_t<T>;
    R result{};
    return get(result) ? result : static_cast<R>(defaultValue);
}

inline const char* Value::getor(EmptyString) const
{
    return yyjson_likely(isString()) ? unsafe_yyjson_get_str(m_val) : "";
}

inline double Value::getor(ZeroNumber) const
{
    return toNumber();
}

inline ConstArray Value::getor(EmptyArray) const
{
    return array();
}

inline ConstObject Value::getor(EmptyObject) const
{
    return object();
}

template<typename funcT>
inline auto Value::pipe(funcT&& func) const
{
    return util::pipeApply(*this, std::forward<funcT>(func));
}

/* @Group 4.1.2: size and index/path */
/* ************************************************************************ */

inline size_t Value::size() const
{
    if (yyjson_unlikely(!m_val)) return 0;
    if (yyjson_is_arr(m_val)) return yyjson_arr_size(m_val);
    if (yyjson_is_obj(m_val)) return yyjson_obj_size(m_val);
    return 0;
}

inline Value Value::index(size_t idx) const
{
    if (yyjson_unlikely(!isArray())) return Value(nullptr);
    return Value(yyjson_arr_get(m_val, idx));
}

inline Value Value::index(const char* key, size_t len) const
{
    if (yyjson_unlikely(!isObject() || !key)) return Value(nullptr);
    return Value(yyjson_obj_getn(m_val, key, len));
}

inline Value Value::pathto(const char* path, size_t len) const
{
    if (yyjson_unlikely(!path || len == 0)) return *this;
    if (path[0] == '/') {
        yyjson_val* result = yyjson_ptr_getn(m_val, path, len);
        return Value(result);
    }
    return index(path, len);
}

/* @Group 4.1.3: create iterator */
/* ************************************************************************ */

inline ArrayIterator Value::iterator(size_t startIndex) const
{
    if (yyjson_likely(isArray())) {
        ArrayIterator iter(m_val);
        if (yyjson_likely(startIndex == 0)) return iter;
        return iter.advance(startIndex);
    }
    return ArrayIterator();
}

inline ArrayIterator Value::iterator(int startIndex) const
{
    return iterator(static_cast<size_t>(startIndex));
}

inline ObjectIterator Value::iterator(const char* startKey) const
{
    if (yyjson_likely(isObject())) {
        ObjectIterator iter(m_val);
        if (yyjson_likely(startKey == nullptr || *startKey == '\0')) return iter;
        return iter.advance(startKey);
    }
    return ObjectIterator();
}

inline ArrayIterator Value::iterator(EmptyArray) const
{
    return yyjson_likely(isArray()) ? iterator(size_t(0)) : ArrayIterator();
}

inline ObjectIterator Value::iterator(EmptyObject) const
{
    return yyjson_likely(isObject()) ? iterator(nullptr) : ObjectIterator();
}

inline ArrayIterator Value::beginArray() const
{
    return iterator(size_t(0));
}

inline ArrayIterator Value::endArray() const
{
    return beginArray().end();
}

inline ObjectIterator Value::beginObject() const
{
    return iterator((const char*)nullptr);
}

inline ObjectIterator Value::endObject() const
{
    return beginObject().end();
}

inline ConstArray Value::array() const 
{
    return ConstArray(*this);
}

inline ConstObject Value::object() const 
{
    return ConstObject(*this);
}

/* @Group 4.1.4: others */
/* ************************************************************************ */

inline std::string Value::toString(bool pretty) const
{
    if (yyjson_unlikely(!isValid())) return "";

    if (isString() && !pretty) {
        return std::string(yyjson_get_str(m_val));
    }
    
    uint32_t flags = YYJSON_WRITE_NOFLAG;
    if (pretty) {
        flags |= YYJSON_WRITE_PRETTY;
    }
    
    size_t len = 0;
    char* json_str = yyjson_val_write(m_val, flags, &len);
    if (yyjson_unlikely(!json_str)) return "";
    
    std::string result(json_str, len);
    free(json_str);
    return result;
}

inline int Value::toInteger() const
{
    if (yyjson_unlikely(!isValid())) return 0;
    if (isArray() || isObject())
    {
        return static_cast<int>(size());
    }
    return util::toIntegerCast(*this);
}

inline double Value::toNumber() const
{
    return yyjson_get_num(m_val);
}

inline bool Value::equal(const Value& other) const
{
    return (m_val == other.m_val) || yyjson_equals(m_val, other.m_val);
}

inline bool Value::less(const Value& other) const
{
    return util::lessCompare(*this, other);
}

/* @Section 4.2: Document Methods */
/* ------------------------------------------------------------------------ */

/* @Group 4.2.1: primary manage */
/* ************************************************************************ */

inline Document::Document(yyjson_doc* doc) : m_doc(doc)
{
}

inline Document::Document(const char* str, size_t len/* = 0*/)
{
    if (len == 0 && str) len = strlen(str);
    m_doc = yyjson_read(str, len, 0);
}

#ifndef XYJSON_DISABLE_MUTABLE
inline Document::Document(const MutableDocument& other)
{
    *this = other.freeze();
}
#endif

inline void Document::free()
{
    if (yyjson_likely(m_doc))
    {
        yyjson_doc_free(m_doc);
        m_doc = nullptr;
    }
}

#ifndef XYJSON_DISABLE_MUTABLE
// Convert read-only document to mutable document
inline MutableDocument Document::mutate() const
{
    if (yyjson_unlikely(!isValid())) {
        return MutableDocument((yyjson_mut_doc*)nullptr);
    }

    yyjson_mut_doc* mut_doc = yyjson_doc_mut_copy(m_doc, nullptr);
    return MutableDocument(mut_doc);
}
#endif

/* @Group 4.2.2: read and write */
/* ************************************************************************ */

inline bool Document::read(const char* str, size_t len)
{
    free();
    
    if (len == 0 && str) len = strlen(str);
    m_doc = yyjson_read(str, len, 0);
    return isValid();
}

inline bool Document::read(FILE* fp)
{
    free();
    
    if (yyjson_unlikely(!fp)) return false;
    m_doc = yyjson_read_fp(fp, 0, nullptr, nullptr);
    return isValid();
}

inline bool Document::read(std::ifstream& ifs)
{
    // Check if stream is in a valid state before any operations
    if (yyjson_unlikely(!ifs.good())) return false;
    
    ifs.seekg(0, std::ios::end);
    std::streampos pos = ifs.tellg();
    
    // Check if tellg() failed (returns -1 for invalid files)
    if (pos == -1) return false;
    
    size_t size = static_cast<size_t>(pos);
    ifs.seekg(0, std::ios::beg);
    
    if (size == 0) return false;
    
    std::string content(size, '\0');
    ifs.read(&content[0], size);
    
    return read(content);
}

inline bool Document::readFile(const char* path)
{
    if (yyjson_unlikely(!path)) return false;
    FILE* fp = fopen(path, "rb");
    if (yyjson_unlikely(!fp)) return false;

    bool result = read(fp);
    fclose(fp);
    return result;
}

inline bool Document::write(std::string& output) const
{
    if (yyjson_unlikely(!m_doc)) return false;
    char* json = yyjson_write(m_doc, 0, nullptr);
    if (yyjson_unlikely(!json)) return false;
    output = json;
    std::free(json);
    return true;
}

inline bool Document::write(FILE* fp) const
{
    if (yyjson_unlikely(!m_doc || !fp)) return false;
    return yyjson_write_fp(fp, m_doc, 0, nullptr, nullptr);
}

inline bool Document::write(std::ofstream& ofs) const
{
    std::string content;
    if (yyjson_unlikely(!write(content))) return false;
    
    ofs.seekp(0, std::ios::beg);
    ofs.write(content.c_str(), content.size());
    ofs.flush();
    
    return !ofs.fail();
}

inline bool Document::writeFile(const char* path) const
{
    if (yyjson_unlikely(!path)) return false;
    FILE* fp = fopen(path, "wb");
    if (yyjson_unlikely(!fp)) return false;

    bool result = write(fp);
    fclose(fp);
    return result;
}

#ifndef XYJSON_DISABLE_MUTABLE
/* @Section 4.3: MutableValue Methods */
/* ------------------------------------------------------------------------ */

/* @Group 4.3.1: get and getor */
/* ************************************************************************ */

inline bool MutableValue::get(bool& result) const
{
    if (yyjson_likely(isBool())) {
        result = unsafe_yyjson_get_bool((yyjson_val*)m_val); // yyjson_mut_get_bool
        return true;
    }
    return false;
}

inline bool MutableValue::get(int& result) const
{
    if (yyjson_likely(isInt())) {
        result = unsafe_yyjson_get_int((yyjson_val*)m_val); // yyjson_mut_get_int
        return true;
    }
    return false;
}

inline bool MutableValue::get(int64_t& result) const
{
    if (isSint()) {
        result = unsafe_yyjson_get_sint((yyjson_val*)m_val); // yyjson_mut_get_sint
        return true;
    }
    return false;
}

inline bool MutableValue::get(uint64_t& result) const
{
    if (isUint()) {
        result = unsafe_yyjson_get_uint((yyjson_val*)m_val); // yyjson_mut_get_uint
        return true;
    }
    return false;
}

inline bool MutableValue::get(double& result) const
{
    if (yyjson_likely(isReal())) {
        result = unsafe_yyjson_get_real((yyjson_val*)m_val); // yyjson_mut_get_real
        return true;
    }
    return false;
}

inline bool MutableValue::get(const char*& result) const
{
    if (yyjson_likely(isString())) {
        result = unsafe_yyjson_get_str((yyjson_val*)m_val); // yyjson_mut_get_str
        return true;
    }
    return false;
}

inline bool MutableValue::get(std::string& result) const
{
    if (yyjson_likely(isString())) {
        result = unsafe_yyjson_get_str((yyjson_val*)m_val); // yyjson_mut_get_str
        return true;
    }
    return false;
}

inline bool MutableValue::get(yyjson_mut_val*& result) const
{
    if (yyjson_likely(isValid())) { result = m_val; return true; }
    return false;
}

inline bool MutableValue::get(yyjson_mut_doc*& result) const
{
    if (yyjson_likely(m_doc != nullptr)) { result = m_doc; return true; }
    return false;
}

template<typename T>
inline std::decay_t<T> MutableValue::getor(T&& defaultValue) const
{
    using R = std::decay_t<T>;
    R result{};
    return get(result) ? result : static_cast<R>(defaultValue);
}

inline const char* MutableValue::getor(EmptyString) const
{
    return yyjson_likely(isString()) ? unsafe_yyjson_get_str((yyjson_val*)m_val) : "";
}

inline double MutableValue::getor(ZeroNumber) const
{
    return toNumber();
}

inline MutableArray MutableValue::getor(EmptyArray) const
{
    return array();
}

inline MutableObject MutableValue::getor(EmptyObject) const
{
    return object();
}

template<typename funcT>
inline auto MutableValue::pipe(funcT&& func) const
{
    return util::pipeApply(*this, std::forward<funcT>(func));
}

/* @Group 4.3.2: size and index/path */
/* ************************************************************************ */

inline size_t MutableValue::size() const
{
    if (yyjson_unlikely(!m_val)) return 0;
    if (yyjson_mut_is_arr(m_val)) return yyjson_mut_arr_size(m_val);
    if (yyjson_mut_is_obj(m_val)) return yyjson_mut_obj_size(m_val);
    return 0;
}

inline void MutableValue::clear()
{
    if (yyjson_unlikely(!m_val || !m_doc)) return;

    if (yyjson_mut_is_arr(m_val)) {
        yyjson_mut_arr_clear(m_val);
    }
    else if (yyjson_mut_is_obj(m_val)) {
        yyjson_mut_obj_clear(m_val);
    }
    else if (yyjson_mut_is_str(m_val)) {
        set("");
    }
    else if (yyjson_mut_is_int(m_val) || yyjson_mut_is_sint(m_val) || yyjson_mut_is_uint(m_val)) {
        set(0);
    }
    else if (yyjson_mut_is_real(m_val)) {
        set(0.0);
    }
    // Note: null and bool types are not cleared as they have no meaningful "zero" value
}

inline MutableValue MutableValue::index(size_t idx) const
{
    if (yyjson_unlikely(!isArray())) return MutableValue(nullptr, m_doc);
    return MutableValue(yyjson_mut_arr_get(m_val, idx), m_doc);
}

inline MutableValue MutableValue::index(const char* key, size_t len) const
{
    if (yyjson_unlikely(!isObject() || !key)) return MutableValue(nullptr, m_doc);
    return MutableValue(yyjson_mut_obj_getn(m_val, key, len), m_doc);
}

inline MutableValue MutableValue::index(size_t idx)
{
    if (yyjson_unlikely(!isArray())) return MutableValue(nullptr, m_doc);
    return MutableValue(yyjson_mut_arr_get(m_val, idx), m_doc);
}

inline MutableValue MutableValue::index(const char* key, size_t len)
{
    if (yyjson_unlikely(!isObject() || !key)) return MutableValue(nullptr, m_doc);
    yyjson_mut_val* val = yyjson_mut_obj_getn(m_val, key, len);
    if (yyjson_unlikely(!val)) {
        // Key doesn't exist, insert null value with explicit length
        val = yyjson_mut_null(m_doc);
        yyjson_mut_val* key_val = yyjson_mut_strncpy(m_doc, key, len);
        yyjson_mut_obj_add(m_val, key_val, val);
    }
    return MutableValue(val, m_doc);
}

inline MutableValue MutableValue::pathto(const char* path, size_t len) const
{
    if (yyjson_unlikely(!path || len == 0)) return *this;
    if (path[0] == '/') {
        yyjson_mut_val* result = yyjson_mut_ptr_getn(m_val, path, len);
        return MutableValue(result, m_doc);
    }
    return index(path, len);
}

/* @Group 4.3.3: assignment set */
/* ************************************************************************ */

inline MutableValue& MutableValue::set(const MutableValue& other)
{
    if (this != &other) {
        m_val = other.m_val;
        m_doc = other.m_doc;
    }
    return *this;
}

inline MutableValue& MutableValue::set(std::nullptr_t value)
{
    yyjson_mut_set_null(m_val);
    return *this;
}

inline MutableValue& MutableValue::set(bool value)
{
    yyjson_mut_set_bool(m_val, value);
    return *this;
}

inline MutableValue& MutableValue::set(int value)
{
    yyjson_mut_set_int(m_val, value);
    return *this;
}

inline MutableValue& MutableValue::set(int64_t value)
{
    yyjson_mut_set_sint(m_val, value);
    return *this;
}

inline MutableValue& MutableValue::set(uint64_t value)
{
    yyjson_mut_set_uint(m_val, value);
    return *this;
}

inline MutableValue& MutableValue::set(double value)
{
    yyjson_mut_set_real(m_val, value);
    return *this;
}

inline MutableValue& MutableValue::set(const std::string& value)
{
    return setCopy(value.c_str(), value.size());
}

inline MutableValue& MutableValue::set(StringRef value)
{
    if (value.len == 2)
    {
        if (::strcmp(value, "{}") == 0) { return setObject(); }
        if (::strcmp(value, "[]") == 0) { return setArray(); }
    }
    yyjson_mut_set_strn(m_val, value.str, value.len);
    return *this;
}

inline MutableValue& MutableValue::setCopy(const char* value, size_t len)
{
    if (isValid() && value)
    {
        // copy input value to new node
        // let old node point to the string in the new old.
        // Note the new node and old string is not accessible
        // and only free when the document is freed.
        yyjson_mut_val* newNode = yyjson_mut_strncpy(m_doc, value, len);
        const char* newStr = yyjson_mut_get_str(newNode);
        yyjson_mut_set_str(m_val, newStr);
    }
    return *this;
}

template <size_t N>
inline MutableValue& MutableValue::set(const char(&value)[N])
{
    if (N == 3)
    {
        if (::strcmp(value, "{}") == 0) { return setObject(); }
        if (::strcmp(value, "[]") == 0) { return setArray(); }
    }
    yyjson_mut_set_strn(m_val, value, N-1);
    return *this;
}

template <size_t N>
inline MutableValue& MutableValue::set(char(&value)[N])
{
    return setCopy(value, ::strlen(value));
}

template<typename T>
inline typename std::enable_if<trait::is_cstr_type<T>(), MutableValue&>::type
MutableValue::set(T value)
{
    return setCopy(value, ::strlen(value));
}

inline MutableValue& MutableValue::setArray()
{
    if (yyjson_unlikely(isValid()))
    {
        yyjson_mut_set_arr(m_val);
    }
    return *this;
}

inline MutableValue& MutableValue::setObject()
{
    if (yyjson_unlikely(isValid()))
    {
        yyjson_mut_set_obj(m_val);
    }
    return *this;
}

/* @Group 4.3.4: array and object add */
/* ************************************************************************ */

inline MutableValue& MutableValue::append(yyjson_mut_val* value)
{
    yyjson_mut_arr_append(m_val, value); // the C API would check argument
    return *this;
}

template <typename T>
inline MutableValue& MutableValue::append(T&& value)
{
    return append(util::create(m_doc, std::forward<T>(value)));
}

inline MutableValue& MutableValue::add(yyjson_mut_val* key, yyjson_mut_val* value)
{
    yyjson_mut_obj_add(m_val, key, value); // the C API would check argument
    return *this;
}

inline MutableValue& MutableValue::add(KeyValue&& kv)
{
    if (yyjson_likely(yyjson_mut_obj_add(m_val, kv.key, kv.value)))
    {
        kv.key = nullptr;
        kv.value = nullptr;
    }
    return *this;
}

template<typename keyT, typename valT>
inline MutableValue& MutableValue::add(keyT&& key, valT&& value)
{
    if (yyjson_likely(isObject())) {
        yyjson_mut_val* keyNode = util::createKey(m_doc, std::forward<keyT>(key));
        if (yyjson_unlikely(!keyNode)) {
            return *this;
        }
        
        yyjson_mut_val* valNode = util::create(m_doc, std::forward<valT>(value));
        yyjson_mut_obj_add(m_val, keyNode, valNode);
    }
    return *this;
}

/* @Group 4.3.5: smart input and tag */
/* ************************************************************************ */

inline KeyValue MutableValue::tag(MutableValue&& key) &&
{
    // Require same document pool and string type; do not copy
    yyjson_mut_val* keyNode = nullptr;
    if (yyjson_likely(key.m_doc == m_doc && key.isString()))
    {
        keyNode = key.m_val;
    }
    auto ret = KeyValue(keyNode, m_val);
    m_val = nullptr;
    key.m_val = nullptr;
    return ret;
}

template <typename keyT>
inline KeyValue MutableValue::tag(keyT&& key) &&
{
    yyjson_mut_val* keyNode = util::createKey(m_doc, std::forward<keyT>(key));
    auto ret = KeyValue(keyNode, m_val);
    m_val = nullptr;
    return ret;
}

// Specialization for KeyValue - can only be added to object
inline MutableValue& MutableValue::push(KeyValue&& kv)
{
    if (yyjson_likely(isObject()))
    {
        return add(std::forward<KeyValue>(kv));
    }
    return *this;
}

#ifndef XYJSON_DISABLE_CHAINED_INPUT
template <typename keyT>
inline bool MutableValue::pushKey(keyT&& key)
{
    m_pendingKey = util::createKey(m_doc, std::forward<keyT>(key));
    return false;
}

template <typename T>
inline bool MutableValue::pushValue(T&& value)
{
    if (yyjson_likely(m_pendingKey == nullptr)) {
        return false;
    }
    yyjson_mut_val* succeedVal = util::create(m_doc, std::forward<T>(value));
    add(m_pendingKey, succeedVal);
    m_pendingKey = nullptr;
    return true;
}
#endif

// General template for other types
template <typename T>
inline MutableValue& MutableValue::push(T&& value)
{
    if (isArray()) {
        return append(std::forward<T>(value));
    }
#ifndef XYJSON_DISABLE_CHAINED_INPUT
    else if (isObject()) {
        bool ok = pushValue(std::forward<T>(value)) || pushKey(std::forward<T>(value));
    }
#endif
    return *this;
}

// Pop method for array: remove last element and return it
inline MutableValue& MutableValue::pop(MutableValue& result)
{
    if (yyjson_likely(isArray())) {
        size_t n = size();
        if (yyjson_likely(n > 0)) {
            MutableArrayIterator iter = beginArray();
            iter.advance(n - 1);
            result = iter.remove();
            return *this;
        }
    }
    result = MutableValue(nullptr, m_doc);
    return *this;
}

// Pop method for object: remove last key-value pair and return it
inline MutableValue& MutableValue::pop(KeyValue& result)
{
    if (yyjson_likely(isObject())) {
        size_t n = size();
        if (yyjson_likely(n > 0)) {
            MutableObjectIterator iter = beginObject();
            iter.advance(n - 1);
            result = iter.remove();
            return *this;
        }
    }
    result = KeyValue(nullptr, nullptr);
    return *this;
}

/* @Group 4.3.6: create iterator */
/* ************************************************************************ */

inline MutableArrayIterator MutableValue::iterator(size_t startIndex) const
{
    if (yyjson_likely(isArray())) {
        MutableArrayIterator iter(m_val, m_doc);
        if (yyjson_likely(startIndex == 0)) return iter;
        return iter.advance(startIndex);
    }
    return MutableArrayIterator();
}

inline MutableArrayIterator MutableValue::iterator(int startIndex) const
{
    return iterator(static_cast<size_t>(startIndex));
}

inline MutableObjectIterator MutableValue::iterator(const char* startKey) const
{
    if (yyjson_likely(isObject())) {
        MutableObjectIterator iter(m_val, m_doc);
        if (yyjson_likely(startKey == nullptr || *startKey == '\0')) return iter;
        return iter.advance(startKey);
    }
    return MutableObjectIterator();
}

inline MutableArrayIterator MutableValue::iterator(EmptyArray) const
{
    return yyjson_likely(isArray()) ? iterator(size_t(0)) : MutableArrayIterator();
}

inline MutableObjectIterator MutableValue::iterator(EmptyObject) const
{
    return yyjson_likely(isObject()) ? iterator(nullptr) : MutableObjectIterator();
}

inline MutableArrayIterator MutableValue::beginArray() const
{
    return iterator(size_t(0));
}

inline MutableArrayIterator MutableValue::endArray() const
{
    return beginArray().end();
}

inline MutableObjectIterator MutableValue::beginObject() const
{
    return iterator((const char*)nullptr);
}

inline MutableObjectIterator MutableValue::endObject() const
{
    return beginObject().end();
}

inline MutableArray MutableValue::array() const
{
    return MutableArray(*this);
}

inline MutableObject MutableValue::object() const
{
    return MutableObject(*this);
}

/* @Group 4.3.7: others */
/* ************************************************************************ */

inline std::string MutableValue::toString(bool pretty) const
{
    if (yyjson_unlikely(!isValid())) return "";

    if (isString() && !pretty) {
        return std::string(yyjson_mut_get_str(m_val));
    }

    uint32_t flags = YYJSON_WRITE_NOFLAG;
    if (pretty) {
        flags |= YYJSON_WRITE_PRETTY;
    }

    size_t len = 0;
    char* json_str = yyjson_mut_val_write(m_val, flags, &len);
    if (yyjson_unlikely(!json_str)) return "";

    std::string result(json_str, len);
    free(json_str);
    return result;
}

inline int MutableValue::toInteger() const
{
    if (yyjson_unlikely(!isValid())) return 0;
    if (isArray() || isObject())
    {
        return static_cast<int>(size());
    }
    return util::toIntegerCast(*this);
}

inline double MutableValue::toNumber() const
{
    return yyjson_mut_get_num(m_val);
}

inline bool MutableValue::equal(const MutableValue& other) const
{
    return (m_val == other.m_val) || yyjson_mut_equals(m_val, other.m_val);
}

inline bool MutableValue::less(const MutableValue& other) const
{
    return util::lessCompare(*this, other);
}

/* @Section 4.4: MutableDocument Methods */
/* ------------------------------------------------------------------------ */

/* @Group 4.4.1: primary manage */
/* ************************************************************************ */

inline MutableDocument::MutableDocument(yyjson_mut_doc* doc) : m_doc(doc)
{
}

inline MutableDocument::MutableDocument(const char* str, size_t len/* = 0*/)
{
    if (len == 0 && str) len = strlen(str);
    yyjson_doc* doc = yyjson_read(str, len, 0);
    if (yyjson_likely(doc)) {
        m_doc = yyjson_doc_mut_copy(doc, nullptr);
        yyjson_doc_free(doc);
    } else {
        m_doc = nullptr;
    }
}

inline MutableDocument::MutableDocument(const Document& other)
{
    *this = other.mutate();
}

inline void MutableDocument::free()
{
    if (yyjson_likely(m_doc))
    {
        yyjson_mut_doc_free(m_doc);
        m_doc = nullptr;
    }
}

// Convert mutable document to read-only document
inline Document MutableDocument::freeze() const
{
    if (yyjson_unlikely(!isValid())) {
        return Document((yyjson_doc*)nullptr);
    }
    
    yyjson_doc* doc = yyjson_mut_doc_imut_copy(m_doc, nullptr);
    return Document(doc);
}

inline void MutableDocument::setRoot(MutableValue val)
{
    if (yyjson_likely(m_doc))
    {
        yyjson_mut_doc_set_root(m_doc, val.get());
    }
}

/* @Group 4.4.2: read and write */
/* ************************************************************************ */

inline bool MutableDocument::read(const char* str, size_t len)
{
    free();
    
    if (len == 0 && str) len = strlen(str);
    yyjson_doc* doc = yyjson_read(str, len, 0);
    if (doc != nullptr)
    {
        m_doc = yyjson_doc_mut_copy(doc, nullptr);
        yyjson_doc_free(doc);
    }
    
    return isValid();
}

inline bool MutableDocument::read(FILE* fp)
{
    free();
    
    if (yyjson_unlikely(!fp)) return false;
    yyjson_doc* doc = yyjson_read_fp(fp, 0, nullptr, nullptr);
    if (doc != nullptr)
    {
        m_doc = yyjson_doc_mut_copy(doc, nullptr);
        yyjson_doc_free(doc);
    }
    
    return isValid();
}

inline bool MutableDocument::read(std::ifstream& ifs)
{
    // Check if stream is in a valid state before any operations
    if (yyjson_unlikely(!ifs.good())) return false;
    
    ifs.seekg(0, std::ios::end);
    std::streampos pos = ifs.tellg();
    
    // Check if tellg() failed (returns -1 for invalid files)
    if (pos == -1) return false;
    
    size_t size = static_cast<size_t>(pos);
    ifs.seekg(0, std::ios::beg);
    
    if (size == 0) return false;
    
    std::string content(size, '\0');
    ifs.read(&content[0], size);
    
    return read(content);
}

inline bool MutableDocument::readFile(const char* path)
{
    if (yyjson_unlikely(!path)) return false;
    FILE* fp = fopen(path, "rb");
    if (yyjson_unlikely(!fp)) return false;

    bool result = read(fp);
    fclose(fp);
    return result;
}

inline bool MutableDocument::write(std::string& output) const
{
    if (yyjson_unlikely(!m_doc)) return false;
    char* json = yyjson_mut_write(m_doc, 0, nullptr);
    if (yyjson_unlikely(!json)) return false;
    output = json;
    std::free(json);
    return true;
}

inline bool MutableDocument::write(FILE* fp) const
{
    if (yyjson_unlikely(!m_doc || !fp)) return false;
    return yyjson_mut_write_fp(fp, m_doc, 0, nullptr, nullptr);
}

inline bool MutableDocument::write(std::ofstream& ofs) const
{
    std::string content;
    if (yyjson_unlikely(!write(content))) return false;
    
    ofs.seekp(0, std::ios::beg);
    ofs.write(content.c_str(), content.size());
    ofs.flush();
    
    return !ofs.fail();
}

inline bool MutableDocument::writeFile(const char* path) const
{
    if (yyjson_unlikely(!path)) return false;
    FILE* fp = fopen(path, "wb");
    if (yyjson_unlikely(!fp)) return false;
    
    bool result = write(fp);
    fclose(fp);
    return result;
}

/* @Group 4.4.3: create mutable value */
/* ************************************************************************ */

inline MutableValue MutableDocument::create(yyjson_mut_val* value) const
{ 
    if (yyjson_likely(value && m_doc))
    {
        return MutableValue(value, m_doc); 
    }
    return MutableValue(nullptr, m_doc);
}

inline MutableValue MutableDocument::create() const
{ 
    return create(yyjson_mut_null(m_doc)); 
}

template <typename T>
inline MutableValue MutableDocument::create(T&& value) const
{
    return create(util::create(m_doc, std::forward<T>(value)));
}

#endif // XYJSON_DISABLE_MUTABLE

/* @Section 4.5: ArrayIterator Methods */
/* ------------------------------------------------------------------------ */

inline ArrayIterator::ArrayIterator(yyjson_val* root) : m_arr(root)
{
    if (yyjson_likely(root)) {
        yyjson_arr_iter_init(root, &m_iter);
    }
}

inline ArrayIterator& ArrayIterator::next()
{
    yyjson_arr_iter_next(&m_iter);
    return *this;
}

// Backward iteration is O(N) operation: reset to beginning and advance idx-1 steps
inline ArrayIterator& ArrayIterator::prev()
{
    if (yyjson_unlikely(m_iter.idx == 0)) {
        return end(true);
    }
    size_t idx = m_iter.idx - 1;
    return begin().advance(idx);
}

inline ArrayIterator& ArrayIterator::begin()
{
    if (yyjson_likely(m_arr)) {
        yyjson_arr_iter_init(m_arr, &m_iter);
    }
    return *this;
}

inline ArrayIterator& ArrayIterator::advance(size_t steps)
{
    for (size_t i = 0; i < steps && isValid(); i++) { 
        next();
    }
    return *this;
}


/* @Section 4.6: ObjectIterator Methods */
/* ------------------------------------------------------------------------ */

inline ObjectIterator::ObjectIterator(yyjson_val* root)
{
    if (yyjson_likely(root)) {
        yyjson_obj_iter_init(root, &m_iter);
    }
}

inline ObjectIterator& ObjectIterator::next()
{
    yyjson_obj_iter_next(&m_iter);
    return *this;
}

// Backward iteration is O(N) operation: reset to beginning and advance idx-1 steps
inline ObjectIterator& ObjectIterator::prev()
{
    if (yyjson_unlikely(m_iter.idx == 0)) {
        return end(true);
    }
    size_t idx = m_iter.idx - 1;
    return begin().advance(idx);
}

inline ObjectIterator& ObjectIterator::begin()
{
    if (yyjson_likely(m_iter.obj)) {
        yyjson_obj_iter_init(m_iter.obj, &m_iter);
    }
    return *this;
}

inline ObjectIterator& ObjectIterator::advance(size_t steps)
{
    for (size_t i = 0; i < steps && isValid(); i++) { 
        next();
    }
    return *this;
}

inline ObjectIterator& ObjectIterator::advance(const char* key)
{
    if (yyjson_unlikely(!key || !*key)) return *this;
    
    while (isValid()) {
        const char* currentKey = name();
        if (currentKey && ::strcmp(currentKey, key) == 0) {
            break;
        }
        next();
    }
    return *this;
}

// ObjectIterator fast seek implementation using yyjson_obj_iter_getn
inline Value ObjectIterator::seek(const char* key, size_t key_len)
{
    yyjson_val* val = yyjson_obj_iter_getn(&m_iter, key, key_len);
    return Value(val);
}

#ifndef XYJSON_DISABLE_MUTABLE
/* @Section 4.7: MutableArrayIterator Methods */
/* ------------------------------------------------------------------------ */

inline MutableArrayIterator::MutableArrayIterator(yyjson_mut_val* root, yyjson_mut_doc* doc) : m_doc(doc)
{
    if (yyjson_likely(root)) {
        yyjson_mut_arr_iter_init(root, &m_iter);
        if (yyjson_likely(m_iter.idx < m_iter.max)) {
            m_iter.pre = (yyjson_mut_val*)m_iter.arr->uni.ptr;  // prev = tail element
            m_iter.cur = m_iter.pre->next;                      // cur = first element
        }
    }
}

inline MutableArrayIterator& MutableArrayIterator::next()
{
    if (yyjson_likely(m_iter.idx < m_iter.max)) {
        m_iter.pre = m_iter.cur;          // current element becomes previous
        m_iter.cur = m_iter.cur->next;    // advance to next element
        m_iter.idx++;                     // increment index
    }
    return *this;
}

// Backward iteration is O(N) operation: reset to beginning and advance idx-1 steps
// If at beginning, move to fast end (circular linked list)
inline MutableArrayIterator& MutableArrayIterator::prev()
{
    if (yyjson_unlikely(m_iter.idx == 0)) {
        return end();
    }
    size_t idx = m_iter.idx - 1;
    return begin().advance(idx);
}

inline MutableArrayIterator& MutableArrayIterator::begin()
{
    if (yyjson_likely(m_iter.arr)) {
        yyjson_mut_arr_iter_init(m_iter.arr, &m_iter);
        if (yyjson_likely(m_iter.idx < m_iter.max)) {
            m_iter.pre = (yyjson_mut_val*)m_iter.arr->uni.ptr;  // prev = tail element
            m_iter.cur = m_iter.pre->next;                      // cur = first element
        }
    }
    return *this;
}

inline MutableArrayIterator& MutableArrayIterator::advance(size_t steps)
{
    for (size_t i = 0; i < steps && isValid(); i++) { 
        next();
    }
    return *this;
}

inline bool MutableArrayIterator::insert(yyjson_mut_val* val)
{
    // allow idx == max to insert at the end
    if (yyjson_unlikely(!val || !m_iter.arr || m_iter.idx > m_iter.max)) {
        return false;
    }

    if (m_iter.max == 0) {
        // first element, form self-loop
        val->next = val;
        m_iter.pre = val;
        m_iter.cur = val;
        m_iter.arr->uni.ptr = val;
    }
    else {
        // insert before current element (iterator points to new element)
        yyjson_mut_val *prev = m_iter.pre;
        yyjson_mut_val *next = m_iter.cur;
        prev->next = val;
        val->next = next;
        m_iter.cur = val;

        // if inserting at the end, update arr->uni.ptr
        if (m_iter.idx == m_iter.max) {
            m_iter.arr->uni.ptr = val;
        }
    }
    unsafe_yyjson_set_len(m_iter.arr, ++m_iter.max);
    return true;
}

template<typename T>
inline bool MutableArrayIterator::insert(T&& value)
{
    // Create value from template type and forward to native insert
    yyjson_mut_val* val = util::create(m_doc, std::forward<T>(value));
    return insert(val);
}

/// Remove current element and return it
inline MutableValue MutableArrayIterator::remove()
{
    yyjson_mut_val* removed_val = nullptr;
    if (yyjson_likely(isValid() && m_iter.cur)) {
        removed_val = m_iter.cur;

        if (m_iter.max == 1) {
            // remove the last element
            m_iter.cur = nullptr;
            m_iter.pre = nullptr;
            m_iter.arr->uni.ptr = nullptr;
        }
        else {
            // remove current element, let cur point to next element
            m_iter.pre->next = m_iter.cur->next;
            m_iter.cur = m_iter.pre->next;
        }

        m_iter.max--;
        unsafe_yyjson_set_len(m_iter.arr, m_iter.max);
    }
    return MutableValue(removed_val, m_doc);
}


/* @Section 4.8: MutableObjectIterator Methods */
/* ------------------------------------------------------------------------ */

inline MutableObjectIterator::MutableObjectIterator(yyjson_mut_val* root, yyjson_mut_doc* doc) : m_doc(doc)
{
    if (yyjson_likely(root)) {
        yyjson_mut_obj_iter_init(root, &m_iter);
        if (yyjson_likely(m_iter.idx < m_iter.max)) {
            m_iter.pre = (yyjson_mut_val*)m_iter.obj->uni.ptr;  // prev = tail key
            m_iter.cur = m_iter.pre->next->next;                // cur = first key
        }
    }
}

inline MutableObjectIterator& MutableObjectIterator::next()
{
    // directly operate on pointers, advance by two steps (key->val->next key)
    if (yyjson_likely(m_iter.idx < m_iter.max)) {
        m_iter.pre = m_iter.cur;                   // current key becomes previous
        m_iter.cur = m_iter.cur->next->next;       // advance to next key (skip value)
        m_iter.idx++;                              // increment index
    }
    return *this;
}

// Backward iteration is O(N) operation: reset to beginning and advance idx-1 steps
// If at beginning, move to fast end (circular linked list)
inline MutableObjectIterator& MutableObjectIterator::prev()
{
    if (yyjson_unlikely(m_iter.idx == 0)) {
        return end();
    }
    size_t idx = m_iter.idx - 1;
    return begin().advance(idx);
}

inline MutableObjectIterator& MutableObjectIterator::begin()
{
    if (yyjson_likely(m_iter.obj)) {
        yyjson_mut_obj_iter_init(m_iter.obj, &m_iter);
        if (yyjson_likely(m_iter.idx < m_iter.max)) {
            m_iter.pre = (yyjson_mut_val*)m_iter.obj->uni.ptr;  // prev = tail key
            m_iter.cur = m_iter.pre->next->next;                // cur = first key
        }
    }
    return *this;
}

inline MutableObjectIterator& MutableObjectIterator::advance(size_t steps)
{
    for (size_t i = 0; i < steps && isValid(); i++) { 
        next();
    }
    return *this;
}

inline MutableObjectIterator& MutableObjectIterator::advance(const char* key)
{
    if (yyjson_unlikely(!key || !*key)) return *this;
    
    while (isValid()) {
        const char* currentKey = name();
        if (currentKey && ::strcmp(currentKey, key) == 0) {
            break;
        }
        next();
    }
    return *this;
}

// MutableObjectIterator fast seek implementation using yyjson_mut_obj_iter_getn
inline MutableValue MutableObjectIterator::seek(const char* key, size_t key_len)
{
    // adapt from yyjson_mut_obj_iter_getn to our iterator semantics
    if (yyjson_unlikely(!key || key_len == 0 || m_iter.max == 0)) {
        return MutableValue();
    }

    yyjson_mut_val* cur = m_iter.cur;
    yyjson_mut_val* pre = m_iter.pre;

    for (size_t i = 0; i < m_iter.max; ++i) {
        if (unsafe_yyjson_equals_strn(cur, key, key_len)) {
            m_iter.pre = cur;
            m_iter.cur = cur->next->next;
            m_iter.idx += i + 1;
            if (yyjson_unlikely(m_iter.idx > m_iter.max)) {
                m_iter.idx -= m_iter.max;  // wrap around
            }
            return MutableValue(cur->next, m_doc);
        }
        pre = cur;
        cur = cur->next->next;
    }

    return MutableValue();  // not found
}

// MutableObjectIterator insert implementation
inline bool MutableObjectIterator::insert(yyjson_mut_val* key, yyjson_mut_val* val)
{
    if (yyjson_unlikely(!key || !val || !m_iter.obj)) {
        return false;
    }

    // Insert new key-value pair at current iterator position
    // The structure is: ... -> pre_key -> pre_val -> (key -> val) -> cur_key -> cur_val -> ...

    if (m_iter.max == 0) { // insert the first kv
        key->next = val;
        val->next = key;  // Close the circle: key -> val -> key
        m_iter.cur = key;
        m_iter.obj->uni.ptr = key;  // Update object tail pointer (last key)
    }
    else {
        yyjson_mut_val* prev_key = m_iter.pre;
        yyjson_mut_val* prev_val = prev_key->next;
        prev_val->next = key;
        key->next = val;
        val->next = m_iter.cur;
        m_iter.cur = key;

        if (m_iter.idx == m_iter.max) { // insert at the end
            m_iter.obj->uni.ptr = key;  // keep obj point to end key
        }
    }

    m_iter.max++;
    unsafe_yyjson_set_len(m_iter.obj, m_iter.max);
    return true;
}

template<typename K, typename V>
inline bool MutableObjectIterator::insert(K&& key, V&& value)
{
    // Create key and value from template types
    yyjson_mut_val* key_val = util::createKey(m_doc, std::forward<K>(key));
    yyjson_mut_val* value_val = util::create(m_doc, std::forward<V>(value));
    return insert(key_val, value_val);
}

inline bool MutableObjectIterator::insert(KeyValue&& kv_pair)
{
    bool result = insert(kv_pair.key, kv_pair.value);
    if (yyjson_likely(result)) {
        kv_pair.key = nullptr;
        kv_pair.value = nullptr;
    }
    return result;
}

#ifndef XYJSON_DISABLE_CHAINED_INPUT
// Chained insertion support: insertKey
template<typename keyT>
inline bool MutableObjectIterator::insertKey(keyT&& key)
{
    m_pendingKey = util::createKey(m_doc, std::forward<keyT>(key));
    return false;
}

// Chained insertion support: insertValue
template<typename valT>
inline bool MutableObjectIterator::insertValue(valT&& value)
{
    if (m_pendingKey == nullptr) {
        return false;
    }
    yyjson_mut_val* valueNode = util::create(m_doc, std::forward<valT>(value));
    if (yyjson_unlikely(!valueNode)) {
        return false;
    }
    bool result = insert(m_pendingKey, valueNode);
    m_pendingKey = nullptr;
    return result;
}

// Insert method for chained insertion - handles key/value logic internally
// Returns true if a value was inserted (meaning we should advance)
// Returns false if a key was stored (meaning we should wait for value) or both failed
template<typename T>
inline bool MutableObjectIterator::insert(T&& arg)
{
    return insertValue(std::forward<T>(arg)) || insertKey(std::forward<T>(arg));
}
#endif

// MutableObjectIterator remove implementation
inline KeyValue MutableObjectIterator::remove()
{
    yyjson_mut_val* removed_key = nullptr;
    yyjson_mut_val* removed_val = nullptr;

    if (yyjson_likely(isValid() && m_iter.cur)) {
        removed_key = m_iter.cur;
        removed_val = m_iter.cur->next;

        if (m_iter.max == 1) {
            // remove the last key-value pair
            m_iter.cur = nullptr;
            m_iter.pre = nullptr;
            m_iter.obj->uni.ptr = nullptr;
        }
        else {
            // remove current key-value pair from circular linked list
            // Kprev -> Vprev -> Kcur -> Vcur -> Knext -> Vnext -> ...
            // becomes: Kprev -> Vprev -> Knext -> Vnext -> ...
            yyjson_mut_val *key_prev = m_iter.pre;
            yyjson_mut_val *val_prev = key_prev->next;
            yyjson_mut_val *key_next = m_iter.cur->next->next;
            val_prev->next = key_next;

            // move iterator to next key
            m_iter.cur = key_next;
        }

        m_iter.max--;
        unsafe_yyjson_set_len(m_iter.obj, m_iter.max);
    }
    return KeyValue(removed_key, removed_val);
}

#endif // XYJSON_DISABLE_MUTABLE

/* @Part 5: Operator Interface */
/* ======================================================================== */

/* @Section 5.1: Primary Path Access */
/* ------------------------------------------------------------------------ */

// Value and MutableValue path operators
// `json / path` --> `json.pathto(path)`
template <typename jsonT, typename T>
inline typename std::enable_if<trait::is_value<jsonT>::value, jsonT>::type
operator/(const jsonT& json, T&& path)
{
    return json.pathto(std::forward<T>(path));
}

// Value and MutableValue extraction operators
// `json | def` --> `json.getor(def)` : return json value or default value
template<typename jsonT, typename T>
inline typename std::enable_if<
    trait::is_value<jsonT>::value &&
    trait::enable_getor<T>::value,
    std::decay_t<T>
>::type
operator|(const jsonT& json, T&& defaultValue)
{
    return json.getor(std::forward<T>(defaultValue));
}

// Overloads for sentinels kString and kNumber
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, const char*>::type
operator|(const jsonT& json, EmptyString)
{
    return json.getor(kString);
}

template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, double>::type
operator|(const jsonT& json, ZeroNumber)
{
    return json.getor(kNumber);
}

// Simple overloads for kArray and kObject sentinels
inline ConstArray operator|(const Value& json, EmptyArray)
{
    return json.getor(kArray);
}

inline ConstObject operator|(const Value& json, EmptyObject)
{
    return json.getor(kObject);
}

#ifndef XYJSON_DISABLE_MUTABLE
inline MutableArray operator|(const MutableValue& json, EmptyArray)
{
    return json.getor(kArray);
}

inline MutableObject operator|(const MutableValue& json, EmptyObject)
{
    return json.getor(kObject);
}
#endif

// `dest |= json` --> `dest = json | dest`;
template <typename valueT, typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, valueT&>::type
operator|=(valueT& dest, const jsonT& json)
{
    json.get(dest);
    return dest;
}

// `json >> dest` --> `json.get(dest)` : return if get successfully
template <typename valueT, typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator>>(const jsonT& json, valueT& dest)
{
    return json.get(dest);
}

template<typename jsonT, typename funcT, typename ifT = typename std::enable_if<
    // constraint by the not used 3rd optional type parameter
    trait::is_value<jsonT>::value && !trait::enable_getor<funcT>::value
//  && trait::is_callable_type_v<funcT>
    >::type >
inline auto operator|(const jsonT& json, funcT&& func)
{
    return json.pipe(std::forward<funcT>(func));
}

// Value and MutableValue type checking operators
// `json & type` --> `json.isType(type)` : return bool for type checking
template<typename jsonT, typename T>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator&(const jsonT& json, const T& type)
{
    return json.isType(type);
}

// Specialization for const char* to avoid template deduction issues
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator&(const jsonT& json, const char* type)
{
    return json.isType(type);
}

/* @Section 5.2: Conversion Unary Operator */
/* ------------------------------------------------------------------------ */

// `+json` --> `json.toInteger()` : convert any json to int
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, int>::type
operator+(const jsonT& json)
{
    return json.toInteger();
}

// `-json` --> `json.toString()`
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, std::string>::type
operator-(const jsonT& json)
{
    return json.toString();
}

// `*doc` --> `doc.root()` : returns root value
template<typename docT>
inline typename std::enable_if<trait::is_document<docT>::value, typename docT::value_type>::type
operator*(docT& doc)
{
    return doc.root();
}

template<typename docT>
inline typename std::enable_if<trait::is_document<docT>::value, typename docT::value_type>::type
operator*(const docT& doc)
{
    return doc.root();
}

// `~doc` --> `doc.mutate()` : Document --> MutableDocument
#ifndef XYJSON_DISABLE_MUTABLE
inline MutableDocument operator~(const Document& doc)
{
    return doc.mutate();
}

// `~doc` --> `doc.freeze()` : MutableDocument --> Document
inline Document operator~(const MutableDocument& doc)
{
    return doc.freeze();
}
#endif

/* @Section 5.3: Comparison Operator */
/* ------------------------------------------------------------------------ */

// Value and MutableValue comparison operators
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator==(const jsonT& lhs, const jsonT& rhs)
{
    return lhs.equal(rhs);
}

// Specialization for const char* to avoid pointer comparison issues
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator==(const jsonT& json, const char* scalar)
{
    return (json & scalar) && (::strcmp(json | "", scalar) == 0);
}

template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator==(const char* scalar, const jsonT& json)
{
    return json == scalar; // Use the reverse implementation
}

// JSON value and scalar type comparison operators
template<typename jsonT, typename scalarT>
inline typename std::enable_if<trait::is_value<jsonT>::value && !trait::is_value<scalarT>::value, bool>::type
operator==(const jsonT& json, const scalarT& scalar)
{
    return (json & scalar) && ((json | scalar) == scalar);
}

// Scalar type and JSON value comparison operators
template<typename scalarT, typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value && !trait::is_value<scalarT>::value, bool>::type
operator==(const scalarT& scalar, const jsonT& json)
{
    return json == scalar; // Use the reverse implementation
}

// Iterator comparison operators
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, bool>::type
operator==(const iteratorT& lhs, const iteratorT& rhs)
{
    return lhs.equal(rhs);
}

template <typename leftT, typename rightT>
inline bool operator!=(const leftT& lhs, const rightT& rhs)
{
    return !(lhs == rhs);
}

// Less-than comparison operator for Value and MutableValue
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator<(const jsonT& lhs, const jsonT& rhs)
{
    return lhs.less(rhs);
}

// Other comparison operators
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator>(const jsonT& lhs, const jsonT& rhs)
{
    return rhs < lhs;
}

template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator<=(const jsonT& lhs, const jsonT& rhs)
{
    return !(rhs < lhs);
}

template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, bool>::type
operator>=(const jsonT& lhs, const jsonT& rhs)
{
    return !(lhs < rhs);
}

/* @Section 5.4: Create and Bind KeyValue */
/* ------------------------------------------------------------------------ */
#ifndef XYJSON_DISABLE_MUTABLE

// `doc * value` --> `doc.create(value)`
template <typename T>
inline MutableValue operator*(const MutableDocument& doc, T&& value)
{
    return doc.create(std::forward<T>(value));
}

template <typename T>
inline typename std::enable_if<trait::is_key_v<T>, KeyValue>::type
operator*(MutableValue&& json, T&& key)
{
    return std::move(json).tag(std::forward<T>(key));
}

template <typename T>
inline typename std::enable_if<trait::is_key_v<T>, KeyValue>::type
operator*(T&& key, MutableValue&& json)
{
    return std::move(json).tag(std::forward<T>(key));
}

inline KeyValue operator*(MutableValue&& key, MutableValue&& value)
{
    return std::move(value).tag(std::move(key));
}

#endif // XYJSON_DISABLE_MUTABLE

/* @Section 5.5: Stream and Input Operator */
/* ------------------------------------------------------------------------ */

// `doc << input` --> `doc.read(input)`
template<typename docT, typename T>
inline typename std::enable_if<trait::is_document<docT>::value, bool>::type
operator<<(docT& doc, T& input)
{
    return doc.read(input);
}

// `doc >> output` --> `doc.write(output)`
template<typename docT, typename T>
inline typename std::enable_if<trait::is_document<docT>::value, bool>::type
operator>>(const docT& doc, T& output)
{
    return doc.write(output);
}

// `std::ostream << json` --> output value as JSON string
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, std::ostream&>::type
operator<<(std::ostream& os, const jsonT& json)
{
    return os << json.toString();
}

#ifndef XYJSON_DISABLE_MUTABLE

// MutableValue array/object input operators
// `json << value` --> `json.push(value)`
template <typename T>
inline MutableValue& operator<<(MutableValue& json, T&& value)
{
    return json.push(std::forward<T>(value));
}

// Rvalue version for MutableValue, delegates to lvalue version
template <typename T>
inline MutableValue& operator<<(MutableValue&& json, T&& value)
{
    return json.push(std::forward<T>(value));
}

// MutableValue array/object output operators
// `json >> value` --> `json.pop(value)` for array
inline MutableValue& operator>>(MutableValue& json, MutableValue& result)
{
    return json.pop(result);
}

// `json >> kv` --> `json.pop(kv)` for object
inline MutableValue& operator>>(MutableValue& json, KeyValue& result)
{
    return json.pop(result);
}

// Rvalue version for MutableValue, delegates to lvalue version
inline MutableValue& operator>>(MutableValue&& json, MutableValue& result)
{
    return json.pop(result);
}

// Rvalue version for KeyValue, delegates to lvalue version
inline MutableValue& operator>>(MutableValue&& json, KeyValue& result)
{
    return json.pop(result);
}

#endif // XYJSON_DISABLE_MUTABLE

/* @Section 5.6: Iterator Creation and Operation */
/* ------------------------------------------------------------------------ */

// Iterator creation operator (json % index_or_key)
// `json % index` --> `json.iterator(index)` for array
// `json % key` --> `json.iterator(key)` for object
template<typename jsonT, typename ArgT, typename ifT = std::enable_if_t<trait::is_value<jsonT>::value>>
inline auto operator%(const jsonT& json, ArgT&& arg)
{
    return json.iterator(std::forward<ArgT>(arg));
}

// Prefix increment operator (++iter)
// `++iter` --> `iter.next()`
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT&>::type
operator++(iteratorT& iter)
{
    return iter.next();
}

// Postfix increment operator (iter++)
// `iter++` --> `iter.next()` (return old copy)
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT>::type
operator++(iteratorT& iter, int)
{
    iteratorT old = iter;
    iter.next();
    return old;
}

// Prefix decrement operator (--iter)
// `--iter` --> `iter.prev()`
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT&>::type
operator--(iteratorT& iter)
{
    return iter.prev();
}

// Postfix decrement operator (iter--)
// `iter--` --> `iter.prev()` (return old copy)
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT>::type
operator--(iteratorT& iter, int)
{
    iteratorT old = iter;
    iter.prev();
    return old;
}

// Iterator advance operator (+= +)
// `iter += step` --> `iter.advance(step)`
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT&>::type
operator+=(iteratorT& iter, size_t step)
{
    return iter.advance(step);
}

// `iter + step` --> `copy iter.advance(step)`
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT>::type
operator+(iteratorT& iter, size_t step)
{
    iteratorT copy = iter;
    return copy.advance(step);
}

// Iterator jump operators (%= %)
// `iter %= target` --> `iter.begin().advance(target)` for position jump
template<typename iteratorT, typename T>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT&>::type
operator%=(iteratorT& iter, const T& target)
{
    return iter.begin().advance(target);
}

// `iter % target` --> `copy iter.begin().advance(target)` for position jump
template<typename iteratorT, typename T>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT>::type
operator%(iteratorT& iter, const T& target)
{
    iteratorT copy = iter;
    return copy.begin().advance(target);
}

// Iterator unary operator: +iter (get current index)
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, size_t>::type
operator+(const iteratorT& iter)
{
    return iter.index();
}

// Iterator unary operator: -iter (get current key name)
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, const char*>::type
operator-(const iteratorT& iter)
{
    return iter.name();
}

// Iterator unary operator: ~iter (get current key node)
template<typename iteratorT>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, typename iteratorT::json_type>::type
operator~(const iteratorT& iter)
{
    return iter.key();
}

#ifndef XYJSON_DISABLE_MUTABLE

// Iterator insert operator: iter << value (calls iter.insert(value))
template<typename T>
inline MutableArrayIterator& operator<<(MutableArrayIterator& iter, T&& value)
{
    // for chained insertion, need next after insert.
    bool result = iter.insert(std::forward<T>(value)) && iter.next();
    return iter;
}

// Iterator remove operator: iter >> value (calls iter.remove() and stores removed value)
inline MutableArrayIterator& operator>>(MutableArrayIterator& iter, MutableValue& value)
{
    value = iter.remove();
    return iter;
}

// Iterator insert operator: iter << kv_pair (calls iter.insert(kv_pair))
inline MutableObjectIterator& operator<<(MutableObjectIterator& iter, KeyValue&& kv_pair)
{
    // for chained insertion, need next after insert.
    bool result = iter.insert(std::forward<KeyValue>(kv_pair)) && iter.next();
    return iter;
}

#ifndef XYJSON_DISABLE_CHAINED_INPUT
// Iterator insert operator for chained insertion: iter << key or value
// Uses universal reference forwarding and checks return value to decide if to advance
template<typename T>
inline MutableObjectIterator& operator<<(MutableObjectIterator& iter, T&& arg)
{
    // Call insert method which handles key/value logic internally
    // If insert returns true, it means a value was inserted (advance)
    // If insert returns false, it means a key was stored (don't advance)
    if (yyjson_likely(iter.insert(std::forward<T>(arg)))) {
        iter.next();
    }
    return iter;
}
#endif

// Iterator remove operator: iter >> kv_pair (calls iter.remove() and stores removed key-value pair)
inline MutableObjectIterator& operator>>(MutableObjectIterator& iter, KeyValue& kv_pair)
{
    kv_pair = iter.remove();
    return iter;
}

#endif

// `iter / key`: iterator fast seek
// Only for object iterators, should fail to compile for array iterators
template<typename iteratorT, typename T>
inline typename std::enable_if<
    trait::is_iterator<iteratorT>::value && iteratorT::for_object,
    typename iteratorT::json_type
>::type
operator/(iteratorT& iter, const T& key)
{
    return iter.seek(key);
}

/* @Section 5.7: Document Forward Root Operator */
/* ------------------------------------------------------------------------ */

// `doc / path` : path from root
template <typename docT, typename T>
inline typename std::enable_if<trait::is_document<docT>::value, typename docT::value_type>::type
operator/(const docT& doc, const T& path)
{
    return doc.root() / path;
}

template <typename docT, typename T, typename ifT =
    typename std::enable_if<trait::is_document<docT>::value>::type
>
inline auto operator%(const docT& doc, const T& path)
{
    return doc.root() % path;
}

// `+doc` : convert root to integer
template<typename docT>
inline typename std::enable_if<trait::is_document<docT>::value, int>::type
operator+(docT& doc)
{
    return +doc.root();
}

template<typename docT>
inline typename std::enable_if<trait::is_document<docT>::value, int>::type
operator+(const docT& doc)
{
    return +doc.root();
}

// `-doc` : convert root to string
template<typename docT>
inline typename std::enable_if<trait::is_document<docT>::value, std::string>::type
operator-(docT& doc)
{
    return -doc.root();
}

template<typename docT>
inline typename std::enable_if<trait::is_document<docT>::value, std::string>::type
operator-(const docT& doc)
{
    return -doc.root();
}

// `doc == doc` : compare their root
template<typename docT>
inline typename std::enable_if<trait::is_document<docT>::value, bool>::type
operator==(const docT& lhs, const docT& rhs)
{
    return lhs.root() == rhs.root();
}

// `std::ostream << doc` : output root as JSON string
template<typename docT>
inline typename std::enable_if<trait::is_document<docT>::value, std::ostream&>::type
operator<<(std::ostream& os, const docT& doc)
{
    return os << doc.root();
}

/* @Section 5.8: User-defined Literal Operator */
/* ------------------------------------------------------------------------ */

/**
 * User-defined literal operator for converting string literals to Document
 * Usage: "{\"key\": \"value\"}"_xyjson
 */
inline yyjson::Document operator""_xyjson(const char* jsonStr, std::size_t len)
{
    return yyjson::Document(jsonStr, len);
}

/* @Part 6: Last Definitions */
/* ======================================================================== */

} /* end of namespace yyjson:: */

#endif /* end of include guard: XYJSON_H__ */
