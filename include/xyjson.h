/** 
 * @file xyjson.h 
 * @author lymslive 
 * @date 2025-09-15 
 *
 * @details 
 * C++ wrapper for yyjson library with operator overloading.
 * Provides simple path operator(/), value extraction operator(|), 
 * and extra assignment operator(=), input operator(<<) for mutable json model.
 * */
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
/* Part 1: Front Definitions                                       Line: 78 */
/*   Section 1.1: Forward Class Declarations                       Line: 81 */
/*   Section 1.2: Type and Operator Constants                      Line: 95 */
/*   Section 1.3: Type Traits                                     Line: 138 */
/* Part 2: Class Definitions                                      Line: 240 */
/*   Section 2.1: Read-only Json Model                            Line: 243 */
/*   Section 2.2: Mutable Json Model                              Line: 489 */
/*   Section 2.3: Helper Class for Mutable Json                   Line: 860 */
/*   Section 2.4: Iterator Json                                   Line: 905 */
/* Part 3: Non-Class Functions                                   Line: 1194 */
/*   Section 3.1: Underlying mut_val Creation                    Line: 1200 */
/*   Section 3.2: Conversion Helper Functions                    Line: 1392 */
/* Part 4: Class Implementations                                 Line: 1503 */
/*   Section 4.1: Value Methods                                  Line: 1506 */
/*     Group 4.1.1: get and getor                                Line: 1509 */
/*     Group 4.1.2: size and index/path                          Line: 1605 */
/*     Group 4.1.3: create iterator                              Line: 1638 */
/*     Group 4.1.4: others                                       Line: 1679 */
/*   Section 4.2: Document Methods                               Line: 1729 */
/*     Group 4.2.1: primary manage                               Line: 1732 */
/*     Group 4.2.2: read and write                               Line: 1764 */
/*   Section 4.3: MutableValue Methods                           Line: 1849 */
/*     Group 4.3.1: get and getor                                Line: 1852 */
/*     Group 4.3.2: size and index/path                          Line: 1954 */
/*     Group 4.3.3: assignment set                               Line: 2006 */
/*     Group 4.3.4: array and object add                         Line: 2128 */
/*     Group 4.3.5: smart input and tag                          Line: 2181 */
/*     Group 4.3.6: create iterator                              Line: 2259 */
/*     Group 4.3.7: others                                       Line: 2300 */
/*   Section 4.4: MutableDocument Methods                        Line: 2350 */
/*     Group 4.4.1: primary manage                               Line: 2353 */
/*     Group 4.4.2: read and write                               Line: 2400 */
/*     Group 4.4.3: create mutable value                         Line: 2497 */
/*   Section 4.5: ArrayIterator Methods                          Line: 2520 */
/*   Section 4.6: ObjectIterator Methods                         Line: 2553 */
/*   Section 4.7: MutableArrayIterator Methods                   Line: 2606 */
/*   Section 4.8: MutableObjectIterator Methods                  Line: 2639 */
/* Part 5: Operator Interface                                    Line: 2692 */
/*   Section 5.1: Primary Path Access                            Line: 2695 */
/*   Section 5.2: Conversion Unary Operator                      Line: 2779 */
/*   Section 5.3: Comparison Operator                            Line: 2825 */
/*   Section 5.4: Create and Bind KeyValue                       Line: 2911 */
/*   Section 5.5: Stream and Input Operator                      Line: 2940 */
/*   Section 5.6: Iterator Creation and Operation                Line: 2982 */
/*   Section 5.7: Document Forward Root Operator                 Line: 3095 */
/*   Section 5.8: User-defined Literal Operator                  Line: 3160 */
/* Part 6: Last Definitions                                      Line: 3177 */
/* ======================================================================== */

