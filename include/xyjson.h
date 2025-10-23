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
/* Part 1: Front Definitions                                       Line: 76 */
/* Part 2: Class Definitions                                      Line: 130 */
/*   Section 2.1: Read-only Json Model                            Line: 133 */
/*   Section 2.2: Mutable Json Model                              Line: 361 */
/*   Section 2.3: Helper Class for Mutable Json                   Line: 704 */
/*   Section 2.4: Iterator Json                                   Line: 752 */
/*   Section 2.5: Type Traits                                    Line: 1013 */
/* Part 3: Non-Class Functions                                   Line: 1062 */
/*   Section 3.1: Underlying mut_val Creation                    Line: 1065 */
/*   Section 3.2: Conversion Helper Functions                    Line: 1224 */
/* Part 4: Class Implementations                                 Line: 1303 */
/*   Section 4.1: Value Methods                                  Line: 1306 */
/*     Group 4.1.1: get and getor                                Line: 1309 */
/*     Group 4.1.2: size and index/path                          Line: 1398 */
/*     Group 4.1.3: create iterator                              Line: 1445 */
/*     Group 4.1.4: others                                       Line: 1492 */
/*   Section 4.2: Document Methods                               Line: 1542 */
/*     Group 4.2.1: primary manage                               Line: 1545 */
/*     Group 4.2.2: read and write                               Line: 1577 */
/*   Section 4.3: MutableValue Methods                           Line: 1662 */
/*     Group 4.3.1: get and getor                                Line: 1665 */
/*     Group 4.3.2: size and index/path                          Line: 1760 */
/*     Group 4.3.3: assignment set                               Line: 1836 */
/*     Group 4.3.4: array append                                 Line: 1991 */
/*     Group 4.3.5: object add                                   Line: 2009 */
/*     Group 4.3.6: tag create KeyValue                          Line: 2041 */
/*     Group 4.3.7: smart input                                  Line: 2061 */
/*     Group 4.3.8: create iterator                              Line: 2124 */
/*     Group 4.3.9: others                                       Line: 2171 */
/*   Section 4.4: MutableDocument Methods                        Line: 2221 */
/*     Group 4.4.1: primary manage                               Line: 2224 */
/*     Group 4.4.2: read and write                               Line: 2271 */
/*     Group 4.4.3: create mutable value                         Line: 2368 */
/*   Section 4.5: ArrayIterator Methods                          Line: 2406 */
/*   Section 4.6: ObjectIterator Methods                         Line: 2453 */
/*   Section 4.7: MutableArrayIterator Methods                   Line: 2524 */
/*   Section 4.8: MutableObjectIterator Methods                  Line: 2577 */
/* Part 5: Operator Interface                                    Line: 2651 */
/*   Section 5.1: Primary Path Access                            Line: 2654 */
/*   Section 5.2: Conversion Unary Operator                      Line: 2738 */
/*   Section 5.3: Comparison Operator                            Line: 2784 */
/*   Section 5.4: Create and Bind KeyValue                       Line: 2870 */
/*   Section 5.5: Stream and Input Operator                      Line: 2911 */
/*   Section 5.6: Iterator Creation and Operation                Line: 2953 */
/*   Section 5.7: Document Forward Root Operator                 Line: 3028 */
/*   Section 5.8: User-defined Literal Operator                  Line: 3093 */
/* Part 6: Last Definitions                                      Line: 3110 */
/* ======================================================================== */

namespace yyjson
{

/* @Part 1: Front Definitions */
/* ======================================================================== */

// Forward declarations for classes
class Value;
class Document;
class MutableValue;
class MutableDocument;
class KeyValue;
class StringRef;
class ArrayIterator;
class ObjectIterator;
class MutableArrayIterator;
class MutableObjectIterator;

struct EmptyObject {};
struct EmptyArray {};
struct EmptyString {};
struct ZeroNumber {};

// Type representative constants for type checking
constexpr int kInt = 0;
constexpr int64_t kSint = 0L;
constexpr uint64_t kUint = 0uL;
constexpr EmptyString kString;
constexpr ZeroNumber kNumber;
constexpr double kReal = 0.0;
constexpr bool kBool = false;
constexpr std::nullptr_t kNull = nullptr;
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
    bool isNull() const { return isValid() && yyjson_is_null(m_val); }
    bool isBool() const { return isValid() && yyjson_is_bool(m_val); }
    bool isNumber() const { return isValid() && yyjson_is_num(m_val); }
    bool isInt() const { return isValid() && yyjson_is_int(m_val); }
    bool isSint() const { return isValid() && yyjson_is_sint(m_val); }
    bool isUint() const { return isValid() && yyjson_is_uint(m_val); }
    bool isReal() const { return isValid() && yyjson_is_real(m_val); }
    bool isString() const { return isValid() && yyjson_is_str(m_val); }
    bool isArray() const { return isValid() && yyjson_is_arr(m_val); }
    bool isObject() const { return isValid() && yyjson_is_obj(m_val); }
    const char* typeName() const { return yyjson_get_type_desc(m_val); }
    yyjson_type getType() const { return isValid() ? yyjson_get_type(m_val) : YYJSON_TYPE_NONE; }
    
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
    template<typename T>
    T getor(const T& defaultValue) const;
    // Special overloads for sentinels
    const char* getor(EmptyString) const;
    double getor(ZeroNumber) const;
    
    // Array/Object size and access by index or key
    size_t size() const;
    Value index(size_t idx) const;
    Value index(int idx) const;
    Value index(const char* key) const;
    Value index(const std::string& key) const;
    
    // Array and object access operator
    template <typename T>
    Value operator[](const T& index) const
    {
        return this->index(index);
    }
    
    // Path operations. 
    // Like index in the simplest case, but can resolve deep path.
    Value pathto(const char* path) const;
    Value pathto(const std::string& path) const { return pathto(path.c_str()); }
    Value pathto(int idx) const { return index(idx); }
    Value pathto(size_t idx) const { return index(idx); }
    
    // Iterator creation methods
    ArrayIterator arrayIter(size_t startIndex = 0) const;
    ObjectIterator objectIter(const char* startKey = nullptr) const;
    
    // Standard iterator pattern methods
    ArrayIterator beginArray() const;
    ArrayIterator endArray() const;
    ObjectIterator beginObject() const;
    ObjectIterator endObject() const;
    
