/**
 * @file t_usage.cpp
 * @author lymslive
 * @date 2025-11-10
 * @brief docs/usage.md 文档示例的单元测试
 * */
#include "couttast/couttast.h"
#include "xyjson.h"
#include <numeric>
#include <sstream>

// use to mark code snippet from document
#define MARKDOWN_CODE_SNIPPET

// begin code in markdown file with ```cpp
#ifdef MARKDOWN_CODE_SNIPPET
using namespace yyjson;
#endif
// end code in markdown file with ```

#define CODE(statement) statement

DEF_TAST(usage_2_1_1_read_json, "usage: 2.1.1 Document 读入操作 - 基础构造")
{
    // MARKDOWN_CODE_SNIPPET 之前的代码行不同步到文档
#ifdef MARKDOWN_CODE_SNIPPET
    DESC("宏语句不同步到文档");
    std::string jsonText = R"({"name": "Alice", "age": 30})";
    yyjson::Document doc(jsonText);

    COUT((doc / "name" | ""), "Alice");
    COUT((doc / "age" | 0), 30);
#endif
    // MARKDOWN_CODE_SNIPPET 之后的代码行不同步到文档
}

DEF_TAST(usage_2_1_1_error_check, "usage: 2.1.1 Document 读入操作 - 错误检查")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc1("I'm not json");
    if (doc1.hasError()) {
        DESC("下面的 //+ 注释行在同步到文档时要取消注释");
//+     std::cout << "非法 Json\n";
    }

    // 仅有一个字符串类型的根结点的 Json
    yyjson::Document doc2("\"I'm sure json\"");
    if (doc2) {
//+     std::cout << "合法 Json\n";
    }
#endif

    COUT((bool)doc1, false);
    COUT((bool)doc2, true);
}

DEF_TAST(usage_2_1_1_read_operator, "usage: 2.1.1 Document 读入操作 - << 操作符")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";
    if (!doc) {
//+     std::cerr << "JSON 解析失败" << std::endl;
    }
#endif

    COUT((doc / "name" | ""), "Alice");
    COUT((doc / "age" | 0), 30);
}

DEF_TAST(usage_2_1_1_lazy_read, "usage: 2.1.1 Document 读入操作 - 延迟读入")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    if (!doc) {
        doc << R"({"name": "Alice", "age": 30})"; // 第一次读入
    }

    if (!doc) {
        doc << R"({"name": "Alice", "age": 35})"; // 不会二次读入
    }

    COUT((doc / "name" | ""), "Alice");
#endif
}

DEF_TAST(usage_2_1_1_read_file, "usage: 2.1.1 Document 读入操作 - 文件读取")
{
    // 创建测试用临时文件
    {
        const char* filePath = "/tmp/input.json";
        FILE* fp = fopen(filePath, "w");
        if (fp) {
            fprintf(fp, "{\"name\": \"Alice\", \"age\": 30}\n");
            fclose(fp);
        }
    }

#ifdef MARKDOWN_CODE_SNIPPET
    const char* filePath = "/tmp/input.json";
    yyjson::Document doc;
    doc.readFile(filePath);
    COUT((doc / "name" | ""), "Alice");
    COUT((doc / "age" | 0), 30);

    FILE* fp = fopen(filePath, "r");
    if (fp) {
        doc << fp;
    }
    COUT((doc / "name" | ""), "Alice");
    COUT((doc / "age" | 0), 30);

    std::ifstream ifs(filePath);
    if (ifs) {
        doc << ifs;
    }
    COUT((doc / "name" | ""), "Alice");
    COUT((doc / "age" | 0), 30);
#endif

    // 清理临时文件
    ::remove(filePath);
}

DEF_TAST(usage_2_1_2_write_json, "usage: 2.1.2 Document 写出操作 - 基本输出")
{
#ifdef MARKDOWN_CODE_SNIPPET
    std::string jsonText = R"({"name": "Alice", "age": 30})";
    yyjson::MutableDocument mutDoc(jsonText);

    std::string strTarget;
    mutDoc >> strTarget;
    COUT(strTarget, R"({"name":"Alice","age":30})");

    const char* filePath = "/tmp/output.json";
    FILE* fp = fopen(filePath, "w");
    if (fp) {
        mutDoc >> fp;
    }

    std::ofstream ofs(filePath);
    if (ofs) {
        mutDoc >> ofs;
    }

    mutDoc.writeFile(filePath);
#endif
}

DEF_TAST(usage_2_1_2_stdout, "usage: 2.1.2 Document 写出操作 - 标准流输出")
{
#ifdef MARKDOWN_CODE_SNIPPET
    std::string jsonText = R"({"name": "Alice", "age": 30})";
    yyjson::MutableDocument mutDoc(jsonText);
//+ std::cout << mutDoc << std::endl;
    COUT(mutDoc);
    COUT((bool)mutDoc, true);
#endif
}

DEF_TAST(usage_2_2_1_root_access, "usage: 2.2.1 根结点访问")
{
#ifdef MARKDOWN_CODE_SNIPPET
    std::string jsonText = R"({"name": "Alice", "age": 30})";
    yyjson::Document doc(jsonText);

    // yyjson::Document root = doc.root();
    auto root = *doc;

    // 以下三行输出相同的结果
//+ std::cout << root << std::endl;
//+ std::cout << doc << std::endl;
//+ std::cout << *doc << std::endl;

    COUT((doc / "name" | ""), "Alice");
    COUT((root / "name" | ""), "Alice");
    COUT(((*doc) / "name" | ""), "Alice");
#endif
}

DEF_TAST(usage_2_2_2_index_access, "usage: 2.2.2 索引操作 []")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30, "score": [10, 20, 30]})";

    auto json = doc["name"];
//+ std::cout << json << std::endl; // 输出: Alice
    COUT(json | "", "Alice");

    json = doc["age"];
//+ std::cout << json << std::endl; // 输出: 30
    COUT(json | 0, 30);

    json = doc["sex"];
    if (!json) {
//+     std::cout << "不可说" << std::endl;
    }
    COUT(!json, true);

    json = doc["score"][0];
//+ std::cout << json << std::endl; // 输出: 10
    COUT(json | 0, 10);

    json = doc["score"][10];
    if (!json) {
//+     std::cout << "数组越界" << std::endl;
    }
    COUT(!json, true);
#endif
}

DEF_TAST(usage_2_2_3_path_access, "usage: 2.2.3 路径操作 /")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30, "score": [10, 20, 30],
        "friend": [{"name": "Bob", "age": 35}, {"name": "Cous", "age": 20}]
    })";

    auto json = doc / "friend" / 0 / "age";
//+ std::cout << json << std::endl; // 输出: 35
    COUT((json | 0), 35);

    // 字段拼写错误
    json = doc / "Friend" / 1 / "age";
//+ std::cout << json << std::endl; // 输出: 空
    if (!json) {
//+     std::cout << "路径操作错误" << std::endl;
    }
    COUT(!json, true);
#endif
}

DEF_TAST(usage_2_2_4_json_pointer, "usage: 2.2.4 JSON Pointer 路径操作")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30, "score": [10, 20, 30],
        "friend": [{"name": "Bob", "age": 35}, {"name": "Cous", "age": 20}]
    })";

    auto json = doc / "/friend/0/age";