namespace yyjson
{

// moved to trait namespace below

/* @Part 1: Front Definitions */
/* ======================================================================== */

/* @Section 1.1: Forward Class Declarations */
/* ------------------------------------------------------------------------ */

class Value;
class Document;
class MutableValue;
class MutableDocument;
class ArrayIterator;
class ObjectIterator;
class MutableArrayIterator;
class MutableObjectIterator;
class KeyValue;
class StringRef;

/* @Section 1.2: Type and Operator Constants */
/* ------------------------------------------------------------------------ */

// Special representation type
struct ZeroNumber {};
struct EmptyString {};
struct EmptyArray {};
struct EmptyObject {};

// Type representative constants for type checking
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
constexpr const char* okExtract = "|";
constexpr const char* okPipe = "|";
constexpr const char* okType = "&";
constexpr const char* okPath = "/";
constexpr const char* okAssign = "=";
constexpr const char* okInput = "<<";
constexpr const char* okOutput = ">>";
constexpr const char* okEqual = "==";
constexpr const char* okCreate = "*";
constexpr const char* okRoot = "*";
constexpr const char* okNumberify = "+";
constexpr const char* okStringify = "-";
constexpr const char* okConvert = "~";
constexpr const char* okIterator = "%";
constexpr const char* okIncreace = "++";

/* @Section 1.3: Type Traits */
/* ------------------------------------------------------------------------ */

namespace trait
{
/**
 * @brief Type traits for yyjson wrapper classes
 * 
 * These traits can be used with std::enable_if, std::conditional, etc.
 * for template metaprogramming and SFINAE techniques.
 */

// is_value<T> Specializations for Value and MutableValue
template<typename T> struct is_value : std::false_type {};
template<> struct is_value<Value> : std::true_type {};
template<> struct is_value<MutableValue> : std::true_type {};

// is_document<T> Specializations for Document and MutableDocument
template<typename T> struct is_document : std::false_type {};
template<> struct is_document<Document> : std::true_type {};
template<> struct is_document<MutableDocument> : std::true_type {};

// is_iterator<T> Specializations for all iterator types
template<typename T> struct is_iterator : std::false_type {};
template<> struct is_iterator<ArrayIterator> : std::true_type {};
template<> struct is_iterator<ObjectIterator> : std::true_type {};
template<> struct is_iterator<MutableArrayIterator> : std::true_type {};
template<> struct is_iterator<MutableObjectIterator> : std::true_type {};

// is_key<T> Type trait for supported key string types
template<typename T> struct is_key : std::false_type {};
template<> struct is_key<const char*> : std::true_type {}; // and decay const char[N]
template<> struct is_key<char*> : std::true_type {};       // and decay char[N]
template<> struct is_key<std::string> : std::true_type {};
template<typename T>
constexpr bool is_key_v = is_key<std::decay_t<T>>::value;

// Helper constexpr function to check if a type is a C-style string
// Handles both const char* and char* types
template<typename T>
constexpr bool is_cstr_type() {
    using decayed_t = std::decay_t<T>;
    return std::is_same<decayed_t, const char*>::value || std::is_same<decayed_t, char*>::value;
}

// is_scalar<T> Type trait for supported scalar types
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

// enable_getor<T>: types eligible for getor(default)
template<typename T>
struct enable_getor : std::bool_constant<
    is_scalar<std::decay_t<T>>::value ||
    std::is_same<std::decay_t<T>, yyjson_val*>::value ||
    std::is_same<std::decay_t<T>, yyjson_mut_val*>::value ||
    std::is_same<std::decay_t<T>, yyjson_mut_doc*>::value ||
    std::is_same<std::decay_t<T>, std::nullptr_t>::value ||
    std::is_same<std::decay_t<T>, EmptyString>::value ||
    std::is_same<std::decay_t<T>, ZeroNumber>::value
> {};

template<typename T>
constexpr bool enable_getor_v = enable_getor<T>::value;

// Check if funcT is callable with any supported scalar type
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

// Detect callable type (lambda, std::function, function pointer)
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
 * @brief Simple wrapper for yyjson_val with operator overloading
 * Provides basic read-only operations with operator overloading
 * 
 * Supported operators:
 * - Access: json[index] (index access), json / path (path access)
 * - Extraction: json | defaultValue (value extraction), dest |= json (assignment)
 * - Pipe: json | func (custom transformation)
 * - Type: json & type (check type)
 * - Scalar output: json >> value
 * - Steam output: os << json
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
        if (type != nullptr) {
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
    
    // Pipe function for custom transformations
    template<typename funcT>
    inline auto pipe(funcT&& func) const;

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
    inline typename std::enable_if<trait::is_cstr_type<T>(), Value>::type
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
    inline typename std::enable_if<trait::is_cstr_type<T>(), Value>::type
    pathto(T path) const {
        return pathto(path, path ? ::strlen(path) : 0);
    }
    
    // Iterator creation methods
    ArrayIterator arrayIter(size_t startIndex = 0) const;
    ObjectIterator objectIter(const char* startKey = nullptr) const;
    
    // Standard iterator pattern methods
    ArrayIterator beginArray() const;
    ArrayIterator endArray() const;
    ObjectIterator beginObject() const;
    ObjectIterator endObject() const;

    // Match iterator method
    Value* operator->() { return this; }
    yyjson_val* c_val() const { return m_val; }
    
    // Conversion methods
    std::string toString(bool pretty = false) const;
    int toInteger() const;
    double toNumber() const;
    
    // Comparison method
    bool equal(const Value& other) const;
    bool less(const Value& other) const;

private:
    yyjson_val* m_val = nullptr;
};

/** 
 * @brief Wrapper for yyjson_doc with operator overloading
 */
/**
 * @brief Read-only JSON document wrapper for yyjson_doc
 * Provides read-only access to JSON data with operator overloading
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
        if (this != &other) {
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

/** 
 * @brief Writable wrapper for yyjson_mut_val
 * Provides read-write access to yyjson values with operator overloading
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
 * - Steam output: os << json
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
        if (type != nullptr) {
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
    
    // Pipe function for custom transformations
    template<typename funcT>
    inline auto pipe(funcT&& func) const;

    // Array/Object size and access by index or key
    size_t size() const;

    MutableValue index(size_t idx) const;
    MutableValue index(int idx) const { return index(static_cast<size_t>(idx)); }

    MutableValue index(const char* key, size_t len) const;
    MutableValue index(const std::string& key) const {
        return index(key.c_str(), key.size()); 
    }
    template <size_t N>
    MutableValue index(const char(&key)[N]) const { return index(key, N-1); }
    template<typename T>
    inline typename std::enable_if<trait::is_cstr_type<T>(), MutableValue>::type
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
    inline typename std::enable_if<trait::is_cstr_type<T>(), MutableValue>::type
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
    inline typename std::enable_if<trait::is_cstr_type<T>(), MutableValue>::type
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
    inline typename std::enable_if<trait::is_cstr_type<T>(), MutableValue&>::type
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

    // Smart input method, append for array, add for object, assign for scalar
    template <typename T>
    MutableValue& input(T&& value);

    // Input pending key, only support string type.
    template <typename keyT>
    bool inputKey(keyT&& key);

    // Input value after pending key.
    template <typename T>
    bool inputValue(T&& value);

    // Iterator creation methods
    MutableArrayIterator arrayIter(size_t startIndex = 0) const;
    MutableObjectIterator objectIter(const char* startKey = nullptr) const;
    
    // Standard iterator pattern methods
    MutableArrayIterator beginArray() const;
    MutableArrayIterator endArray() const;
    MutableObjectIterator beginObject() const;
    MutableObjectIterator endObject() const;
    
    // Match iterator method
    MutableValue* operator->() { return this; }
    yyjson_mut_val* c_val() const { return m_val; }
    yyjson_mut_doc* c_doc() const { return m_doc; }

    // Conversion methods
    std::string toString(bool pretty = false) const;
    int toInteger() const;
    double toNumber() const;
    
    // Comparison method
    bool equal(const MutableValue& other) const;
    bool less(const MutableValue& other) const;
    
private:
    yyjson_mut_val* m_val = nullptr;
    yyjson_mut_doc* m_doc = nullptr; // Needed for memory allocation

    // For chained operator << key << value, save the key
    yyjson_mut_val* m_pendingKey = nullptr;
    
    friend class MutableDocument;
};

/**
 * @brief Mutable JSON document wrapper for yyjson_mut_doc
 * Provides read-write access to JSON data with operator overloading
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
        if (this != &other) {
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

    yyjson_mut_doc* get() const { return m_doc; }

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

/* @Section 2.3: Helper Class for Mutable Json */
/* ------------------------------------------------------------------------ */

/// Helper class to optimize create json node from string literal.
struct StringRef
{
    const char* str = nullptr;
    size_t len = 0;

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
 * @brief Key-Value pair for optimized object insertion
 * Simple structure containing key and value for efficient object insertion
 */
struct KeyValue
{
    yyjson_mut_val* key = nullptr;   // The key for object insertion
    yyjson_mut_val* value = nullptr;  // The value for object insertion
    