    // Conversion methods
    std::string toString(bool pretty = false) const;
    int toInteger() const;
    double toNumber() const;
    
    // Comparison method
    bool equal(const Value& other) const;
    bool less(const Value& other) const;

    // Pipe function for custom transformations
    template<typename Func>
    auto pipe(Func&& func) const -> decltype(func(*this))
    {
        return std::forward<Func>(func)(*this);
    }

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
    Value operator[](const T& index) const
    {
        return root().index(index);
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
    
    // Access underlying yyjson value. yyjson api use non-const pointer.
    yyjson_mut_val* get() const { return m_val; }
    
    // Set underlying yyjson value pointer directly
    void set(yyjson_mut_val* val) { m_val = val; }
    
    // Error checking
    bool isValid() const { return m_val != nullptr && m_doc != nullptr; }
    bool hasError() const { return !isValid(); }
    bool operator!() const { return hasError(); }
    explicit operator bool() const { return isValid(); }

    // Type checking (same as read-only)
    bool isNull() const { return isValid() && yyjson_mut_is_null(m_val); }
    bool isBool() const { return isValid() && yyjson_mut_is_bool(m_val); }
    bool isNumber() const { return isValid() && yyjson_mut_is_num(m_val); }
    bool isInt() const { return isValid() && yyjson_mut_is_int(m_val); }
    bool isSint() const { return isValid() && yyjson_mut_is_sint(m_val); }
    bool isUint() const { return isValid() && yyjson_mut_is_uint(m_val); }
    bool isReal() const { return isValid() && yyjson_mut_is_real(m_val); }
    bool isString() const { return isValid() && yyjson_mut_is_str(m_val); }
    bool isArray() const { return isValid() && yyjson_mut_is_arr(m_val); }
    bool isObject() const { return isValid() && yyjson_mut_is_obj(m_val); }
    const char* typeName() const { return yyjson_mut_get_type_desc(m_val); }
    yyjson_type getType() const { return isValid() ? yyjson_mut_get_type(m_val) : YYJSON_TYPE_NONE; }
    
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
    T getor(const T& defaultValue) const;
    // Special overloads for sentinels
    const char* getor(EmptyString) const;
    double getor(ZeroNumber) const;
    
    // Array/Object size and access by index or key
    size_t size() const;
    MutableValue index(size_t idx) const;
    MutableValue index(int idx) const;
    MutableValue index(const char* key) const;
    MutableValue index(const std::string& key) const;
    // non-const version for automatic object insertion
    MutableValue index(size_t idx);
    MutableValue index(int idx);
    MutableValue index(const char* key);
    MutableValue index(const std::string& key);

    // Array and object access operator (const version)
    template <typename T>
    MutableValue operator[](const T& index) const
    {
        return this->index(index);
    }
    
    // Array and object access operator (non-const version)
    template <typename T>
    MutableValue operator[](const T& index)
    {
        return this->index(index);
    }

    // Iterator creation methods
    MutableArrayIterator arrayIter(size_t startIndex = 0) const;
    MutableObjectIterator objectIter(const char* startKey = nullptr) const;
    
    // Standard iterator pattern methods
    MutableArrayIterator beginArray() const;
    MutableArrayIterator endArray() const;
    MutableObjectIterator beginObject() const;
    MutableObjectIterator endObject() const;
    
    // Path operations
    MutableValue pathto(const char* path) const;
    MutableValue pathto(const std::string& path) const { return pathto(path.c_str()); }
    MutableValue pathto(int idx) const { return index(idx); }
    MutableValue pathto(size_t idx) const { return index(idx); }
    
    // Set(Assignment) methods.
    // Should not be used on an existing object or array.
    MutableValue& set(std::nullptr_t value);
    MutableValue& set(bool value);
    MutableValue& set(int value);
    MutableValue& set(int64_t value);
    MutableValue& set(uint64_t value);
    MutableValue& set(double value);
    MutableValue& set(const char* value);
    MutableValue& set(const std::string& value);
    MutableValue& set(StringRef value);
    MutableValue& setRef(const char* value);
    MutableValue& setCopy(const char* value, size_t len);
    MutableValue& setArray();
    MutableValue& setObject();
    // Set using empty-type sentinels
    MutableValue& set(EmptyArray);
    MutableValue& set(EmptyObject);
    MutableValue& set(EmptyString);
    MutableValue& set(ZeroNumber);

    // No effect set to other types.
    template <typename T>
    MutableValue& set(const T&) { return *this; }
    
    // Assignment operators
    template <typename T>
    MutableValue& operator=(const T& value)
    {
        return set(value);
    }

    // specified set to literal string.
    template <size_t N> 
    MutableValue& operator=(const char(&value)[N]);
    template <size_t N> 
    MutableValue& operator=(char(&value)[N]);

    // Append methods for array.
    MutableValue& append(yyjson_mut_val* value);
    // T as create: bool | int | string | (Mutable)Document | (Mutable)Value
    template <typename T>
    MutableValue& append(T&& value);
    
    // Insert methods for object.
    MutableValue& add(yyjson_mut_val* key, yyjson_mut_val* value);
    MutableValue& add(KeyValue&& kv);
    // valT as create: bool | int | string | (Mutable)Document | (Mutable)Value
    template<typename keyT, typename valT>
    MutableValue& add(keyT&& key, valT&& value);
    
    // KeyValue creation methods for optimized object insertion
    KeyValue tag(MutableValue&& key) &&;
    template <typename T>
    KeyValue tag(T&& key) &&;

    MutableValue& input(KeyValue&& kv);

    template<typename keyT, typename valT>
    MutableValue& input(std::pair<keyT, valT>&& kv);

    // Smart input method, append for array, add for object, assign for scalar
    template <typename T>
    MutableValue& input(T&& value);

    // Input pending key, only support string type.
    template <typename T>
    bool inputKey(T&& key);

    // Input value after pending key.
    template <typename T>
    bool inputValue(T&& value);

    // Conversion methods
    std::string toString(bool pretty = false) const;
    int toInteger() const;
    double toNumber() const;
    
    // Comparison method
    bool equal(const MutableValue& other) const;
    bool less(const MutableValue& other) const;
    

    // Pipe function for custom transformations
    template<typename Func>
    auto pipe(Func&& func) const -> decltype(func(*this))
    {
        return std::forward<Func>(func)(*this);
    }

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
    MutableValue createArray() const;  // []
    MutableValue createObject() const; // {}
    MutableValue createRef(const char* value) const; // string without copy
    // T: bool | int | string | (Mutable)Document | (Mutable)Value
    template <typename T> MutableValue create(const T& value) const;

    // Index access - const version for read-only access
    template <typename T>
    MutableValue operator[](const T& index) const
    {
        return root().index(index);
    }
    
    // Index access - non-const version for automatic insertion
    template <typename T>
    MutableValue operator[](const T& index)
    {
        return root().index(index);
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
    StringRef(const char(&value)[N]) : str(value), len(N-1)
    {
    }

    explicit StringRef(const char* value) : str(value), len(::strlen(value))
    {
    }

    explicit StringRef(const char* value, size_t n) : str(value), len(n)
    {
    }

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
 * - Increment: ++iter (calls next()), iter++ (calls Next())
 * - Advance: iter + n, iter += n (calls advance(n))
 * - Position: iter % n , iter %= n (calls seek(n))
 * - Comparison: ==, != (calls equal())
 * - Boolean: if (iter), !iter
 */
class ArrayIterator
{
public:
    struct Item
    {
        size_t key = 0; // Array index (0-based)
        Value value;    // Current array element value
    };
    
    ArrayIterator() : m_root(nullptr), m_iter({}) {}
    explicit ArrayIterator(yyjson_val* root);
    
    // Check if iterator is valid
    bool isValid() const { return m_current.value.isValid(); }
    
    // Boolean conversion for conditional checks
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }
    
    // Check if two iterators are equal
    bool equal(const ArrayIterator& other) const
    {
        return m_current.value.get() == other.m_current.value.get();
    }
    
    // Move to next element
    ArrayIterator& next();
    
    // Create a copy of current state, then move to next element (for postfix ++)
    ArrayIterator Next();
    
    // Get current item
    Item& operator*() { return m_current; }
    const Item& operator*() const { return m_current; }
    
    // Get current item pointer
    Item* operator->() { return &m_current; }
    const Item* operator->() const { return &m_current; }
    
    // Position manipulation
    ArrayIterator& advance(size_t steps = 1);          // Advance iterator by n steps
    ArrayIterator& seek(size_t index, bool reset = true); // Seek to specific index
    
    // Rewind to beginning
    ArrayIterator& rewind();
    
private:
    yyjson_val* m_root = nullptr;  // Root array for context  
    yyjson_arr_iter m_iter;        // Native yyjson array iterator (read-only)
    Item m_current;                // Current item (index and value)
};

/**
 * @brief Iterator for JSON object key-value pairs
 * Provides efficient iteration over object properties
 * 
 * Supported operators:
 * - Dereference: *iter, iter-> (access current key-value pair)
 * - Increment: ++iter (calls next()), iter++ (calls Next())
 * - Advance: iter + n, iter += n (calls advance(n))
 * - Position: iter % key , iter %= key (calls seek(key))
 * - Comparison: ==, != (calls equal())
 * - Boolean: if (iter), !iter
 */
class ObjectIterator
{
public:
    struct Item
    {
        const char* key = nullptr; // Object key
        Value value;               // Current value
    };
    
    ObjectIterator() : m_root(nullptr), m_iter({}) {}
    explicit ObjectIterator(yyjson_val* root);
    
    // Check if iterator is valid
    bool isValid() const { return m_current.value.isValid(); }
    
    // Boolean conversion for conditional checks
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }
    
    // Check if two iterators are equal
    bool equal(const ObjectIterator& other) const
    {
        return m_current.value.get() == other.m_current.value.get();
    }

    // Move to next key-value pair
    ObjectIterator& next();
    
    // Create a copy of current state, then move to next element (for postfix ++)
    ObjectIterator Next();
    
    // Get current item
    Item& operator*() { return m_current; }
    const Item& operator*() const { return m_current; }
    
    // Get current item pointer
    Item* operator->() { return &m_current; }
    const Item* operator->() const { return &m_current; }
    
    // Position manipulation  
    ObjectIterator& advance(size_t steps = 1);          // Advance iterator by n steps
    ObjectIterator& seek(const char* key, bool reset = true); // Seek to specific key
    
    // Rewind to beginning
    ObjectIterator& rewind();
    
private:
    yyjson_val* m_root = nullptr;  // Root object for context
    yyjson_obj_iter m_iter;        // Native yyjson object iterator (read-only)
    Item m_current;                // Current item (key and value)
};

/** 
 * @brief Mutable iterator for JSON array elements
 * Provides efficient iteration over mutable array elements with write access
 * 
 * Supported operators:
 * - Dereference: *iter, iter-> (access current mutable item)
 * - Increment: ++iter (calls next()), iter++ (calls Next())
 * - Advance: iter + n, iter += n (calls advance(n))
 * - Position: iter % n , iter %= n (calls seek(n))
 * - Comparison: ==, != (calls equal())
 * - Boolean: if (iter), !iter
 */
class MutableArrayIterator
{
public:
    struct Item
    {
        size_t key = 0;          // Array index (0-based)
        MutableValue value;      // Current array element value (mutable)
    };
    