//+ std::cout << json << std::endl; // 输出: 35
    COUT((json | 0), 35);

    // 不是 Json Pointer ，与 [] 一样尝试找直接字段名
    json = doc / "friend/0/age";
    if (!json) {
//+     std::cout << "路径操作错误" << std::endl;
    }
    COUT(!json, true);
#endif
}

DEF_TAST(usage_2_3_1_extract_value, "usage: 2.3.1 带默认值的取值操作 |")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30, "ratio": 0.618, "pass": true})";

    const char* name = doc / "name" | "";
    int age = doc / "age" | 0;
    double ratio = doc / "ratio" | 0.0;
    bool pass = doc / "pass" | false;

    COUT(name, "Alice");
    COUT(age, 30);
    COUT(ratio > 0.617 && ratio < 0.619, true);
    COUT(pass, true);
#endif
}

DEF_TAST(usage_2_3_1_convert_value, "usage: 2.3.1 带默认值的取值操作 |")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30, "ratio": 0.618, "pass": true})";

    // | 先返回 const char* ，= 再转为 std::string
    std::string name = doc / "name" | "";
    COUT(name, "Alice");

    std::string nameDefault("unknown");
    std::string name2 = doc / "name" | nameDefault;
    std::string name3 = doc / "name" | std::string();
    COUT(name2, "Alice");
    COUT(name3, "Alice");

    // | 先返回 int ，= 再提升为 uint_64
    uint64_t age = doc / "age" | 0;
    COUT(age, 30);
#endif
}

DEF_TAST(usage_2_3_2_or_assign, "usage: 2.3.2 使用变量原来的默认值提取 |=")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    int age = 18;
    age = doc / "age" | age; // 30
    COUT(age, 30);
    age |= doc / "age";
    COUT(age, 30);
    age |= doc / "Age"; // 值不变，Age 键名有误
    COUT(age, 30);

    std::string name = "unknown";
    name = doc / "name" | name; // 可能多一次字符串拷贝
    COUT(name, "Alice");
    name |= doc / "name";
    COUT(name, "Alice");
    name |= doc / "Name"; // 键名有误，无影响，完全不涉及字符串的拷贝与赋值
    COUT(name, "Alice");
#endif
}

DEF_TAST(usage_2_3_3_explicit_check, "usage: 2.3.3 明确判断取值操作是否成功 >>")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    std::string name;
    if (doc / "name" >> name) {
//+     std::cout << name << std::endl; // 输出：Alice
    }
    COUT((doc / "name" >> name), true);
    COUT(name, "Alice");
#endif
}

DEF_TAST(usage_2_3_3_post_check, "usage: 2.3.3 明确判断取值是否有效 >>")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    std::string name = doc / "name" | "";
    if (name.empty()) {
        std::cout << "无效名字" << std::endl; //! 不会执行到这行
        return;
    }
    COUT(name.empty(), false);
    COUT(name, "Alice");
#endif
}

DEF_TAST(usage_2_3_4_pipe_function, "usage: 2.3.4 自定义管道函数取值 |")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    // 取值转大写
    std::string name = doc / "name" | [](yyjson::Value json) {
        std::string result = json | "";
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    };

//+ std::cout << name << std::endl; // 输出：ALICE
    COUT(name, "ALICE");
#endif
}

DEF_TAST(usage_2_3_4_pipe_function_basic, "usage: 2.3.4 管道函数基本类型参数")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    // 取值转大写的简化写法
    std::string name = doc / "name" | [](const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    };

//+ std::cout << name << std::endl; // 输出：ALICE
    COUT(name, "ALICE");
#endif
}

DEF_TAST(usage_2_3_4_pipe_function_int, "usage: 2.3.4 管道函数整数类型")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    // int age = doc / "age" | 0;
    int age = doc / "age" | [](yyjson::Value json) {
        return json.getor(0);
    };

//+ std::cout << age << std::endl; // 输出：30
    COUT(age, 30);
#endif
}

DEF_TAST(usage_2_3_4_complex_object, "usage: 2.3.4 复杂对象读取示例")
{
#ifdef MARKDOWN_CODE_SNIPPET
    struct User {
        std::string name;
        int age;
    };

    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    // 使用自定义函数
    User user = doc.root() | [](yyjson::Value json) -> User {
        User result;
        result.name = json / "name" | "";
        result.age = json / "age" | 0;
        return result;
    };

    // 等效的写法
    User user2;
    user2.name = doc / "name" | "";
    user2.age = doc / "age" | 0;
#endif
    COUT(user.name, "Alice");
    COUT(user.age, 30);
    COUT(user2.name, "Alice");
    COUT(user2.age, 30);
}

DEF_TAST(usage_2_3_5_underlying_ptr, "usage: 2.3.5 底层指针访问")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc("{}");

    yyjson_val* p  = doc.root().get();
    yyjson_val* p1 = doc.root() | (yyjson_val*)nullptr;
    yyjson_val* p2 = nullptr;
    p2 |= doc.root();
    yyjson_val* p3 = nullptr;
    if (doc.root() >> p3) {
        // 使用 yyjson C API 直接操作
    }
    COUT(p  != nullptr, true);
    COUT(p1 != nullptr, true);
    COUT(p2 != nullptr, true);
    COUT(p3 != nullptr, true);
#endif
}

DEF_TAST(usage_2_3_5_underlying_mutptr, "usage: 2.3.5 底层指针访问")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument doc("{}");
    yyjson_mut_val* p1 = nullptr;
    yyjson_mut_doc* p2 = nullptr;
    p1 |= doc.root();
    p2 |= doc.root();
    COUT(p1 != nullptr, true);
    COUT(p2 != nullptr, true);
#endif
}

DEF_TAST(usage_2_4_1_type_check, "usage: 2.4.1 Json 结点类型判断 &")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30, "score": [10, 20, 30]})";

    bool isString = doc["name"].isString();
    bool isNumber = doc["age"].isInt();
    bool isArray  = doc["score"].isArray();
    bool isObject = doc["config"].isObject();

    COUT(isString, true);
    COUT(isNumber, true);
    COUT(isArray, true);

    isString = doc / "name" & "";
    isNumber = doc / "age" & 0;
    isArray = doc / "score" & "[]";

    COUT(isString, true);
    COUT(isNumber, true);
    COUT(isArray, true);

    // 使用类型常量（推荐）
    isString = doc / "name" & kString;
    isObject = doc / "config" & kObject;
    COUT(isString, true);
    COUT(isObject, false);

    // 具名类型判断方法
    if ((doc / "age").isNumber()) {
        // 是数字类型
        COUT(true, true);
    }

    // 获取类型名称
    std::string typeName = (doc / "name").typeName();
    COUT(typeName == "string", true);
#endif
}

DEF_TAST(usage_2_4_2_type_represent, "usage: 2.4.2 类型代表值")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30, "ratio": 0.618, "score": [10, 20, 30]})";

    COUT((doc / "name" & kString), true);
    if (doc / "name" & kString) {
//+     std::cout << (doc / "name" | kString) << std::endl; // 输出：Alice
        COUT((doc / "name" | kString), "Alice");
    }
    COUT((doc / "age" & kInt), true);
    if (doc / "age" & kInt) {
//+     std::cout << (doc / "age" | kInt) << std::endl; // 输出：30
        COUT((doc / "age" | kInt), 30);
    }
    COUT((doc / "ratio" & kReal), true);
    if (doc / "ratio" & kReal) {
//+     std::cout << (doc / "ratio" | kReal) << std::endl; // 输出：0.618
        COUT((doc / "ratio" | kReal), 0.618);
    }

    COUT((doc / "score" & kArray), true);
    if (doc / "score" & kArray) {
        // kArray 与 kObject 也可用于 | 参数，返回表示数组/对象的特殊子类
//+     std::cout << (doc / "score" | kArray).toString() << std::endl;
        //^ 输出：[10,20,30]
        COUT((doc / "score" | kArray).toString(), "[10,20,30]");
    }
    COUT((doc / "name" & kString), true);
    COUT((doc / "age" & kInt), true);
    COUT((doc / "ratio" & kReal), true);
    COUT((doc / "score" & kArray), true);