    KeyValue() : key(nullptr), value(nullptr) {}
    KeyValue(yyjson_mut_val* k, yyjson_mut_val* v) 
        : key(k), value(v) {}
    
    // Check if KeyValue is valid
    bool isValid() const { return key && value; }
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }
};

/* @Section 2.4: Iterator Json */
/* ------------------------------------------------------------------------ */

/**
 * @brief Iterator for JSON array elements
 * Provides efficient iteration over array elements
 * 
 * Supported operators:
 * - Dereference: *iter, iter-> (access current item)
 * - Increment: ++iter, iter++ (calls next())
 * - Advance: iter + n, iter += n (calls advance(n))
 * - Position: iter % n , iter %= n (calls advance(n))
 * - Unary: +iter (get current index), -iter (no-op), ~iter (no-op)
 * - Comparison: ==, != (calls equal())
 * - Boolean: if (iter), !iter
 */
class ArrayIterator
{
public:
    static constexpr bool for_object = false;
    static constexpr bool for_mutable = false;
    using json_type = Value;

    ArrayIterator() : m_arr(nullptr), m_iter({0}) {}
    explicit ArrayIterator(yyjson_val* root);
    
    // Check if iterator is valid (current element exists)
    bool isValid() const { return m_iter.idx < m_iter.max; }
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }
    
    // Set to end state
    ArrayIterator& over() { m_iter.idx = m_iter.max; return *this; }

    // Check if two iterators are equal
    bool equal(const ArrayIterator& other) const {
        return m_arr == other.m_arr && m_iter.idx == other.m_iter.idx;
    }
    
    // Get underlying C API iterator and value structure pointer
    yyjson_arr_iter* c_iter() { return &m_iter; }
    const yyjson_arr_iter* c_iter() const { return &m_iter; }
    yyjson_val* c_val() const { return m_iter.cur; }

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
    ArrayIterator& advance(size_t steps = 1); // +n, move n steps
    ArrayIterator& advance(const char* key) { return over(); } // No-op for array iterator
    ArrayIterator& rewind(); // Reset iterator to beginning

private:
    /// Native yyjson array iterator (mutable for const methods)
    mutable yyjson_arr_iter m_iter;
    /// Current array to iterate (not include in m_iter)
    yyjson_val* m_arr = nullptr;
};

/**
 * @brief Iterator for JSON object key-value pairs
 * Provides efficient iteration over object properties
 * 
 * Supported operators:
 * - Dereference: *iter, iter-> (access value node)
 * - Increment: ++iter, iter++ (calls next())
 * - Advance: iter + n, iter += n (calls advance(n))
 * - Position: iter % key , iter %= key (calls advance(key))
 * - Fast Seek: / (sequential static seek)
 * - Unary: +iter(current index), -iter(current key name), ~iter(current key node)
 * - Comparison: ==, != (calls equal())
 * - Boolean: if (iter), !iter
 */
class ObjectIterator
{
public:
    static constexpr bool for_object = true;
    static constexpr bool for_mutable = false;
    using json_type = Value;
    
    ObjectIterator() : m_iter({0}) {}
    explicit ObjectIterator(yyjson_val* root);
    
    // Check if iterator is valid
    bool isValid() const { return m_iter.idx < m_iter.max; }
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }
    
    // Set to end state
    ObjectIterator& over() { m_iter.idx = m_iter.max; return *this; }

    // Check if two iterators are equal
    bool equal(const ObjectIterator& other) const {
        return m_iter.obj == other.m_iter.obj && m_iter.idx == other.m_iter.idx;
    }

    // Get underlying C API iterator and key/val structure pointer
    yyjson_obj_iter* c_iter() { return &m_iter; }
    const yyjson_obj_iter* c_iter() const { return &m_iter; }
    yyjson_val* c_key() const { return m_iter.cur; }
    yyjson_val* c_val() const { return m_iter.cur ? (m_iter.cur + 1) : nullptr; }

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
    ObjectIterator& advance(size_t steps = 1); // +n
    ObjectIterator& advance(const char* key); // Jump to specific key
    ObjectIterator& rewind(); // Reset iterator to beginning

    // Seek to specific key using fast yyjson API and return Value
    Value seek(const char* key, size_t key_len);
    Value seek(const std::string& key) { return seek(key.c_str(), key.size()); }
    template<size_t N>
    Value seek(const char(&key)[N]) { return seek(key, N-1); }
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), Value>::type
    seek(T key) { return seek(key, key ? ::strlen(key) : 0); }
    