    MutableArrayIterator() : m_root(nullptr), m_iter({}) {}
    explicit MutableArrayIterator(yyjson_mut_val* root, yyjson_mut_doc* doc);
    
    // Check if iterator is valid
    bool isValid() const { return m_current.value.isValid(); }
    
    // Boolean conversion for conditional checks
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }
    
    // Check if two iterators are equal
    bool equal(const MutableArrayIterator& other) const
    {
        return m_current.value.get() == other.m_current.value.get();
    }

    // Move to next element
    MutableArrayIterator& next();
    
    // Create a copy of current state, then move to next element (for postfix ++)
    MutableArrayIterator Next();
    
    // Get current item
    Item& operator*() { return m_current; }
    const Item& operator*() const { return m_current; }
    
    // Get current item pointer
    Item* operator->() { return &m_current; }
    const Item* operator->() const { return &m_current; }
    
    // Position manipulation
    MutableArrayIterator& advance(size_t steps = 1);          // Advance iterator by n steps
    MutableArrayIterator& seek(size_t index, bool reset = true); // Seek to specific index
    
    // Rewind to beginning
    MutableArrayIterator& rewind();
    
private:
    yyjson_mut_val* m_root = nullptr;  // Root array for context  
    yyjson_mut_doc* m_doc = nullptr;   // Document for context (needed for mutation)
    yyjson_mut_arr_iter m_iter;        // Native yyjson mutable array iterator
    Item m_current;                    // Current item (index and mutable value)
};