#endif
}

DEF_TAST(usage_2_4_3_number_type, "usage: 2.4.3 数字类型细分")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"age": 30, "ratio": 0.618})";

    // 整数与浮点数都属于 Number
    bool isNumber = doc / "age" & kNumber; // 结果：true
    COUT(isNumber, true);
    isNumber = doc / "ratio" & kNumber;    // 结果：true
    COUT(isNumber, true);
    auto age = doc / "age" | kNumber;      // 结果: 30.0
    COUT(age, 30.0);
#endif
}

DEF_TAST(usage_2_4_3_integer_types, "usage: 2.4.3 整数类型详细区分")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"uint": 1, "sint": -1, "int": 100})";

    bool check;
    check = doc / "uint" & int(0);  // 结果: true
    COUT(check, true);
    check = doc / "uint" & uint64_t(0);  // 结果: true
    COUT(check, true);
    check = doc / "uint" &  int64_t(0);  // 结果: false
    COUT(check, false);

    check = doc / "sint" & int(0);  // 结果: true
    COUT(check, true);
    check = doc / "sint" & uint64_t(0);  // 结果: false
    COUT(check, false);
    check = doc / "sint" &  int64_t(0);  // 结果: true
    COUT(check, true);

    // 用类型常量判断可能更直观些
    check = doc / "uint" & kInt;   // 结果: true
    COUT(check, true);
    check = doc / "uint" & kUint;  // 结果: true
    COUT(check, true);
    check = doc / "uint" & kSint;  // 结果: false
    COUT(check, false);

    check = doc / "sint" & kInt;   // 结果: true
    COUT(check, true);
    check = doc / "sint" & kUint;  // 结果: false
    COUT(check, false);
    check = doc / "sint" & kSint;  // 结果: true
    COUT(check, true);

    // 取值失败，"int" 结点不是负整数
    int64_t value = 0;
    value |= doc / "int";
//+ std::cout << value << std::endl; // 输出：0
    COUT(value, 0);

    // 取值成功，字面量 `0` 属于 `int` 类型，与 json 结点匹配
//+ std::cout << (doc / "int" | 0) << std::endl; // 输出：100
    COUT((doc / "int" | 0), 100);
#endif
}

DEF_TAST(usage_2_4_4_string_type, "usage: 2.4.4 字符串类型")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    const char* pszName = doc / "name" | "";
    std::string strName = doc / "name" | ""; // 或 | std::string()
    COUT(pszName, "Alice");
    COUT(strName, "Alice");

    COUT((doc / "name" & kString), true);
    if (doc / "name" & kString)
    {
        pszName = doc / "name" | kString;
        COUT(pszName, "Alice");
    }
#endif
}

DEF_TAST(usage_2_4_5_null_type, "usage: 2.4.5 特殊 Json 类型 Null")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30, "sex": null})";

    auto sex = doc / "sex";
    if (sex) {
        bool tf1 = sex.isNull(); // 结果：true
        COUT(tf1, true);
        bool tf2 = sex & kNull;  // 结果：true
        COUT(tf2, true);
    }

    // 查找错误字段，无效 Value
    sex = doc / "Sex";
    if (!sex) {
        bool tf1 = sex.isNull(); // 结果：false
        COUT(tf1, false);
        bool tf2 = sex & kNull;  // 结果：false
        COUT(tf2, false);
    }
#endif
}

DEF_TAST(usage_2_5_1_equal_compare, "usage: 2.5.1 等值比较")
{
#ifdef MARKDOWN_CODE_SNIPPET
    std::string jsonText = R"({"name": "Alice", "age": 30})";
    yyjson::Document doc1(jsonText);
    yyjson::Document doc2(jsonText);

    if (doc1 == doc2) {
//+     std::cout << "相等" << std::endl;
    }
    COUT((doc1 == doc2), true);

    if ((doc1 / "name") == (doc2 / "name")) {
//+     std::cout << "相等" << std::endl;
    }
    COUT(((doc1 / "name") == (doc2 / "name")), true);
#endif
}

DEF_TAST(usage_2_5_1_scalar_compare, "usage: 2.5.1 标量比较")
{
#ifdef MARKDOWN_CODE_SNIPPET
    std::string jsonText = R"({"name": "Alice", "age": 30})";
    yyjson::Document doc1(jsonText);

    if ((doc1 / "name") == "Alice") {
//+     std::cout << "相等" << std::endl;
    }
    COUT(((doc1 / "name") == "Alice"), true);

    if ((doc1 / "age") == 30) {
//+     std::cout << "相等" << std::endl;
    }
    COUT(((doc1 / "age") == 30), true);
#endif
}

DEF_TAST(usage_2_6_1_to_integer, "usage: 2.6.1 JSON 转整数 +")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30, "ratio": "61.8", "score": [10, 20, 30]})";

    int result = 0;
    result = +(doc / "name");  // 结果：0
    COUT(result, 0);
    result = +(doc / "age");   // 结果：30
    COUT(result, 30);
    result = +(doc / "ratio"); // 结果：61
    COUT(result, 61);
    result = +(doc / "score"); // 结果：3
    COUT(result, 3);

    result = (doc / "score").toInteger(); // 结果：3
    COUT(result, 3);
#endif
}

DEF_TAST(usage_2_6_2_to_string, "usage: 2.6.2 JSON 转字符串 -")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    std::string result; // toString 的返回类型
    result = -(doc / "name");  // 结果：Alice
    COUT(result, "Alice");
    result = -(doc / "age");   // 结果：30
    COUT(result, "30");
    result = -doc.root();      // 结果：{"name":"Alice","age":30}
    COUT(result == "{\"name\":\"Alice\",\"age\":30}", true);

    result = (doc / "name").toString(true); // 结果："Alice"
    COUT(result, "\"Alice\"");
    result = doc.root().toString(true);
//+ std::cout << result << std::endl;
    COUT(result);
#endif
}

DEF_TAST(usage_2_6_3_doc_convert, "usage: 2.6.3 Document 只读可写互转 ~")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    yyjson::MutableDocument mutDoc = ~doc;
    auto doc2 = ~mutDoc;
    bool result = (doc2 == doc); // 结果：true
    COUT(result, true);
#endif
}

DEF_TAST(usage_2_6_4_literal_operator, "usage: 2.6.4 字符串字面量直接转 Document _xyjson")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
    auto name = doc / "name" | "";
    auto age = doc / "age" | 0;

//+ std::cout << name << "=" << age << std::endl; // 输出：Alice=30
    COUT(name, "Alice");
    COUT(age, 30);
#endif
}