private:
    /// Native yyjson object iterator (mutable for const methods)
    mutable yyjson_obj_iter m_iter;
};

/** 
 * @brief Mutable iterator for JSON array elements
 * Provides efficient iteration over mutable array elements with write access
 * 
 * Supported operators:
 * - Dereference: *iter, iter-> (access current mutable item)
 * - Increment: ++iter, iter++ (calls next())
 * - Advance: iter + n, iter += n (calls advance(n))
 * - Position: iter % n , iter %= n (calls advance(n))
 * - Unary: +iter (get current index), -iter (no-op), ~iter (no-op)
 * - Comparison: ==, != (calls equal())
 * - Boolean: if (iter), !iter
 */
class MutableArrayIterator
{
public:
    static constexpr bool for_object = false;
    static constexpr bool for_mutable = true;
    using json_type = MutableValue;
    
    MutableArrayIterator() : m_doc(nullptr), m_iter({0}) {}
    explicit MutableArrayIterator(yyjson_mut_val* root, yyjson_mut_doc* doc);
    
    // Check if iterator is valid (current element exists)
    bool isValid() const { return m_iter.idx < m_iter.max; }
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }
    
    // Set to end state
    MutableArrayIterator& over() { m_iter.idx = m_iter.max; return *this; }

    // Check if two iterators are equal
    bool equal(const MutableArrayIterator& other) const {
        return m_iter.arr == other.m_iter.arr && m_iter.idx == other.m_iter.idx;
    }

    // Get underlying C API iterator and value structure pointer
    yyjson_mut_arr_iter* c_iter() { return &m_iter; }
    const yyjson_mut_arr_iter* c_iter() const { return &m_iter; }
    yyjson_mut_val* c_val() const { return m_iter.cur ? m_iter.cur->next : nullptr; }

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
    MutableArrayIterator& advance(size_t steps = 1); // +n
    MutableArrayIterator& advance(const char* key) { return over(); } // No-op for array iterator
    MutableArrayIterator& rewind(); // Reset iterator to beginning

    // Insert methods
    bool insert(yyjson_mut_val* value); // Insert raw pointer, return success status
    template<typename T>
    bool insert(T&& value); // Insert various types, return success status

private:
    /// Native yyjson mutable array iterator (mutable for const methods)
    mutable yyjson_mut_arr_iter m_iter;
    /// Document for context (needed for mutation)
    yyjson_mut_doc* m_doc = nullptr;
};

/** 
 * @brief Mutable iterator for JSON object key-value pairs
 * Provides efficient iteration over mutable object properties with write access
 * 
 * Supported operators:
 * - Dereference: *iter, iter-> (access current mutable key-value pair)
 * - Increment: ++iter, iter++ (calls next())
 * - Advance: iter + n, iter += n (calls advance(n))
 * - Position: iter % key , iter %= key (calls advance(key))
 * - Fast Seek: / (sequential static seek)
 * - Unary: +iter(current index), -iter(current key name), ~iter(current key node)
 * - Comparison: ==, != (calls equal())
 * - Boolean: if (iter), !iter
 */
class MutableObjectIterator
{
public:
    static constexpr bool for_object = true;
    static constexpr bool for_mutable = true;
    using json_type = MutableValue;
    
    MutableObjectIterator() : m_doc(nullptr), m_iter({0}) {}
    explicit MutableObjectIterator(yyjson_mut_val* root, yyjson_mut_doc* doc);
    
    // Check if iterator is valid
    bool isValid() const { return m_iter.idx < m_iter.max; }
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }
    
    // Set to end state
    MutableObjectIterator& over() { m_iter.idx = m_iter.max; return *this; }

    // Check if two iterators are equal
    bool equal(const MutableObjectIterator& other) const {
        return m_iter.obj == other.m_iter.obj && m_iter.idx == other.m_iter.idx;
    }

    // Get underlying C API iterator and key/val structure pointer
    yyjson_mut_obj_iter* c_iter() { return &m_iter; }
    const yyjson_mut_obj_iter* c_iter() const { return &m_iter; }
    yyjson_mut_val* c_key() const {
        return m_iter.cur && m_iter.cur->next ? m_iter.cur->next->next : nullptr;
    }
    yyjson_mut_val* c_val() const { return yyjson_mut_obj_iter_get_val(c_key()); }

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
    MutableObjectIterator& advance(size_t steps = 1); // +n to next n pairs
    MutableObjectIterator& advance(const char* key); // jump to specific key
    MutableObjectIterator& rewind(); // Reset iterator to beginning

    // Seek to specific key using fast yyjson API and return MutableValue
    MutableValue seek(const char* key, size_t key_len);
    MutableValue seek(const std::string& key) { return seek(key.c_str(), key.size()); }
    template<size_t N>
    MutableValue seek(const char(&key)[N]) { return seek(key, N-1); }
    template<typename T>
    typename std::enable_if<trait::is_cstr_type<T>(), MutableValue>::type
    seek(T key) { return seek(key, key ? ::strlen(key) : 0); }
    
private:
    /// Native yyjson mutable object iterator (mutable for const methods)
    mutable yyjson_mut_obj_iter m_iter;
    /// Document for context (needed for mutation)
    yyjson_mut_doc* m_doc = nullptr;
};

/* @Part 3: Non-Class Functions */
/* ======================================================================== */