/** 
 * @brief Mutable iterator for JSON object key-value pairs
 * Provides efficient iteration over mutable object properties with write access
 * 
 * Supported operators:
 * - Dereference: *iter, iter-> (access current mutable key-value pair)
 * - Increment: ++iter (calls next()), iter++ (calls Next())
 * - Advance: iter + n, iter += n (calls advance(n))
 * - Position: iter % key , iter %= key (calls seek(key))
 * - Comparison: ==, != (calls equal())
 * - Boolean: if (iter), !iter
 */
class MutableObjectIterator
{
public:
    struct Item
    {
        const char* key = nullptr; // Object key
        MutableValue value;        // Current value (mutable)
    };
    
    MutableObjectIterator() : m_root(nullptr), m_iter({}) {}
    explicit MutableObjectIterator(yyjson_mut_val* root, yyjson_mut_doc* doc);
    
    // Check if iterator is valid
    bool isValid() const { return m_current.value.isValid(); }
    
    // Boolean conversion for conditional checks
    explicit operator bool() const { return isValid(); }
    bool operator!() const { return !isValid(); }
    
    // Check if two iterators are equal
    bool equal(const MutableObjectIterator& other) const
    {
        return m_current.value.get() == other.m_current.value.get();
    }

    // Move to next key-value pair
    MutableObjectIterator& next();
    
    // Create a copy of current state, then move to next element (for postfix ++)
    MutableObjectIterator Next();
    
    // Get current item
    Item& operator*() { return m_current; }
    const Item& operator*() const { return m_current; }
    
    // Get current item pointer
    Item* operator->() { return &m_current; }
    const Item* operator->() const { return &m_current; }
    
    // Position manipulation  
    MutableObjectIterator& advance(size_t steps = 1);          // Advance iterator by n steps
    MutableObjectIterator& seek(const char* key, bool reset = true); // Seek to specific key
    
    // Rewind to beginning
    MutableObjectIterator& rewind();
    
private:
    yyjson_mut_val* m_root = nullptr;  // Root object for context
    yyjson_mut_doc* m_doc = nullptr;   // Document for context (needed for mutation)
    yyjson_mut_obj_iter m_iter;        // Native yyjson mutable object iterator
    Item m_current;                    // Current item (key and mutable value)
};

/* @Section 2.5: Type Traits */
/* ------------------------------------------------------------------------ */

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
template<> struct is_key<const char*> : std::true_type {}; // and const char[N]
template<> struct is_key<char*> : std::true_type {};       // and char[N]
template<> struct is_key<std::string> : std::true_type {};

// Helper constexpr function to check if a type can be used as key
// Handles array-to-pointer decay and string literals
template<typename T>
constexpr bool is_key_type() {
    using decayed_t = std::decay_t<T>;
    return is_key<decayed_t>::value;
}

/* @Part 3: Non-Class Functions */
/* ======================================================================== */

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
inline typename std::enable_if<std::is_same<T, const char*>::value || std::is_same<T, char*>::value, yyjson_mut_val*>::type
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