DEF_TAST(usage_3_1_create_mutable, "usage: 3.1 创建与初始化可写文档")
{
#ifdef MARKDOWN_CODE_SNIPPET
    // 创建空对象文档
    yyjson::MutableDocument mutDoc("{}");

    // 创建带初始内容的文档
    yyjson::MutableDocument mutDoc2(R"({"name": "Alice", "age": 30})");

    // 创建空数组文档
    yyjson::MutableDocument mutDoc3("[]");

    COUT((bool)mutDoc, true);
    COUT((bool)mutDoc2, true);
    COUT((bool)mutDoc3, true);
#endif
}

DEF_TAST(usage_3_2_basic_assign, "usage: 3.2 赋值操作修改已有结点")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc << R"({"name": "Alice", "age": 30})";

    mutDoc / "age" = 18;
    mutDoc / "name" = "bob";
    std::string strName = mutDoc / "name" | ""; // 结果：bob
    COUT(strName, "bob");
    strName[0] = 'B';
    mutDoc / "name" = strName;
//+ std::cout << mutDoc / "name" << std::endl;  // 输出：Bob
//+ std::cout << mutDoc << std::endl;  // 输出：{"name":"Bob","age":18}
    COUT((mutDoc / "name" | ""), "Bob");
    COUT(mutDoc.root().toString() == "{\"name\":\"Bob\",\"age\":18}", true);
#endif
}

DEF_TAST(usage_3_2_1_integer_type, "usage: 3.2.1 修改整数类型")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc << R"({"name": "Alice", "age": 30})";

    auto age = mutDoc / "age";
    if(age & 0);           // kInt:  true
    if(age & uint64_t(0)); // kUint: true
    if(age & int64_t(0));  // kSint: false
    COUT((age & 0), true);
    COUT((age & uint64_t(0)), true);
    COUT((age & int64_t(0)), false);

    // 显式将 age 修改为使用 int64_t 存储整数
    age = int64_t(3000);
    if(age & 0);           // kInt:  true
    if(age & uint64_t(0)); // kUint: false
    if(age & int64_t(0));  // kSint: true
    COUT((age & 0), true);
    COUT((age & uint64_t(0)), false);
    COUT((age & int64_t(0)), true);

    // 重新序列化与反序列化
    std::string json = mutDoc.root().toString();
    yyjson::Document doc(json);
    if (doc / "age" & int64_t()); // kSint: false
    COUT(((doc / "age") & int64_t()), false);
#endif
}

DEF_TAST(usage_3_2_2_type_constant_assign, "usage: 3.2.2 使用类型常量赋值")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc << R"({"name":"Alice", "age":"30", "sex":"null", "score":100, "friend":"Bob"})";

    mutDoc / "name" = "";     // kString
    mutDoc / "age" = 0;       // kInt
    mutDoc / "sex" = nullptr; // kNull
    mutDoc / "score" = "{}";  // kObject
    mutDoc / "friend" = "[]"; // kArray

//+ std::cout << mutDoc << std::endl;
    //^ 输出：{"name":"","age":0,"sex":null,"score":{},"friend":[]}
    COUT((mutDoc / "name" | ""), "");
    COUT((mutDoc / "age" | 0), 0);
    COUT((mutDoc / "sex").isNull(), true);
    COUT((mutDoc / "score").isObject(), true);
    COUT((mutDoc / "friend").isArray(), true);
#endif
}

DEF_TAST(usage_3_2_3_value_assign, "usage: 3.2.3 Value 类型本身的赋值")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc << R"({"name": "Alice", "age": 30})";

    auto age = mutDoc / "age";
    auto age2 = age;
    age = 20;

    // 以下三行都输出：20
//+ std::cout << age << std::endl;
//+ std::cout << age2 << std::endl;
//+ std::cout << mutDoc / "age" << std::endl;
    COUT((age | 0), 20);
    COUT((age2 | 0), 20);
    COUT(((mutDoc / "age") | 0), 20);
#endif
}

DEF_TAST(usage_3_3_auto_insert, "usage: 3.3 索引操作自动插入结点 []")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;

    mutDoc["name"] = "Alice";     // 自动创建并设置 name 字段
    mutDoc["age"] = 30;           // 自动创建并设置 age 字段

    mutDoc / "name" = "Bob";      // 修改已存在的 name 字段

    //! 错误用法：尝试用 / 操作符创建新字段，没有任何效果
    mutDoc / "sex" = "female";
    if (mutDoc / "sex"); // false

    // 正确用法：先用 [] 创建，再用 / 或 [] 修改
    mutDoc["sex"] = "female";   // 创建
    mutDoc / "sex" = "Female";  // 修改
    mutDoc["sex"] = "Male";     // 修改
    if (mutDoc / "sex"); // true

    COUT((mutDoc / "name" | ""), "Bob");
    COUT((mutDoc / "age" | 0), 30);
    COUT((mutDoc / "sex" | ""), "Male");
#endif
}

DEF_TAST(usage_3_4_1_add_array, "usage: 3.4.1 给数组添加结点")
{
#ifdef MARKDOWN_CODE_SNIPPET
    MutableDocument mutDoc;

    mutDoc["num1"] = "[]";
    mutDoc["num2"] = kArray;

    mutDoc["num1"].append(1).append(2).append(3);
    mutDoc / "num2" << 1 << 2 << 3;

//+ std::cout << mutDoc / "num1" << std::endl; // 输出：[1,2,3]
//+ std::cout << mutDoc / "num2" << std::endl; // 输出：[1,2,3]

    (mutDoc["mix"] = kArray) << false << 5.5 << 666 << "[]" << "{}" << "end";
//+ std::cout << mutDoc / "mix" << std::endl; // 输出：[false,5.5,666,[],{},"end"]

    COUT((mutDoc / "num1" / 0 | 0), 1);
    COUT((mutDoc / "num1" / 2 | 0), 3);
    COUT((mutDoc / "num2" / 1 | 0), 2);
    COUT(mutDoc["num1"].toString(), "[1,2,3]");
    COUT(mutDoc["num2"].toString(), "[1,2,3]");
    COUT(mutDoc["mix"].toString(), "[false,5.5,666,[],{},\"end\"]");
#endif
}

DEF_TAST(usage_3_4_2_add_object, "usage: 3.4.2 给对象添加结点")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    auto root = *mutDoc;

    root.add("name", "Alice").add("age", 35);
    root << "sex" << "Female" << "login" << false;

    // 如果担心长链看不清键值对组合，建议一行写一对
    root << "name" << "Alice";
    root << "age" << "25";

//+ std::cout << mutDoc << std::endl;
#endif
    COUT((mutDoc / "name" | ""), "Alice");
    COUT((mutDoc / "age" | 0), 35);
    COUT((mutDoc / "sex" | ""), "Female");
    COUT(mutDoc.root().toString(), R"({"name":"Alice","age":35,"sex":"Female","login":false,"name":"Alice","age":"25"})");
}

DEF_TAST(usage_3_4_3_build_object, "usage: 3.4.3 从头构建复杂 Json")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;

    // 链式添加数组元素
    mutDoc["numbers"] = "[]"; // mutDoc.root() << "numbers" << kArray;
    mutDoc / "numbers" << 1 << 2 << 3 << 4 << 5;

    // 链式设置对象属性，键、值需成对出现
    mutDoc["config"] = "{}"; // mutDoc.root() << "config" << kObject;
    mutDoc / "config" << "timeout" << 30 << "retries" << 3 << "debug" << true;

    // 混合类型数组
    mutDoc["mixed"] = "[]"; // mutDoc.root() << "mixed" << kArray;
    mutDoc / "mixed" << 42 << "text" << 3.14 << false;

    mutDoc / "config" / "timeout" = 40;