namespace util
{

/* @Section 3.1: Underlying mut_val Creation */
/* ------------------------------------------------------------------------ */

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

/* @Section 3.2: Conversion Helper Functions */
/* ------------------------------------------------------------------------ */

// Helper function to get yyjson_val* from both Value and MutableValue

/// Compare two JSON values (Value or MutableValue) using hybrid logic.
template<typename T>
inline typename std::enable_if<trait::is_value<T>::value, bool>::type
lessCompare(const T& lhs, const T& rhs)
{
    if (!lhs.isValid()) {
        return rhs.isValid(); // an invalid value is less than a valid one
    }
    if (!rhs.isValid()) {
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

/// Convert JSON(Value or MutableValue) values to integers
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, int>::type
toIntegerCast(const jsonT& val)
{
    if (!val.isValid()) return 0;
    
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

// Apply function to json value with smart scalar mapping
// Prefer callable with json type, else map to supported scalars,
// finally fallback to getor(default) when func itself is a default value.
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
    if (isBool()) {
        result = yyjson_get_bool(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(int& result) const
{
    if (isInt()) {
        result = yyjson_get_int(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(int64_t& result) const
{
    if (isSint()) {
        result = yyjson_get_sint(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(uint64_t& result) const
{
    if (isUint()) {
        result = yyjson_get_uint(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(double& result) const
{
    if (isReal()) {
        result = yyjson_get_real(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(const char*& result) const
{
    if (isString()) {
        result = yyjson_get_str(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(std::string& result) const
{
    if (isString()) {
        result = yyjson_get_str(m_val);
        return true;
    }
    return false;
}

inline bool Value::get(yyjson_val*& result) const
{
    if (isValid()) { result = m_val; return true; }
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
    return isString() ? yyjson_get_str(m_val) : "";
}

inline double Value::getor(ZeroNumber) const
{
    return toNumber();
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
    if (!m_val) return 0;
    if (yyjson_is_arr(m_val)) return yyjson_arr_size(m_val);
    if (yyjson_is_obj(m_val)) return yyjson_obj_size(m_val);
    return 0;
}

inline Value Value::index(size_t idx) const
{
    if (!isArray()) return Value(nullptr);
    return Value(yyjson_arr_get(m_val, idx));
}

inline Value Value::index(const char* key, size_t len) const
{
    if (!isObject() || !key) return Value(nullptr);
    return Value(yyjson_obj_getn(m_val, key, len));
}

inline Value Value::pathto(const char* path, size_t len) const
{
    if (!path || len == 0) return *this;
    if (path[0] == '/') {
        yyjson_val* result = yyjson_ptr_getn(m_val, path, len);
        return Value(result);
    }
    return index(path, len);
}

/* @Group 4.1.3: create iterator */
/* ************************************************************************ */

inline ArrayIterator Value::arrayIter(size_t startIndex) const
{
    if (isArray()) {
        ArrayIterator iter(m_val);
        return iter.advance(startIndex);
    }
    return ArrayIterator();
}

inline ObjectIterator Value::objectIter(const char* startKey) const
{
    if (isObject()) {
        ObjectIterator iter(m_val);
        return iter.advance(startKey);
    }
    return ObjectIterator();
}

inline ArrayIterator Value::beginArray() const
{
    return arrayIter(0);
}

inline ArrayIterator Value::endArray() const
{
    return beginArray().over();
}

inline ObjectIterator Value::beginObject() const
{
    return objectIter(nullptr);
}

inline ObjectIterator Value::endObject() const
{
    return beginObject().over();
}

/* @Group 4.1.4: others */
/* ************************************************************************ */

inline std::string Value::toString(bool pretty) const
{
    if (!isValid()) return "";
    
    if (isString() && !pretty) {
        return std::string(yyjson_get_str(m_val));
    }
    
    uint32_t flags = YYJSON_WRITE_NOFLAG;
    if (pretty) {
        flags |= YYJSON_WRITE_PRETTY;
    }
    
    size_t len = 0;
    char* json_str = yyjson_val_write(m_val, flags, &len);
    if (!json_str) return "";
    
    std::string result(json_str, len);
    free(json_str);
    return result;
}

inline int Value::toInteger() const
{
    if (!isValid()) return 0;
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

inline void Document::free()
{
    if (m_doc)
    {
        yyjson_doc_free(m_doc);
        m_doc = nullptr;
    }
}

inline MutableDocument Document::mutate() const
{
    if (!isValid()) {
        return MutableDocument((yyjson_mut_doc*)nullptr);
    }
    
    yyjson_mut_doc* mut_doc = yyjson_doc_mut_copy(m_doc, nullptr);
    return MutableDocument(mut_doc);
}

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
    
    if (!fp) return false;
    m_doc = yyjson_read_fp(fp, 0, nullptr, nullptr);
    return isValid();
}

inline bool Document::read(std::ifstream& ifs)
{
    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    
    if (size == 0) return false;
    
    std::string content(size, '\0');
    ifs.read(&content[0], size);
    
    return read(content);
}

inline bool Document::readFile(const char* path)
{
    if (!path) return false;
    FILE* fp = fopen(path, "rb");
    if (!fp) return false;

    bool result = read(fp);
    fclose(fp);
    return result;
}

inline bool Document::write(std::string& output) const
{
    if (!m_doc) return false;
    char* json = yyjson_write(m_doc, 0, nullptr);
    if (!json) return false;
    output = json;
    std::free(json);
    return true;
}

inline bool Document::write(FILE* fp) const
{
    if (!m_doc || !fp) return false;
    return yyjson_write_fp(fp, m_doc, 0, nullptr, nullptr);
}

inline bool Document::write(std::ofstream& ofs) const
{
    std::string content;
    if (!write(content)) return false;
    
    ofs.seekp(0, std::ios::beg);
    ofs.write(content.c_str(), content.size());
    ofs.flush();
    
    return !ofs.fail();
}

inline bool Document::writeFile(const char* path) const
{
    if (!path) return false;
    FILE* fp = fopen(path, "wb");
    if (!fp) return false;

    bool result = write(fp);
    fclose(fp);
    return result;
}

/* @Section 4.3: MutableValue Methods */
/* ------------------------------------------------------------------------ */

/* @Group 4.3.1: get and getor */
/* ************************************************************************ */

inline bool MutableValue::get(bool& result) const
{
    if (isBool()) {
        result = yyjson_mut_get_bool(m_val);
        return true;
    }
    return false;
}

inline bool MutableValue::get(int& result) const
{
    if (isInt()) {
        result = yyjson_mut_get_int(m_val);
        return true;
    }
    return false;
}

inline bool MutableValue::get(int64_t& result) const
{
    if (isSint()) {
        result = yyjson_mut_get_sint(m_val);
        return true;
    }
    return false;
}

inline bool MutableValue::get(uint64_t& result) const
{
    if (isUint()) {
        result = yyjson_mut_get_uint(m_val);
        return true;
    }
    return false;
}

inline bool MutableValue::get(double& result) const
{
    if (isReal()) {
        result = yyjson_mut_get_real(m_val);
        return true;
    }
    return false;
}

inline bool MutableValue::get(const char*& result) const
{
    if (isString()) {
        result = yyjson_mut_get_str(m_val);
        return true;
    }
    return false;
}

inline bool MutableValue::get(std::string& result) const
{
    if (isString()) {
        result = yyjson_mut_get_str(m_val);
        return true;
    }
    return false;
}

inline bool MutableValue::get(yyjson_mut_val*& result) const
{
    if (isValid()) { result = m_val; return true; }
    return false;
}

inline bool MutableValue::get(yyjson_mut_doc*& result) const
{
    if (m_doc != nullptr) { result = m_doc; return true; }
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
    return isString() ? yyjson_mut_get_str(m_val) : "";
}

inline double MutableValue::getor(ZeroNumber) const
{
    return toNumber();
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
    if (!m_val) return 0;
    if (yyjson_mut_is_arr(m_val)) return yyjson_mut_arr_size(m_val);
    if (yyjson_mut_is_obj(m_val)) return yyjson_mut_obj_size(m_val);
    return 0;
}

inline MutableValue MutableValue::index(size_t idx) const
{
    if (!isArray()) return MutableValue(nullptr, m_doc);
    return MutableValue(yyjson_mut_arr_get(m_val, idx), m_doc);
}

inline MutableValue MutableValue::index(const char* key, size_t len) const
{
    if (!isObject() || !key) return MutableValue(nullptr, m_doc);
    return MutableValue(yyjson_mut_obj_getn(m_val, key, len), m_doc);
}

inline MutableValue MutableValue::index(size_t idx)
{
    if (!isArray()) return MutableValue(nullptr, m_doc);
    return MutableValue(yyjson_mut_arr_get(m_val, idx), m_doc);
}

inline MutableValue MutableValue::index(const char* key, size_t len)
{
    if (!isObject() || !key) return MutableValue(nullptr, m_doc);
    yyjson_mut_val* val = yyjson_mut_obj_getn(m_val, key, len);
    if (!val) {
        // Key doesn't exist, insert null value with explicit length
        val = yyjson_mut_null(m_doc);
        yyjson_mut_val* key_val = yyjson_mut_strncpy(m_doc, key, len);
        yyjson_mut_obj_add(m_val, key_val, val);
    }
    return MutableValue(val, m_doc);
}

inline MutableValue MutableValue::pathto(const char* path, size_t len) const
{
    if (!path || len == 0) return *this;
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
    if (isValid())
    {
        yyjson_mut_set_arr(m_val);
    }
    return *this;
}

inline MutableValue& MutableValue::setObject()
{
    if (isValid())
    {
        yyjson_mut_set_obj(m_val);
    }
    return *this;
}

/* @Group 4.3.4: array and object add */
/* ************************************************************************ */

inline MutableValue& MutableValue::append(yyjson_mut_val* value)
{
    if (isArray() && value)
    {
        yyjson_mut_arr_append(m_val, value);
    }
    return *this;
}

template <typename T>
inline MutableValue& MutableValue::append(T&& value)
{
    return append(util::create(m_doc, std::forward<T>(value)));
}

inline MutableValue& MutableValue::add(yyjson_mut_val* key, yyjson_mut_val* value)
{
    if (isObject() && key && value)
    {
        yyjson_mut_obj_add(m_val, key, value);
    }
    return *this;
}

inline MutableValue& MutableValue::add(KeyValue&& kv)
{
    if (isObject() && kv.isValid())
    {
        add(kv.key, kv.value);
        kv.key = nullptr;
        kv.value = nullptr;
    }
    return *this;
}

template<typename keyT, typename valT>
inline MutableValue& MutableValue::add(keyT&& key, valT&& value)
{
    if (isObject()) {
        yyjson_mut_val* keyNode = util::createKey(m_doc, std::forward<keyT>(key));
        if (!keyNode) {
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
    if (key.m_doc == m_doc && key.isString())
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
inline MutableValue& MutableValue::input(KeyValue&& kv)
{
    if (isObject())
    {
        return add(std::forward<KeyValue>(kv));
    }
    return *this;
}

template <typename keyT>
inline bool MutableValue::inputKey(keyT&& key)
{
    yyjson_mut_val* keyNode = util::create(m_doc, std::forward<keyT>(key));
    if (!yyjson_mut_is_str(keyNode)) {
        keyNode = nullptr;
        return false;
    }
    
    m_pendingKey = keyNode;
    return true;
}

template <typename T>
inline bool MutableValue::inputValue(T&& value)
{
    if (m_pendingKey == nullptr)
    {
        return false;
    }
    yyjson_mut_val* succeedVal = util::create(m_doc, std::forward<T>(value));
    add(m_pendingKey, succeedVal);
    m_pendingKey = nullptr;
    return true;
}

// General template for other types
template <typename T>
inline MutableValue& MutableValue::input(T&& value)
{
    if (isArray())
    {
        return append(std::forward<T>(value));
    }
    else if (isObject())
    {
        bool ok = inputValue(std::forward<T>(value)) || inputKey(std::forward<T>(value));
    }
    return *this;
}

/* @Group 4.3.6: create iterator */
/* ************************************************************************ */

inline MutableArrayIterator MutableValue::arrayIter(size_t startIndex) const
{
    if (isArray()) {
        MutableArrayIterator iter(m_val, m_doc);
        return iter.advance(startIndex);
    }
    return MutableArrayIterator();
}

inline MutableObjectIterator MutableValue::objectIter(const char* startKey) const
{
    if (isObject()) {
        MutableObjectIterator iter(m_val, m_doc);
        return iter.advance(startKey);
    }
    return MutableObjectIterator();
}

inline MutableArrayIterator MutableValue::beginArray() const
{
    return arrayIter(0);
}

inline MutableArrayIterator MutableValue::endArray() const
{
    return beginArray().over();
}

inline MutableObjectIterator MutableValue::beginObject() const
{
    return objectIter(nullptr);
}

inline MutableObjectIterator MutableValue::endObject() const
{
    return beginObject().over();
}

/* @Group 4.3.7: others */
/* ************************************************************************ */

inline std::string MutableValue::toString(bool pretty) const
{
    if (!isValid()) return "";

    if (isString() && !pretty) {
        return std::string(yyjson_mut_get_str(m_val));
    }

    uint32_t flags = YYJSON_WRITE_NOFLAG;
    if (pretty) {
        flags |= YYJSON_WRITE_PRETTY;
    }

    size_t len = 0;
    char* json_str = yyjson_mut_val_write(m_val, flags, &len);
    if (!json_str) return "";

    std::string result(json_str, len);
    free(json_str);
    return result;
}

inline int MutableValue::toInteger() const
{
    if (!isValid()) return 0;
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
    if (doc) {
        m_doc = yyjson_doc_mut_copy(doc, nullptr);
        yyjson_doc_free(doc);
    } else {
        m_doc = nullptr;
    }
}

inline void MutableDocument::free()
{
    if (m_doc)
    {
        yyjson_mut_doc_free(m_doc);
        m_doc = nullptr;
    }
}

inline Document MutableDocument::freeze() const
{
    if (!isValid()) {
        return Document((yyjson_doc*)nullptr);
    }
    
    // Convert mutable document to read-only document
    yyjson_doc* doc = yyjson_mut_doc_imut_copy(m_doc, nullptr);
    return Document(doc);
}

inline void MutableDocument::setRoot(MutableValue val)
{
    if (m_doc)
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
    
    if (!fp) return false;
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
    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    
    if (size == 0) return false;
    
    std::string content(size, '\0');
    ifs.read(&content[0], size);
    
    return read(content);
}

inline bool MutableDocument::readFile(const char* path)
{
    if (!path) return false;
    FILE* fp = fopen(path, "rb");
    if (!fp) return false;

    bool result = read(fp);
    fclose(fp);
    return result;
}

inline bool MutableDocument::write(std::string& output) const
{
    if (!m_doc) return false;
    char* json = yyjson_mut_write(m_doc, 0, nullptr);
    if (!json) return false;
    output = json;
    std::free(json);
    return true;
}

inline bool MutableDocument::write(FILE* fp) const
{
    if (!m_doc || !fp) return false;
    return yyjson_mut_write_fp(fp, m_doc, 0, nullptr, nullptr);
}

inline bool MutableDocument::write(std::ofstream& ofs) const
{
    std::string content;
    if (!write(content)) return false;
    
    ofs.seekp(0, std::ios::beg);
    ofs.write(content.c_str(), content.size());
    ofs.flush();
    
    return !ofs.fail();
}

inline bool MutableDocument::writeFile(const char* path) const
{
    if (!path) return false;
    FILE* fp = fopen(path, "wb");
    if (!fp) return false;
    
    bool result = write(fp);
    fclose(fp);
    return result;
}

/* @Group 4.4.3: create mutable value */
/* ************************************************************************ */

inline MutableValue MutableDocument::create(yyjson_mut_val* value) const
{ 
    if (value && m_doc)
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

/* @Section 4.5: ArrayIterator Methods */
/* ------------------------------------------------------------------------ */

inline ArrayIterator::ArrayIterator(yyjson_val* root) : m_arr(root)
{
    if (root) {
        yyjson_arr_iter_init(root, &m_iter);
    }
}

inline ArrayIterator& ArrayIterator::next()
{
    yyjson_arr_iter_next(const_cast<yyjson_arr_iter*>(&m_iter));
    return *this;
}

inline ArrayIterator& ArrayIterator::rewind()
{
    if (m_arr) {
        yyjson_arr_iter_init(m_arr, const_cast<yyjson_arr_iter*>(&m_iter));
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
    if (root) {
        yyjson_obj_iter_init(root, &m_iter);
    }
}

inline ObjectIterator& ObjectIterator::next()
{
    yyjson_obj_iter_next(const_cast<yyjson_obj_iter*>(&m_iter));
    return *this;
}

inline ObjectIterator& ObjectIterator::rewind()
{
    if (m_iter.obj) {
        yyjson_obj_iter_init(m_iter.obj, const_cast<yyjson_obj_iter*>(&m_iter));
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
    if (!key || !*key) return *this;
    
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

/* @Section 4.7: MutableArrayIterator Methods */
/* ------------------------------------------------------------------------ */

inline MutableArrayIterator::MutableArrayIterator(yyjson_mut_val* root, yyjson_mut_doc* doc) : m_doc(doc)
{
    if (root) {
        yyjson_mut_arr_iter_init(root, &m_iter);
    }
}

inline MutableArrayIterator& MutableArrayIterator::next()
{
    yyjson_mut_arr_iter_next(const_cast<yyjson_mut_arr_iter*>(&m_iter));
    return *this;
}

inline MutableArrayIterator& MutableArrayIterator::rewind()
{
    if (m_iter.arr) {
        yyjson_mut_arr_iter_init(m_iter.arr, const_cast<yyjson_mut_arr_iter*>(&m_iter));
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
    // alow idx == max to insert at the end.
    if (!val || !m_iter.arr || m_iter.idx > m_iter.max) {
        return false;
    }
#if 0    
    // Slow version: two linear scan
    // Save current index before insertion
    size_t current_idx = m_iter.idx;
    if (yyjson_mut_arr_insert(m_iter.arr, val, current_idx)) {
        rewind();
        // keep interator point to newly inserted element
        advance(current_idx);
        return true;
    }
    return false;
#else
    // refer to: yyjson_mut_arr_insert Implementation

    if (m_iter.max == 0) {
        // first element in circle link
        val->next = val;
        m_iter.cur = val;
        m_iter.arr->uni.ptr = val;
    }
    else {
        // C++ iterator `c_key` is `m_iter.cur->next`, so insert new val as:
        // m_iter.cur->next -> (new val) -> origianl next
        // m_iter.cur not changed
        yyjson_mut_val *prev = m_iter.cur;
        yyjson_mut_val *next = m_iter.cur->next;
        prev->next = val;
        val->next = next;

        // insert to the end, update arr point to end
        if (m_iter.idx == m_iter.max) {
            m_iter.arr->uni.ptr = val;
        }
    }
    unsafe_yyjson_set_len(m_iter.arr, ++m_iter.max);
    return true;
#endif
}

template<typename T>
inline bool MutableArrayIterator::insert(T&& value)
{
    // Create value from template type and forward to native insert
    yyjson_mut_val* val = util::create(m_doc, std::forward<T>(value));
    return insert(val);
}


/* @Section 4.8: MutableObjectIterator Methods */
/* ------------------------------------------------------------------------ */

inline MutableObjectIterator::MutableObjectIterator(yyjson_mut_val* root, yyjson_mut_doc* doc) : m_doc(doc)
{
    if (root) {
        yyjson_mut_obj_iter_init(root, &m_iter);
    }
}

inline MutableObjectIterator& MutableObjectIterator::next()
{
    yyjson_mut_obj_iter_next(const_cast<yyjson_mut_obj_iter*>(&m_iter));
    return *this;
}

inline MutableObjectIterator& MutableObjectIterator::rewind()
{
    if (m_iter.obj) {
        yyjson_mut_obj_iter_init(m_iter.obj, const_cast<yyjson_mut_obj_iter*>(&m_iter));
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
    if (!key || !*key) return *this;
    
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
    yyjson_mut_val* val = yyjson_mut_obj_iter_getn(&m_iter, key, key_len);
    return MutableValue(val, m_doc);
}

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
inline MutableDocument operator~(const Document& doc)
{
    return doc.mutate();
}

// `~doc` --> `doc.freeze()` : MutableDocument --> Document
inline Document operator~(const MutableDocument& doc)
{
    return doc.freeze();
}

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

// MutableValue array/object input operators
// `json << value` --> `json.input(value)`
template <typename T>
inline MutableValue& operator<<(MutableValue& json, T&& value)
{
    return json.input(std::forward<T>(value));
}

// Rvalue version for MutableValue, delegates to lvalue version
template <typename T>
inline MutableValue& operator<<(MutableValue&& json, T&& value)
{
    return json.input(std::forward<T>(value));
}

/* @Section 5.6: Iterator Creation and Operation */
/* ------------------------------------------------------------------------ */

// Array iterator creation (json % int)
// `json % index` --> `json.arrayIter(index)`
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, typename jsonT::array_iterator>::type
operator%(const jsonT& json, int offset)
{
    return json.arrayIter(offset);
}

// Object iterator creation (json % const char*)
// `json % key` --> `json.objectIter(key)`
template<typename jsonT>
inline typename std::enable_if<trait::is_value<jsonT>::value, typename jsonT::object_iterator>::type
operator%(const jsonT& json, const char* key)
{
    return json.objectIter(key);
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

// Iterator seek operators (%= %)
// `iter %= target` --> `iter.rewind().advance(target)` for position jump
template<typename iteratorT, typename T>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT&>::type
operator%=(iteratorT& iter, const T& target)
{
    return iter.rewind().advance(target);
}

// `iter % target` --> `copy iter.rewind().advance(target)` for position jump
template<typename iteratorT, typename T>
inline typename std::enable_if<trait::is_iterator<iteratorT>::value, iteratorT>::type
operator%(iteratorT& iter, const T& target)
{
    iteratorT copy = iter;
    return copy.rewind().advance(target);
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

// Iterator insert operator: iter << value (calls iter.insert(value))
// For MutableArrayIterator only
template<typename T>
inline MutableArrayIterator& operator<<(MutableArrayIterator& iter, T&& value)
{
    iter.insert(std::forward<T>(value));
    iter.next(); // for chained insertion.
    return iter;
}

// Iterator fast seek operator: iter / key (calls iter.seek(key))
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

// `doc % path` : create iterator for root
//template <typename docT, typename T>
//inline typename std::enable_if<trait::is_document<docT>::value, typename docT::value_type>::type
//operator%(const docT& doc, const T& path)
//{
//    return doc.root() % path;
//}

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

namespace literals
{

/**
 * User-defined literal operator for converting string literals to Document
 * Usage: "{\"key\": \"value\"}"_xyjson
 */
inline yyjson::Document operator""_xyjson(const char* jsonStr, std::size_t len)
{
    return yyjson::Document(jsonStr, len);
}

} /* end of namespace yyjson::literals */

/* @Part 6: Last Definitions */
/* ======================================================================== */

} /* end of namespace yyjson:: */

#endif /* end of include guard: XYJSON_H__ */