inline yyjson_mut_val* createRef(yyjson_mut_doc* doc, const char* value)
{
    return create(doc, StringRef(value));
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

inline yyjson_mut_val* create(yyjson_mut_doc* doc, const MutableDocument& src)
{
    return create(doc, src.root());
}

/* @Section 3.2: Conversion Helper Functions */
/* ------------------------------------------------------------------------ */

// Helper function to get yyjson_val* from both Value and MutableValue

/// Compare two JSON values (Value or MutableValue) using hybrid logic.
template<typename T>
inline typename std::enable_if<is_value<T>::value, bool>::type
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
inline typename std::enable_if<is_value<jsonT>::value, int>::type
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
inline T Value::getor(const T& defaultValue) const
{
    T result;
    return get(result) ? result : defaultValue;
}

inline const char* Value::getor(EmptyString) const
{
    return isString() ? yyjson_get_str(m_val) : "";
}

inline double Value::getor(ZeroNumber) const
{
    return toNumber();
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

inline Value Value::index(int idx) const
{
    return index(static_cast<size_t>(idx));
}

inline Value Value::index(const char* key) const
{
    if (!isObject() || !key) return Value(nullptr);
    return Value(yyjson_obj_get(m_val, key));
}

inline Value Value::index(const std::string& key) const
{
    return index(key.c_str());
}

inline Value Value::pathto(const char* path) const
{
    if (!path || !*path) return *this;
    
    if (*path == '/') {
        // JSON Pointer support
        yyjson_val* result = yyjson_ptr_get(m_val, path);
        return Value(result);
    }
    
    // Single level indexing for backward compatibility
    return index(path);
}

/* @Group 4.1.3: create iterator */
/* ************************************************************************ */

inline ArrayIterator Value::arrayIter(size_t startIndex) const
{
    if (isArray()) {
        ArrayIterator iter(m_val);
        if (startIndex > 0) {
            iter.advance(startIndex);
        }
        return iter;
    }
    return ArrayIterator();
}

inline ObjectIterator Value::objectIter(const char* startKey) const
{
    if (isObject()) {
        ObjectIterator iter(m_val);
        if (startKey && *startKey) {
            iter.seek(startKey);
        }
        return iter;
    }
    return ObjectIterator();
}

inline ArrayIterator Value::beginArray() const
{
    return arrayIter(0);
}

inline ArrayIterator Value::endArray() const
{
    return ArrayIterator();
}

inline ObjectIterator Value::beginObject() const
{
    return objectIter(nullptr);
}

inline ObjectIterator Value::endObject() const
{
    return ObjectIterator();
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
    return yyjson::toIntegerCast(*this);
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
    return lessCompare(*this, other);
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
inline T MutableValue::getor(const T& defaultValue) const
{
    T result;
    return get(result) ? result : defaultValue;
}

inline const char* MutableValue::getor(EmptyString) const
{
    return isString() ? yyjson_mut_get_str(m_val) : "";
}

inline double MutableValue::getor(ZeroNumber) const
{
    return toNumber();
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

inline MutableValue MutableValue::index(int idx) const
{
    return index(static_cast<size_t>(idx));
}

inline MutableValue MutableValue::index(const char* key) const
{
    if (!isObject() || !key) return MutableValue(nullptr, m_doc);
    return MutableValue(yyjson_mut_obj_get(m_val, key), m_doc);
}

inline MutableValue MutableValue::index(const std::string& key) const
{
    return index(key.c_str());
}

inline MutableValue MutableValue::index(size_t idx)
{
    if (!isArray()) return MutableValue(nullptr, m_doc);
    return MutableValue(yyjson_mut_arr_get(m_val, idx), m_doc);
}

inline MutableValue MutableValue::index(int idx)
{
    return index(static_cast<size_t>(idx));
}

inline MutableValue MutableValue::index(const char* key)
{
    if (!isObject() || !key) return MutableValue(nullptr, m_doc);
    yyjson_mut_val* val = yyjson_mut_obj_get(m_val, key);
    if (!val) {
        // Key doesn't exist, insert null value
        val = yyjson_mut_null(m_doc);
        yyjson_mut_val* key_val = yyjson_mut_strcpy(m_doc, key);
        yyjson_mut_obj_add(m_val, key_val, val);
    }
    return MutableValue(val, m_doc);
}

inline MutableValue MutableValue::index(const std::string& key)
{
    return index(key.c_str());
}

inline MutableValue MutableValue::pathto(const char* path) const
{
    if (!path || !*path) return *this;
    
    if (*path == '/') {
        // JSON Pointer support
        yyjson_mut_val* result = yyjson_mut_ptr_get(m_val, path);
        return MutableValue(result, m_doc);
    }
    
    // Single level indexing for backward compatibility
    return index(path);
}

/* @Group 4.3.3: assignment set */
/* ************************************************************************ */

inline MutableValue& MutableValue::set(std::nullptr_t value)
{
    if (isValid())
    {
        yyjson_mut_set_null(m_val);
    }
    return *this;
}

inline MutableValue& MutableValue::set(bool value)
{
    if (isValid())
    {
        yyjson_mut_set_bool(m_val, value);
    }
    return *this;
}

inline MutableValue& MutableValue::set(int value)
{
    if (isValid())
    {
        yyjson_mut_set_int(m_val, value);
    }
    return *this;
}

inline MutableValue& MutableValue::set(int64_t value)
{
    if (isValid())
    {
        yyjson_mut_set_sint(m_val, value);
    }
    return *this;
}

inline MutableValue& MutableValue::set(uint64_t value)
{
    if (isValid())
    {
        yyjson_mut_set_uint(m_val, value);
    }
    return *this;
}

inline MutableValue& MutableValue::set(double value)
{
    if (isValid())
    {
        yyjson_mut_set_real(m_val, value);
    }
    return *this;
}

inline MutableValue& MutableValue::set(const char* value)
{
    return setCopy(value, ::strlen(value));
}

inline MutableValue& MutableValue::set(const std::string& value)
{
    return setCopy(value.c_str(), value.size());
}

inline MutableValue& MutableValue::set(StringRef value)
{
    if (value.len == 2)
    {
        // Special handling for empty object and array literals
        if (::strcmp(value, "{}") == 0)
        {
            return setObject();
        }
        else if (::strcmp(value, "[]") == 0)
        {
            return setArray();
        }
    }
    if (isValid())
    {
        yyjson_mut_set_strn(m_val, value.str, value.len);
    }
    return *this;
}

inline MutableValue& MutableValue::setRef(const char* value)
{
    return set(StringRef(value));
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

inline MutableValue& MutableValue::set(EmptyArray)
{
    return setArray();
}
inline MutableValue& MutableValue::set(EmptyObject)
{
    return setObject();
}
inline MutableValue& MutableValue::set(EmptyString)
{
    return set("");
}
inline MutableValue& MutableValue::set(ZeroNumber)
{
    return set(0.0);
}

template <size_t N> 
MutableValue& MutableValue::operator=(const char(&value)[N])
{
    return set(StringRef(value));
}

template <size_t N> 
MutableValue& MutableValue::operator=(char(&value)[N])
{
    return set(const_cast<const char*>(value));
}

/* @Group 4.3.4: array append */
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
    return append(create(m_doc, std::forward<T>(value)));
}

/* @Group 4.3.5: object add */
/* ************************************************************************ */

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
        yyjson_mut_val* keyNode = nullptr;
        
        if constexpr (is_key_type<keyT>()) {
            keyNode = create(m_doc, std::forward<keyT>(key));
        }
        else {
            keyNode = create(m_doc, std::forward<keyT>(key));
            if (!yyjson_mut_is_str(keyNode)) {
                return *this;
            }
        }
        
        yyjson_mut_val* valNode = create(m_doc, std::forward<valT>(value));
        yyjson_mut_obj_add(m_val, keyNode, valNode);
    }
    return *this;
}

/* @Group 4.3.6: tag create KeyValue */
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

template <typename T>
inline KeyValue MutableValue::tag(T&& key) &&
{
    yyjson_mut_val* keyNode = nullptr;
    
    if constexpr (is_key_type<T>()) {
        keyNode = create(m_doc, std::forward<T>(key));
    }
    else {
        keyNode = create(m_doc, std::forward<T>(key));
        if (!yyjson_mut_is_str(keyNode)) {
            keyNode = nullptr;
        }
    }
    
    auto ret = KeyValue(keyNode, m_val);
    m_val = nullptr;
    return ret;
}

/* @Group 4.3.7: smart input */
/* ************************************************************************ */

// todo: is_key concept
template <typename T>
inline bool MutableValue::inputKey(T&& key)
{
    yyjson_mut_val* keyNode = nullptr;
    
    if constexpr (is_key_type<T>()) {
        keyNode = create(m_doc, std::forward<T>(key));
    }
    else {
        keyNode = create(m_doc, std::forward<T>(key));
        if (!yyjson_mut_is_str(keyNode)) {
            keyNode = nullptr;
        }
    }
    
    m_pendingKey = keyNode;
    return m_pendingKey != nullptr;
}

template <typename T>
inline bool MutableValue::inputValue(T&& value)
{
    if (m_pendingKey == nullptr)
    {
        return false;
    }
    yyjson_mut_val* succeedVal = create(m_doc, std::forward<T>(value));
    add(m_pendingKey, succeedVal);
    m_pendingKey = nullptr;
    return true;
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

template<typename keyT, typename valT>
inline MutableValue& MutableValue::input(std::pair<keyT, valT>&& kv)
{
    if (isObject())
    {
        return add(std::forward<keyT>(kv.first), std::forward<valT>(kv.second));
    }
    return *this;
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
    else
    {
        return set(std::forward<T>(value));
    }
    return *this;
}

/* @Group 4.3.8: create iterator */
/* ************************************************************************ */

inline MutableArrayIterator MutableValue::arrayIter(size_t startIndex) const
{
    if (isArray()) {
        MutableArrayIterator iter(m_val, m_doc);
        if (startIndex > 0) {
            iter.advance(startIndex);
        }
        return iter;
    }
    return MutableArrayIterator();
}

inline MutableObjectIterator MutableValue::objectIter(const char* startKey) const
{
    if (isObject()) {
        MutableObjectIterator iter(m_val, m_doc);
        if (startKey && *startKey) {
            iter.seek(startKey);
        }
        return iter;
    }
    return MutableObjectIterator();
}

inline MutableArrayIterator MutableValue::beginArray() const
{
    return arrayIter(0);
}

inline MutableArrayIterator MutableValue::endArray() const
{
    return MutableArrayIterator();
}

inline MutableObjectIterator MutableValue::beginObject() const
{
    return objectIter(nullptr);
}

inline MutableObjectIterator MutableValue::endObject() const
{
    return MutableObjectIterator();
}

/* @Group 4.3.9: others */
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
    return yyjson::toIntegerCast(*this);
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
    return lessCompare(*this, other);
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

inline MutableValue MutableDocument::createRef(const char* value) const
{ 
    return create(StringRef(value));
}

inline MutableValue MutableDocument::createArray() const
{ 
    return create(yyjson_mut_arr(m_doc)); 
}

inline MutableValue MutableDocument::createObject() const
{ 
    return create(yyjson_mut_obj(m_doc)); 
}

template <typename T>
inline MutableValue MutableDocument::create(const T& value) const
{
    return create(yyjson::create(m_doc, value));
}

/* @Section 4.5: ArrayIterator Methods */
/* ------------------------------------------------------------------------ */

inline ArrayIterator::ArrayIterator(yyjson_val* root) : m_root(root) 
{
    if (root) {
        yyjson_arr_iter_init(root, &m_iter);
        m_current.value.set(yyjson_arr_iter_next(&m_iter));
        m_current.key = 0;
    }
}

inline ArrayIterator& ArrayIterator::next()
{
    m_current.value.set(yyjson_arr_iter_next(&m_iter));
    m_current.key++;
    return *this;
}

inline ArrayIterator ArrayIterator::Next()
{
    ArrayIterator old = *this;
    next();
    return old;
}

inline ArrayIterator& ArrayIterator::advance(size_t steps)
{
    for (int i = 0; i < steps; i++) { next(); }
    return *this;
}

inline ArrayIterator& ArrayIterator::seek(size_t index, bool reset)
{
    if (reset) { rewind(); }
    advance(index);
    return *this;
}

inline ArrayIterator& ArrayIterator::rewind()
{
    yyjson_arr_iter_init(m_root, &m_iter);
    m_current.value.set(yyjson_arr_iter_next(&m_iter));
    m_current.key = 0;
    return *this;
}

/* @Section 4.6: ObjectIterator Methods */
/* ------------------------------------------------------------------------ */

inline ObjectIterator::ObjectIterator(yyjson_val* root) : m_root(root) 
{
    if (root) {
        yyjson_obj_iter_init(root, &m_iter);
        yyjson_val* currentKey = yyjson_obj_iter_next(&m_iter);
        if (currentKey) {
            m_current.key = yyjson_get_str(currentKey);
            m_current.value.set(yyjson_obj_iter_get_val(currentKey));
        }
    }
}

inline ObjectIterator& ObjectIterator::next()
{
    yyjson_val* currentKey = yyjson_obj_iter_next(&m_iter);
    if (currentKey) {
        m_current.key = yyjson_get_str(currentKey);
        m_current.value.set(yyjson_obj_iter_get_val(currentKey));
    } else {
        m_current.key = nullptr;
        m_current.value.set(nullptr);
    }
    return *this;
}

inline ObjectIterator ObjectIterator::Next()
{
    ObjectIterator old = *this;
    next();
    return old;
}

inline ObjectIterator& ObjectIterator::advance(size_t steps)
{
    for (int i = 0; i < steps; i++) { next(); }
    return *this;
}

inline ObjectIterator& ObjectIterator::seek(const char* key, bool reset)
{
    if (!key || !*key) return *this;
    
    if (reset) { rewind(); }
    
    // Search for the key from current position
    while (m_current.value.isValid()) {
        if (m_current.key && strcmp(m_current.key, key) == 0) {
            break; // Found the key
        }
        next();
    }
    return *this;
}

inline ObjectIterator& ObjectIterator::rewind()
{
    yyjson_obj_iter_init(m_root, &m_iter);
    yyjson_val* currentKey = yyjson_obj_iter_next(&m_iter);
    if (currentKey) {
        m_current.key = yyjson_get_str(currentKey);
        m_current.value.set(yyjson_obj_iter_get_val(currentKey));
    } else {
        m_current.key = nullptr;
        m_current.value.set(nullptr);
    }
    return *this;
}

/* @Section 4.7: MutableArrayIterator Methods */
/* ------------------------------------------------------------------------ */

inline MutableArrayIterator::MutableArrayIterator(yyjson_mut_val* root, yyjson_mut_doc* doc) 
    : m_root(root), m_doc(doc)
{
    if (root) {
        yyjson_mut_arr_iter_init(root, &m_iter);
        yyjson_mut_val* nextVal = yyjson_mut_arr_iter_next(&m_iter);
        m_current.value = MutableValue(nextVal, m_doc);
        m_current.key = 0;
    }
}

inline MutableArrayIterator& MutableArrayIterator::next()
{
    yyjson_mut_val* nextVal = yyjson_mut_arr_iter_next(&m_iter);
    m_current.value.set(nextVal);
    m_current.key++;
    return *this;
}

inline MutableArrayIterator MutableArrayIterator::Next()
{
    MutableArrayIterator old = *this;
    next();
    return old;
}

inline MutableArrayIterator& MutableArrayIterator::advance(size_t steps)
{
    for (int i = 0; i < steps; i++) { next(); }
    return *this;
}

inline MutableArrayIterator& MutableArrayIterator::seek(size_t index, bool reset)
{
    if (reset) { rewind(); }
    advance(index);
    return *this;
}

inline MutableArrayIterator& MutableArrayIterator::rewind()
{
    if (m_root) {
        yyjson_mut_arr_iter_init(m_root, &m_iter);
        yyjson_mut_val* nextVal = yyjson_mut_arr_iter_next(&m_iter);
        m_current.value.set(nextVal);
        m_current.key = 0;
    }
    return *this;
}

/* @Section 4.8: MutableObjectIterator Methods */
/* ------------------------------------------------------------------------ */

inline MutableObjectIterator::MutableObjectIterator(yyjson_mut_val* root, yyjson_mut_doc* doc) 
    : m_root(root), m_doc(doc)
{
    if (root) {
        yyjson_mut_obj_iter_init(root, &m_iter);
        yyjson_mut_val* currentKey = yyjson_mut_obj_iter_next(&m_iter);
        if (currentKey) {
            m_current.key = yyjson_mut_get_str(currentKey);
            m_current.value = MutableValue(yyjson_mut_obj_iter_get_val(currentKey), m_doc);
        }
    }
}

inline MutableObjectIterator& MutableObjectIterator::next()
{
    yyjson_mut_val* currentKey = yyjson_mut_obj_iter_next(&m_iter);
    if (currentKey) {
        m_current.key = yyjson_mut_get_str(currentKey);
        m_current.value.set(yyjson_mut_obj_iter_get_val(currentKey));
    } else {
        m_current.key = nullptr;
        m_current.value.set(static_cast<yyjson_mut_val*>(nullptr));
    }
    return *this;
}

inline MutableObjectIterator MutableObjectIterator::Next()
{
    MutableObjectIterator old = *this;
    next();
    return old;
}

inline MutableObjectIterator& MutableObjectIterator::advance(size_t steps)
{
    for (int i = 0; i < steps; i++) { next(); }
    return *this;
}

inline MutableObjectIterator& MutableObjectIterator::seek(const char* key, bool reset)
{
    if (!key || !*key) return *this;
    
    if (reset) { rewind(); }
    
    // Search for the key from current position
    while (m_current.value.isValid()) {
        if (m_current.key && strcmp(m_current.key, key) == 0) {
            break; // Found the key
        }
        next();
    }
    return *this;
}

inline MutableObjectIterator& MutableObjectIterator::rewind()
{
    if (m_root) {
        yyjson_mut_obj_iter_init(m_root, &m_iter);
        yyjson_mut_val* currentKey = yyjson_mut_obj_iter_next(&m_iter);
        if (currentKey) {
            m_current.key = yyjson_mut_get_str(currentKey);
            m_current.value.set(yyjson_mut_obj_iter_get_val(currentKey));
        } else {
            m_current.key = nullptr;
            m_current.value.set(static_cast<yyjson_mut_val*>(nullptr));
        }
    }
    return *this;
}

/* @Part 5: Operator Interface */
/* ======================================================================== */

/* @Section 5.1: Primary Path Access */
/* ------------------------------------------------------------------------ */

// Value and MutableValue path operators
// `json / path` --> `json.pathto(path)`
template <typename jsonT, typename T>
inline typename std::enable_if<is_value<jsonT>::value, jsonT>::type
operator/(const jsonT& json, const T& path)
{
    return json.pathto(path);
}

// Value and MutableValue extraction operators
// `json | def` --> `json.getor(def)` : return json or default value
template<typename jsonT, typename T>
inline typename std::enable_if<is_value<jsonT>::value, T>::type
operator|(const jsonT& json, const T& defaultValue)
{
    return json.getor(defaultValue);
}

// Specialization for const char* to avoid template deduction issues
template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, const char*>::type
operator|(const jsonT& json, const char* defaultValue)
{
    return json.getor(defaultValue);
}
// Overloads for sentinels kString and kNumber
template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, const char*>::type
operator|(const jsonT& json, EmptyString)
{
    return json.getor(kString);
}

template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, double>::type
operator|(const jsonT& json, ZeroNumber)
{
    return json.getor(kNumber);
}

// `dest |= json` --> `dest = json | dest`;
template <typename valueT, typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, valueT&>::type
operator|=(valueT& dest, const jsonT& json)
{
    json.get(dest);
    return dest;
}

// `json >> dest` --> `json.get(dest)` : return if get successfully
template <typename valueT, typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator>>(const jsonT& json, valueT& dest)
{
    return json.get(dest);
}

// `json | func` --> `json.pipe(func)` --> `func(json)`
template<typename jsonT, typename Func>
inline auto operator|(const jsonT& json, Func&& func) -> decltype(func(json))
{
    return json.pipe(std::forward<Func>(func));
}

// Value and MutableValue type checking operators
// `json & type` --> `json.isType(type)` : return bool for type checking
template<typename jsonT, typename T>
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator&(const jsonT& json, const T& type)
{
    return json.isType(type);
}

// Specialization for const char* to avoid template deduction issues
template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator&(const jsonT& json, const char* type)
{
    return json.isType(type);
}

/* @Section 5.2: Conversion Unary Operator */
/* ------------------------------------------------------------------------ */

// `+json` --> `json.toInteger()` : convert any json to int
template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, int>::type
operator+(const jsonT& json)
{
    return json.toInteger();
}

// `-json` --> `json.toString()`
template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, std::string>::type
operator-(const jsonT& json)
{
    return json.toString();
}

// `*doc` --> `doc.root()` : returns root value
template<typename docT>
inline typename std::enable_if<is_document<docT>::value, typename docT::value_type>::type
operator*(docT& doc)
{
    return doc.root();
}

template<typename docT>
inline typename std::enable_if<is_document<docT>::value, typename docT::value_type>::type
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
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator==(const jsonT& lhs, const jsonT& rhs)
{
    return lhs.equal(rhs);
}

// Specialization for const char* to avoid pointer comparison issues
template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator==(const jsonT& json, const char* scalar)
{
    return (json & scalar) && (::strcmp(json | "", scalar) == 0);
}

template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator==(const char* scalar, const jsonT& json)
{
    return json == scalar; // Use the reverse implementation
}

// JSON value and scalar type comparison operators
template<typename jsonT, typename scalarT>
inline typename std::enable_if<is_value<jsonT>::value && !is_value<scalarT>::value, bool>::type
operator==(const jsonT& json, const scalarT& scalar)
{
    return (json & scalar) && ((json | scalar) == scalar);
}

// Scalar type and JSON value comparison operators
template<typename scalarT, typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value && !is_value<scalarT>::value, bool>::type
operator==(const scalarT& scalar, const jsonT& json)
{
    return json == scalar; // Use the reverse implementation
}

// Iterator comparison operators
template<typename iteratorT>
inline typename std::enable_if<is_iterator<iteratorT>::value, bool>::type
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
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator<(const jsonT& lhs, const jsonT& rhs)
{
    return lhs.less(rhs);
}

// Other comparison operators
template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator>(const jsonT& lhs, const jsonT& rhs)
{
    return rhs < lhs;
}

template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator<=(const jsonT& lhs, const jsonT& rhs)
{
    return !(rhs < lhs);
}

template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, bool>::type
operator>=(const jsonT& lhs, const jsonT& rhs)
{
    return !(lhs < rhs);
}

/* @Section 5.4: Create and Bind KeyValue */
/* ------------------------------------------------------------------------ */

// `doc * value` --> `doc.create(value)`
template <typename T>
inline MutableValue operator*(const MutableDocument& doc, const T& value)
{
    return doc.create(value);
}

template <size_t N>
inline MutableValue operator*(const MutableDocument& doc, const char(&value)[N])
{
    return doc.create(StringRef(value, N-1));
}

template <size_t N>
inline MutableValue operator*(const MutableDocument& doc, char(&value)[N])
{
    return doc.create(value);
}

template <typename T>
inline typename std::enable_if<is_key_type<T>(), KeyValue>::type
operator*(MutableValue&& json, T&& key)
{
    return std::move(json).tag(std::forward<T>(key));
}

template <typename T>
inline typename std::enable_if<is_key_type<T>(), KeyValue>::type
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
inline typename std::enable_if<is_document<docT>::value, bool>::type
operator<<(docT& doc, T& input)
{
    return doc.read(input);
}

// `doc >> output` --> `doc.write(output)`
template<typename docT, typename T>
inline typename std::enable_if<is_document<docT>::value, bool>::type
operator>>(const docT& doc, T& output)
{
    return doc.write(output);
}

// `std::ostream << json` --> output value as JSON string
template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, std::ostream&>::type
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
inline typename std::enable_if<is_value<jsonT>::value, typename jsonT::array_iterator>::type
operator%(const jsonT& json, int offset)
{
    return json.arrayIter(offset);
}

// Object iterator creation (json % const char*)
// `json % key` --> `json.objectIter(key)`
template<typename jsonT>
inline typename std::enable_if<is_value<jsonT>::value, typename jsonT::object_iterator>::type
operator%(const jsonT& json, const char* key)
{
    return json.objectIter(key);
}

// Prefix increment operator (++iter)
// `++iter` --> `iter.next()`
template<typename iteratorT>
inline typename std::enable_if<is_iterator<iteratorT>::value, iteratorT&>::type
operator++(iteratorT& iter)
{
    return iter.next();
}

// Postfix increment operator (iter++)
// `iter++` --> `iter.Next()`
template<typename iteratorT>
inline typename std::enable_if<is_iterator<iteratorT>::value, iteratorT>::type
operator++(iteratorT& iter, int)
{
    return iter.Next();
}

// Iterator advance operator (+= +)
// `iter += step` --> `iter.advance(step)`
template<typename iteratorT>
inline typename std::enable_if<is_iterator<iteratorT>::value, iteratorT&>::type
operator+=(iteratorT& iter, size_t step)
{
    return iter.advance(step);
}

// `iter + step` --> `copy iter.advance(step)`
template<typename iteratorT>
inline typename std::enable_if<is_iterator<iteratorT>::value, iteratorT>::type
operator+(iteratorT& iter, size_t step)
{
    iteratorT copy = iter;
    return copy.advance(step);
}

// Iterator seek operators (%= %)
// `iter %= target` --> `iter.seek(target)`
template<typename iteratorT, typename T>
inline typename std::enable_if<is_iterator<iteratorT>::value, iteratorT&>::type
operator%=(iteratorT& iter, const T& target)
{
    return iter.seek(target);
}

// `iter % target` --> `copy iter.seek(target)`
template<typename iteratorT, typename T>
inline typename std::enable_if<is_iterator<iteratorT>::value, iteratorT>::type
operator%(iteratorT& iter, const T& target)
{
    iteratorT copy = iter;
    return copy.seek(target);
}

/* @Section 5.7: Document Forward Root Operator */
/* ------------------------------------------------------------------------ */

// `doc / path` : path from root
template <typename docT, typename T>
inline typename std::enable_if<is_document<docT>::value, typename docT::value_type>::type
operator/(const docT& doc, const T& path)
{
    return doc.root() / path;
}

// `doc % path` : create iterator for root
template <typename docT, typename T>
inline typename std::enable_if<is_document<docT>::value, typename docT::value_type>::type
operator%(const docT& doc, const T& path)
{
    return doc.root() % path;
}

// `+doc` : convert root to integer
template<typename docT>
inline typename std::enable_if<is_document<docT>::value, int>::type
operator+(docT& doc)
{
    return +doc.root();
}

template<typename docT>
inline typename std::enable_if<is_document<docT>::value, int>::type
operator+(const docT& doc)
{
    return +doc.root();
}

// `-doc` : convert root to string
template<typename docT>
inline typename std::enable_if<is_document<docT>::value, std::string>::type
operator-(docT& doc)
{
    return -doc.root();
}

template<typename docT>
inline typename std::enable_if<is_document<docT>::value, std::string>::type
operator-(const docT& doc)
{
    return -doc.root();
}

// `doc == doc` : compare their root
template<typename docT>
inline typename std::enable_if<is_document<docT>::value, bool>::type
operator==(const docT& lhs, const docT& rhs)
{
    return lhs.root() == rhs.root();
}

// `std::ostream << doc` : output root as JSON string
template<typename docT>
inline typename std::enable_if<is_document<docT>::value, std::ostream&>::type
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

// Convenience macro for key-value pair construction
#define KV(key, value) std::make_pair(key, value)

} /* end of namespace yyjson:: */

#endif /* end of include guard: XYJSON_H__ */