#endif
    COUT(mutDoc.root().toString(), R"({"numbers":[1,2,3,4,5],"config":{"timeout":40,"retries":3,"debug":true},"mixed":[42,"text",3.14,false]})");
}

DEF_TAST(usage_3_4_3_build_static, "usage: 3.4.3 从头构建复杂 Json")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;

    mutDoc << R"({
        "numbers": [1, 2, 3, 4, 5],
        "config": {
            "timeout": 40,
            "retries": 3,
            "debug": true
        },
        "mixed": [42, "text", 3.14, false]
    })";
#endif
    COUT(mutDoc.root().toString(), R"({"numbers":[1,2,3,4,5],"config":{"timeout":40,"retries":3,"debug":true},"mixed":[42,"text",3.14,false]})");
}

DEF_TAST(usage_3_5_1_keyvalue_binding, "usage: 3.5.1 键值对绑定")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;

    yyjson::MutableValue name = mutDoc * "Alice"; // mutDoc.create("Alice")
    yyjson::MutableValue age = mutDoc * 25;       // mutDoc.create(25)
    auto kv = std::move(name) * std::move(age);
    //^ 等效: std::move(age).tag(std::move(name));

    auto root = *mutDoc;
    root << std::move(kv);
//+ std::cout << mutDoc << std::endl; // 输出：{"Alice":25}
    COUT((mutDoc / "Alice" | 0), 25);
    COUT(mutDoc.root().toString(), R"({"Alice":25})");

    // 这些操作可写成一行表达式，以下每一行都是等效的
    root << (mutDoc * "Alice") * (mutDoc * 25); // 键结点 * 值结点
    root << "Alice" * (mutDoc * 25); // 键名 * 值结点，括号是必须的
    root << (mutDoc * 25) * "Alice"; // 值结点 * 键名
    root << mutDoc * 25 * "Alice";   // 值结点 * 键名，省去括号
    root << mutDoc.create(25).tag("Alice"); // root.add("Alice", 25)
#endif
    COUT(mutDoc.root().toString(), R"({"Alice":25,"Alice":25,"Alice":25,"Alice":25,"Alice":25,"Alice":25})");
}

DEF_TAST(usage_3_5_2_move_node, "usage: 3.5.2 移动独立结点")
{
#ifdef MARKDOWN_CODE_SNIPPET
    // 创建根为数组的 Json
    yyjson::MutableDocument mutDoc("[]");

    // 创建独立于 Json 树根的对象，填充信息
    auto user = mutDoc * "{}";
    user << "name" << "Alice" << "age" << 30;

    // 将对象移入到 Json 根数组中
    mutDoc.root() << std::move(user);
    if (!user) {
//+     std::cout << "moved" << std::endl;
    }
    COUT((bool)user, false);

    // 创建另一个对象，可复用 user 变量名，再移入
    user = mutDoc * "{}";
    user << "name" << "Bob" << "age" << 25;
    mutDoc.root() << std::move(user);

//+ std::cout << mutDoc << std::endl;
    //^ 输出：[{"name":"Alice","age":30},{"name":"Bob","age":25}]
    COUT(mutDoc.root().toString(), R"([{"name":"Alice","age":30},{"name":"Bob","age":25}])");
    COUT((mutDoc / 0 / "name" | ""), "Alice");
    COUT((mutDoc / 1 / "name" | ""), "Bob");

    // 再创建一个数组
    auto favor = mutDoc * "[]";
    favor << "book" << "movie" << "music";

    // 将这个新数组复制两份插入到前两个（已挂到树上的） user 对象
    mutDoc / 0 << "favor" << favor;
    mutDoc / 1 << "favor" << favor;

//+ std::cout << mutDoc << std::endl;
    //^ 输出：[{"name":"Alice","age":30,"favor":["book","movie","music"]},{"name":"Bob","age":25,"favor":["book","movie","music"]}]
    COUT(mutDoc.root().toString(), R"([{"name":"Alice","age":30,"favor":["book","movie","music"]},{"name":"Bob","age":25,"favor":["book","movie","music"]}])");

    COUT(!favor, false);
    if (favor) {
//+     std::cout << favor << std::endl;
        //^ 仍有效，输出：["book","movie","music"]
        COUT(favor.toString(), R"(["book","movie","music"])");
    }
#endif
}

DEF_TAST(usage_3_6_copy_node, "usage: 3.6 拷贝已有结点")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc(R"(["Alice",25])");

    auto age = mutDoc / 1;       // 引用已有结点
    mutDoc.root() << "Bob" << age; // 拷贝原结点，不要用 std::move(age)
//+ std::cout << mutDoc << std::endl; // 输出：["Alice",25,"Bob",25]
    COUT(mutDoc);
    COUT(mutDoc.root().toString() == "[\"Alice\",25,\"Bob\",25]", true);

    // 也可以先显式用 * 或 create 创建新独立结点再移动插入
    auto newNode = mutDoc * age;
    // 第二个 << 后面直接用 mutDoc * age 代替也可以，临时值也是右值移动语义
    mutDoc.root() << "Candy" << std::move(newNode);
//+ std::cout << mutDoc << std::endl; // 输出：["Alice",25,"Bob",25,"Candy",25]
    COUT(mutDoc);
    COUT(mutDoc.root().toString(), R"(["Alice",25,"Bob",25,"Candy",25])");
    COUT((mutDoc / 4 | ""), "Candy");
#endif
}

DEF_TAST(usage_3_6_copy_document, "usage: 3.6 拷贝整个 Document")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    auto doc2 = ~doc;
    doc2 / "name" = "Bob";
    doc2 / "age" = 25;

    yyjson::MutableDocument mutDoc("[]");
    mutDoc.root() << doc << doc2; // 或 << doc.root() << doc2.root()
//+ std::cout << mutDoc << std::endl;
    //^ 输出：[{"name":"Alice","age":30},{"name":"Bob","age":25}]
    COUT(mutDoc.root().toString(), R"([{"name":"Alice","age":30},{"name":"Bob","age":25}])");
    COUT((mutDoc / 0 / "name" | ""), "Alice");
    COUT((mutDoc / 1 / "name" | ""), "Bob");
#endif
}

DEF_TAST(usage_3_6_copy_to_object, "usage: 3.6 拷贝到对象中")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    auto doc2 = ~doc;
    doc2 / "name" = "Bob";
    doc2 / "age" = 25;

    yyjson::MutableDocument mutDoc("{}");
    // 将 doc 与 doc2 复制到 mutDoc 的内存池中
    mutDoc.root() << "first" * (mutDoc * doc);
    mutDoc.root() << "second" * (mutDoc * doc2);
//+ std::cout << mutDoc << std::endl;
    //^ 输出：{"first":{"name":"Alice","age":30},"second":{"name":"Bob","age":25}}
    COUT(mutDoc.root().toString(), R"({"first":{"name":"Alice","age":30},"second":{"name":"Bob","age":25}})");
    COUT((mutDoc / "first" / "name" | ""), "Alice");
    COUT((mutDoc / "second" / "name" | ""), "Bob");
#endif
}

