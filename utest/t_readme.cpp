/**
 * @file t_readme.cpp
 * @author lymslive
 * @date 2025-11-10
 * @brief README.md 文档示例的单元测试
 * */
#include "couttast/couttast.h"
#include "xyjson.h"

// use to mark code snippet from document
#define MARKDOWN_CODE_SNIPPET

// begin code in markdown file with ```cpp
#ifdef MARKDOWN_CODE_SNIPPET
using namespace yyjson;
#endif
// end code in markdown file with ```

DEF_TAST(readme_quick_start, "README: 快速开始示例 - 免安装直接使用")
{
#ifdef MARKDOWN_CODE_SNIPPET
    std::string json = R"({"name": "Alice", "age": 30})";
    Document doc(json);

    // 提取值
    std::string name = doc / "name" | ""; // 读到 "Alice"
    int age = doc / "age" | 0;            // 读到 30

    COUT(name, "Alice");
    COUT(age, 30);
#endif
}

DEF_TAST(readme_basic_ops, "README: 基本操作示例")
{
#ifdef MARKDOWN_CODE_SNIPPET
    // 从 json 串创建文档对象，也可对已有对象 doc << 输入 json 串解析
    Document doc(R"({"name": "Alice", "scores": [95, 87]})");

    // 路径访问
    std::string name = doc / "name" | ""; // "Alice"

    // 数组访问
    int firstScore = doc / "scores" / 0 | 0; // 95

    // 类型判断
    bool isString = doc / "name" & ""; // true
    bool isNumber = doc / "scores" / 0 & 0;   // true

    COUT(name, "Alice");
    COUT(firstScore, 95);
    COUT(isString, true);
    COUT(isNumber, true);
#endif
}

DEF_TAST(readme_mutable_ops, "README: 可写文档操作示例")
{
#ifdef MARKDOWN_CODE_SNIPPET
    // 创建可写文档对象，默认构建也是创建空 {} 根结点
    MutableDocument mutDoc("{}"); // 特殊字面量表示空对象

    // 添加新键不能用路径操作符 / ，索引操作 [] 支持自动添加
    mutDoc["name"] = "Bob";
    mutDoc["scores"] = "[]"; // 特殊字面量表示空数组

    // 数组追加
    mutDoc / "scores" << 95 << 87;

    // 验证结果
    COUT((mutDoc / "name" | ""), "Bob");
    COUT((mutDoc / "scores" / 0 | 0), 95);
    COUT((mutDoc / "scores" / 1 | 0), 87);
#endif
}

DEF_TAST(readme_iterator, "README: 迭代遍历示例")
{
    // 创建测试数据
    MutableDocument mutDoc("{}");
    mutDoc["items"] = "[]";
    mutDoc / "items" << "apple" << "banana" << "cherry";

    mutDoc["user"] = "{}";
    mutDoc / "user" << "name" << "Alice" << "age" << 30;

#ifdef MARKDOWN_CODE_SNIPPET
    // 数组迭代
    for (auto iter = mutDoc / "items" % 0; iter; ++iter) {
        // 验证迭代器工作
        COUT(true, true);
    }

    // 对象迭代
    for (auto iter = mutDoc / "user" % ""; iter; ++iter) {
        // 验证迭代器工作
        COUT(true, true);
    }
#endif
}