DEF_TAST(usage_3_6_string_ref, "usage: 3.6 字符串引用")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc << R"({"name": "Alice", "age": 30})";
    auto root = *mutDoc;

    root / "age" = "25";     // 改结点类型为字符串，但引用字面量
    std::string strName = "Alice.Green";
    root / "name" = strName; // 或 strName.c_str()，都会拷贝

    // 自动插入的键名，不能获得字面量优化
    root["friend"] = "[]"; // 特殊字面量，空数组
    // 添加数组元素，可以引用字面量
    root / "friend" << "Bob" << "Candy";

    // 插入对象的键名与字符串值，都能获得字面量优化
    root << "telephone" << "{}";
    root / "telephone" << "Home" << "1234567" << "Office" << "7654321";

//+ std::cout << mutDoc << std::endl;
    COUT_PTR((mutDoc / "age" | ""), "25");
    COUT((mutDoc / "name" | ""), "Alice.Green");
    COUT((mutDoc / "friend" | kArray).toString(), "[\"Bob\",\"Candy\"]");
    COUT_PTR((mutDoc / "telephone" / "Home" | ""), "1234567");
#endif
}

DEF_TAST(usage_3_6_string_literal_const, "usage: 3.6 字符串字面量常量")
{
#ifdef MARKDOWN_CODE_SNIPPET
#define OK "OK"
    constexpr const char* kSucc = "Succ";
    constexpr const char kFail[] = "Fail";

    yyjson::MutableDocument mutDoc("[]");
    mutDoc.root() << OK << kSucc << kFail;
//+ std::cout << mutDoc << std::endl; // 输出：["OK","Succ","Fail"]

    if (mutDoc[0] | "" == OK) {
//+     std::cout << "ref" << std::endl;
    }
    if (mutDoc[1] | "" != kSucc) {
//+     std::cout << "copy" << std::endl;
    }
    if (mutDoc[2] | "" == kFail) {
//+     std::cout << "ref" << std::endl;
    }

    COUT_PTR((mutDoc[0] | ""), "OK");
    COUT((mutDoc[1] | ""), "Succ");
    COUT((mutDoc[1] | "" != kSucc), true);
    COUT((mutDoc[2] | "") != "Fail", true);
    COUT_PTR((mutDoc[2] | ""), kFail);
#endif
}

DEF_TAST(usage_3_6_stringref, "usage: 3.6 StringRef 显式引用")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc << R"({"name": "Alice", "age": 30})";
    auto root = *mutDoc;

    std::string strName = "Alice.Green";
    auto refName = yyjson::StringRef(strName.c_str(), strName.size());
    root / "name" = refName;

    if (root / "name" | "" == strName.c_str()) {
//+     std::cout << "ref" << std::endl;
    }
    COUT((root / "name" | ""), "Alice.Green");
    COUT_PTR((root / "name" | ""), strName.c_str());
#endif
}

DEF_TAST(usage_4_2_iter_create, "usage: 4.2 迭代器创建与基本遍历")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
    auto mutDoc = ~doc;

    // 对象迭代器
    for (auto iter = doc % ""; iter; ++iter) { }
    for (auto iter = mutDoc % ""; iter; ++iter) { }

    doc << R"(["name", "Alice", "age", 30])";
    mutDoc = ~doc;

    // 数组迭代器
    for (auto iter = doc % 0; iter; ++iter) { }
    for (auto iter = mutDoc % 0; iter; ++iter) { }
#endif
    COUT(true, true);
}

DEF_TAST(usage_4_2_iter_with_startpos, "usage: 4.2 迭代器带起始位置")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name": "Alice", "age": 30})"_xyjson;

    // 对象迭代器，从第二个键值对 "age" 开始迭代
    // doc.root().iterator("age")
    for (auto iter = doc % "age"; iter; ++iter) { }

    doc << R"(["name", "Alice", "age", 30])";

    // 数组迭代器，从第三个索引开始迭代（第一个的索引是 0）
    // doc.root().iterator(2)
    for (auto iter = doc % 2; iter; ++iter) { }
#endif
    COUT(true, true);
}

DEF_TAST(usage_4_2_iter_type_constants, "usage: 4.2 使用类型常量创建迭代器")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name": "Alice", "age": 30})"_xyjson;

    // 对象迭代器 doc.root().iterator(kObject)
    for (auto iter = doc % kObject; iter; ++iter) { }

    // 数组迭代器 doc.root().iterator(kArray)
    doc << R"(["name", "Alice", "age", 30])";
    for (auto iter = doc % kArray; iter; ++iter) { }
#endif
    COUT(true, true);
}

DEF_TAST(usage_4_2_iter_begin_end, "usage: 4.2 begin/end 风格迭代器")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name": "Alice", "age": 30})"_xyjson;

    // 对象迭代器
    for (auto it = doc.root().beginObject(); it != doc.root().endObject(); ++it) { }

    // 数组迭代器
    doc << R"(["name", "Alice", "age", 30])";
    for (auto it = doc.root().beginArray(); it != doc.root().endArray(); ++it) { }
#endif
    COUT(true, true);
}

DEF_TAST(usage_4_3_iter_validity, "usage: 4.3 迭代器有效性检查")
{
#ifdef MARKDOWN_CODE_SNIPPET
    // 迭代器有效性检查示例
    yyjson::Document doc;
    doc << R"({"users": [{"name":"Alice", "age":25}, {"name":"Bob", "age":30}]})";

    // 1. 正确创建迭代器
    auto validArrayIter = doc / "users" % 0;
    if (validArrayIter) {
//+     std::cout << "OK" << std::endl;
    }
    COUT((bool)validArrayIter, true);

    // 2. 错误类型地创建迭代器
    auto invalidObjectIter = doc / "users" % "";
    if (!invalidObjectIter) {
//+     std::cout << "Error" << std::endl;
    }
    COUT((bool)invalidObjectIter, false);

    // 3. 空数组的迭代器
    yyjson::Document emptyArrayDoc("[]");
    auto emptyArrayIter = emptyArrayDoc.root() % 0;
    if (!emptyArrayIter) {
//+     std::cout << "Empty" << std::endl;
    }
    COUT((bool)emptyArrayIter, false);

    // 4. 空对象的迭代器
    yyjson::Document emptyObjectDoc("{}");
    auto emptyObjectIter = emptyObjectDoc.root() % "";
    if (!emptyObjectIter) {
//+     std::cout << "Empty" << std::endl;
    }
    COUT((bool)emptyObjectIter, false);

    // 5. 迭代器边界检查
    yyjson::Document arrayDoc("[1, 2, 3]");
    auto iter = arrayDoc.root() % 0;
    for (int i = 0; iter; ++iter, ++i) { }

    // 迭代器已到达末尾失效
    if (!iter) {
//+     std::cout << "End" << std::endl;
    }
    COUT((bool)iter, false);
#endif
}

DEF_TAST(usage_4_4_array_iter_deref, "usage: 4.4 数组迭代器解引用")
{
    std::stringstream out;
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"(["name", "Alice", "age", 30])";

    for (auto it = doc % 0; it; ++it) {
        if (*it & "") { // 解引用操作符 *it 优先级高，不必加括号
//+         std::cout << (*it | "") << ","; // | 优先级比 << 低，要加括号
            CODE(out << (*it | "") << ",");
        }
        else if (it->isInt()) { // 调用方法，用 -> 更方便，否则 (*it).isInt()
//+         std::cout << (*it | 0) << ",";
            CODE(out << (*it | 0) << ",");
        }
    }
//+ std::cout << std::endl; // 输出：name,Alice,age,30,
#endif
    COUT(out.str(), "name,Alice,age,30,");
}

DEF_TAST(usage_4_4_object_iter_deref, "usage: 4.4 对象迭代器解引用")
{
    std::stringstream out;
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    for (auto it = doc % ""; it; ++it) {
        if (*it & "") {
//+         std::cout << (*it | "") << ",";
            CODE(out << (*it | "") << ",");
        }
        else if (it->isInt()) {
//+         std::cout << (*it | 0) << ",";
            CODE(out << (*it | 0) << ",");
        }
    }
//+ std::cout << std::endl; // 输出：Alice,30,
    COUT(out.str(), "Alice,30,");
#endif
}

DEF_TAST(usage_4_4_iter_key_value, "usage: 4.4 迭代器键值访问")
{
    std::stringstream out;
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"name": "Alice", "age": 30})";

    for (auto it = doc % ""; it; ++it) {
        if (*it & "") {
//+         std::cout << (it.key() | "") << "," << (it.value() | "") << ",";
            CODE(out << (it.key() | "") << "," << (it.value() | "") << ",");
        }
        else if (it->isInt()) {
//+         std::cout << (it.key() | "") << "," << (it.value() | 0) << ",";
            CODE(out << (it.key() | "") << "," << (it.value() | 0) << ",");
        }
    }
//+ std::cout << std::endl; // 输出：name,Alice,age,30,
#endif
    COUT(out.str(), "name,Alice,age,30,");
}

DEF_TAST(usage_4_5_iter_move_plus, "usage: 4.5 迭代器前进操作 +")
{
    std::stringstream out;
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"([1,2,3,4,5,6])";

    // 从第二个元素开始迭代，每次进两步
    for (auto it = doc % 1; it; it +=2) {
//+     std::cout << (*it | 0) << ",";
        CODE(out << (*it | 0) << ",");
    }
//+ std::cout << std::endl; // 输出：2,4,6,
    COUT(true, true);
#endif
    COUT(out.str(), "2,4,6,");
}

DEF_TAST(usage_4_5_iter_move_plus_object, "usage: 4.5 迭代器前进操作 +")
{
    std::stringstream out;
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc;
    doc << R"({"one":1, "two":2, "three":3, "four":4, "five":5, "six":6})";

    // 从第二个元素开始迭代，每次进两步
    for (auto it = doc % "two"; it; it += 2) {
//+     std::cout << (*it | 0) << ",";
        CODE(out << (*it | 0) << ",");
    }
//+ std::cout << std::endl; // 输出：2,4,6,
    COUT(out.str(), "2,4,6,");

    CODE(out.str(""));
    // 不用循环，已知每个键名，向前搜索
    auto it = doc % "two";
//+ std::cout << (*it | 0) << ",";
    CODE(out << (*it | 0) << ",");
    it %= "four";
//+ std::cout << (*it | 0) << ",";
    CODE(out << (*it | 0) << ",");
    it %= "six";
//+ std::cout << (*it | 0) << ",";
    CODE(out << (*it | 0) << ",");
    it %= "eight";
    COUT(!it, true);
    if (!it); // 找不到键名，迭代器无效了
//+ std::cout << std::endl; // 输出：2,4,6,
    COUT(out.str(), "2,4,6,");
#endif
}

DEF_TAST(usage_4_6_object_iter_seek_traditional, "usage: 4.6 对象迭代器传统查找方式")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name":"Alice", "sex":false, "age":25, "height":163.5, "weight":53.3})"_xyjson;

    std::string name = doc / "name" | "";
    bool sex = doc / "sex" | false;
    int age = doc / "age" | 0;
    double height = doc / "height" | 0.0;
    double weight = doc / "weight" | 0.0;
#endif
    COUT(name, "Alice");
    COUT(sex, false);
    COUT(age, 25);
    COUT(height, 163.5);
    COUT(weight, 53.3);
}

DEF_TAST(usage_4_6_object_iter_seek_fast, "usage: 4.6 对象迭代器快速查找")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name":"Alice", "sex":false, "age":25, "height":163.5, "weight":53.3})"_xyjson;

    auto it = doc % "";
    std::string name = it / "name" | ""; // it.seek("name") | ""
    bool sex = it / "sex" | false;
    int age = it / "age" | 0;
    double height = it / "height" | 0.0;
    double weight = it / "weight" | 0.0; // it.seek("weight") | "
    COUT(name, "Alice");
    COUT(sex, false);
    COUT(age, 25);
    COUT(height, 163.5);
    COUT(weight, 53.3);

    // 等效于如下写法
    it = doc % "";
    name = *it | "";    ++it;
    sex = *it | false;  ++it;
    age = *it | 0;      ++it;
    height = *it | 0.0; ++it;
    weight = *it | 0.0; ++it;
    // seek 方法查找成功后，迭代器位置停在它下一步

    COUT(name, "Alice");
    COUT(sex, false);
    COUT(age, 25);
    COUT(height, 163.5);
    COUT(weight, 53.3);
#endif
}

DEF_TAST(usage_4_7_iter_modify, "usage: 4.7 可写迭代器修改结点")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc << R"({"name": "Alice", "age": 30})";

    auto iter = mutDoc % "age"; // item = mutDoc / "age"
    *iter = 25;                 // item = 25

//+ std::cout << mutDoc << std::endl; // 输出：{"name":"Alice","age":25}
#endif
    COUT((mutDoc / "age" | 0), 25);
    COUT(mutDoc.root().toString(), R"({"name":"Alice","age":25})");
}

DEF_TAST(usage_4_7_iter_modify_batch, "usage: 4.7 迭代器批量修改")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc;
    mutDoc << R"([1,2,3,4,5,6])";

    // 将每个元素增大为 2 倍
    // 路径操作 / 需要每次从头查找
    for (auto i = 0; i < +mutDoc; ++i) {
        mutDoc / i = (mutDoc / i | 0) * 2;
    }
//+ std::cout << mutDoc << std::endl; // 输出：[2,4,6,8,10,12]
    COUT(mutDoc.root().toString(), R"([2,4,6,8,10,12])");

    // 使用迭代器批量修改
    for (auto it = mutDoc % 0; it; ++it) {
        *it = (*it | 0) * 2;
    }
//+ std::cout << mutDoc << std::endl; // 输出：[4,8,12,16,20,24]
    COUT(mutDoc.root().toString(), R"([4,8,12,16,20,24])");
#endif
}

DEF_TAST(usage_4_8_iter_insert, "usage: 4.8 可写迭代器插入结点")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"([1, 3, 4, 5])"_xyjson;
    auto mutDoc = ~doc;

    // 迭代器定位到 [1] 元素 3，在其前面插入 2，迭代器指向新插入元素
    auto it = mutDoc % 1;
    it.insert(2);
//+ std::cout << (*it | 0) << std::endl; // 输出：2
//+ std::cout << mutDoc << std::endl; // 输出：[1,2,3,4,5]
    COUT((*it | 0), 2);
    COUT(mutDoc.root().toString(), "[1,2,3,4,5]");

    // 用 << 在 [3] 元素 4 前链式插入，迭代器位置不变
    it = mutDoc % 3;
//+ std::cout << (*it | 0) << std::endl; // 输出：4
    COUT((*it | 0), 4);
    it << 3.1 << 3.14 << 3.142;
//+ std::cout << (*it | 0) << std::endl; // 输出：4
    COUT((*it | 0), 4);
//+ std::cout << mutDoc << std::endl; // 输出：[1,2,3,3.1,3.14,3.142,4,5]
    COUT(mutDoc.root().toString(), "[1,2,3,3.1,3.14,3.142,4,5]");
#endif
}

DEF_TAST(usage_4_8_iter_insert_object, "usage: 4.8 对象迭代器插入")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
    auto mutDoc = ~doc;

    // 当前 it 指向 age，在其前面插入 sex
    auto it = mutDoc % "age";
    it.insert("sex", false);
//+ std::cout << -it << "=" << *it << std::endl; // 输出：sex=false
//+ std::cout << mutDoc << std::endl;
    //^ 输出：{"name":"Alice","sex":false,"age":30}
    COUT(-it, "sex");
    COUT(mutDoc.root().toString(), R"({"name":"Alice","sex":false,"age":30})");

    // 当前 it 指向 sex ，在其前面再插入 sex2:49
    it << "sex2" * (mutDoc * 49);
//+ std::cout << mutDoc << std::endl;
    //^ 输出：{"name":"Alice","sex2":49,"sex":false,"age":30}
    COUT(mutDoc.root().toString(), R"({"name":"Alice","sex2":49,"sex":false,"age":30})");

    // 当前 it 仍指向 sex ，在前面再插入一个字符串类型的 sex
    it << "sex3" << "spec";
//+ std::cout << mutDoc << std::endl;
    //^ 输出：{"name":"Alice","sex2":49,"sex3":"spec","sex":false,"age":30}
    COUT(mutDoc.root().toString(), R"({"name":"Alice","sex2":49,"sex3":"spec","sex":false,"age":30})");
    COUT((mutDoc / "sex" | true), false);
    COUT((mutDoc / "sex2" | 0), 49);
    COUT((mutDoc / "sex3" | ""), "spec");
#endif
}

DEF_TAST(usage_4_9_iter_delete, "usage: 4.9 可写迭代器删除结点")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"([1,2,3,3.1,3.14,3.142,4,5])"_xyjson;
    auto mutDoc = ~doc;

    auto it = mutDoc % 3;
    auto rm = it.remove();
//+ std::cout << (rm | 0.0) << std::endl; // 输出：3.1
    COUT((rm | 0.0), 3.1);

    yyjson::MutableValue rm1, rm2;
    it >> rm1 >> rm2;
//+ std::cout << (rm1 | 0.0) << std::endl; // 输出：3.14
//+ std::cout << (rm2 | 0.0) << std::endl; // 输出：3.142
//+ std::cout << mutDoc << std::endl; // 输出：[1,2,3,4,5]
    COUT((rm1 | 0.0), 3.14);
    COUT((rm2 | 0.0), 3.142);
    COUT(mutDoc.root().toString(), "[1,2,3,4,5]");
#endif
}

DEF_TAST(usage_4_9_iter_delete_object, "usage: 4.9 对象迭代器删除")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name":"Alice","sex2":49,"sex3":"spec","sex":false,"age":30})"_xyjson;
    auto mutDoc = ~doc;

    auto it = mutDoc % "sex2";
    auto rm2 = it.remove();

    // 在 it 处删除 sex2 后，自动移到下一个结点 sex3
    yyjson::KeyValue rm3;
    it >> rm3;

    // 将删除的 sex2 sex3 字段重新插入到末尾
    it.end(true);
    it << std::move(rm2) << std::move(rm3);

//+ std::cout << mutDoc << std::endl;
    //^ 输出：{"name":"Alice","sex":false,"age":30,"sex2":49,"sex3":"spec"}
    COUT(mutDoc.root().toString(), R"({"name":"Alice","sex":false,"age":30,"sex2":49,"sex3":"spec"})");
    COUT((mutDoc / "name" | ""), "Alice");
    COUT((mutDoc / "sex" | true), false);
    COUT((mutDoc / "age" | 0), 30);
    COUT((mutDoc / "sex2" | 0), 49);
    COUT((mutDoc / "sex3" | ""), "spec");
#endif
}

DEF_TAST(usage_4_10_standard_interface, "usage: 4.10 标准迭代器接口")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"([1, 2, 3, 4, 5])"_xyjson;

    auto array = doc.root().array(); // doc.root() | kArray;
    int sum = 0;
    for (auto item : array) {
        sum += (item | 0);
    }
//+ std::cout << sum << std::endl; // 输出：15
    COUT(sum, 15);

    // 或者直接使用标准库算法
    sum = std::accumulate(array.begin(), array.end(), 0,
        [](int acc, const yyjson::Value& val) {
        return acc + (val | 0);
    });
//+ std::cout << sum << std::endl; // 输出：15
    COUT(sum, 15);
#endif
}

DEF_TAST(usage_6_1_copy_behavior, "usage: 6.1 理解核心类的拷贝行为")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::Document doc1("{\"data\": \"value\"}");

    //! 错误：尝试拷贝 Document
    //! yyjson::Document doc2 = doc1;  // 编译错误

    // 合法：使用移动语义
    yyjson::Document doc2 = std::move(doc1);

    // 合法：使用类型转换，拷贝
    yyjson::MutableDocument mutCopy = ~doc2;
#endif
    COUT((bool)doc2, true);
    COUT((bool)mutCopy, true);
}

DEF_TAST(usage_6_1_mutable_value_ref, "usage: 6.1 MutableValue 引用行为")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
    auto mutDoc = ~doc;

    auto ageNode = mutDoc / "age";
    auto copyNode = ageNode;  // 实际复制的是对结点的引用

    ageNode = 25;
//+ std::cout << (copyNode | 0) << std::endl; // 输出：25
    COUT((copyNode | 0), 25);

    // 要真正复制结点，得调用 mutDoc 的 create 方法，或 * 操作符
    // 新结点是游离在 json 树外的独立结点，除非挂载到树上的某个容器中
    auto cloneNode = mutDoc * ageNode;
    COUT((bool)cloneNode, true);
#endif
}

DEF_TAST(usage_6_3_index_operator, "usage: 6.3 索引操作符只建议放 = 左边")
{
#ifdef MARKDOWN_CODE_SNIPPET
    yyjson::MutableDocument mutDoc("{}");

    //! 错误：用 / 操作符不会创建新字段
    //! mutDoc / "new_field" = "value";

    // 正确：用 [] 操作符创建新字段
    mutDoc["new_field"] = "value";
    mutDoc / "new_field" = "updated";  // 修改已存在字段
    COUT((mutDoc / "new_field" | ""), "updated");

    //! 错误：会自动添加 "no_field" 字段，类型为 null
    int i = mutDoc["non_field"] | 0;
    COUT(i, 0);
    COUT((mutDoc / "non_field").isNull(), true);
#endif
}

DEF_TAST(usage_6_4_iterator_safety, "usage: 6.4 避免对可写容器同时操作两个迭代器")
{
#ifdef MARKDOWN_CODE_SNIPPET
    auto doc = R"({"name": "Alice", "age": 30})"_xyjson;
    auto mutDoc = ~doc;

    // 通过一个迭代器 wit 在对象中插入了一个键
    // 另一个迭代器 rit 在读取时的结果难以预料
    auto rit = mutDoc % "";
    auto wit = mutDoc % "age";
    wit << "sex" << false;
    COUT((mutDoc / "sex" | true), false);
#endif
}

