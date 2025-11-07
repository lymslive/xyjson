/**
 * @file p_parse.cpp
 * @author xyjson project
 * @date 2025-11-07
 * @brief 解析性能测试 - 对比 xyjson 与 yyjson 原生 API
 */

#include "couttast/couttast.h"
#include "xyjson.h"
#include <yyjson.h>
#include <string>
#include "perf_common.h"

using namespace yyjson;
using perf::readFile;
using perf::measurePerformance;
using perf::printComparison;

// 测试 1: 小文件解析对比
DEF_TAST(parse_small, "解析小文件对比")
{
    std::string jsonText = readFile("perf/datasets/small.json");
    COUT(jsonText.empty() == false, true);

    long long xyjson_total = measurePerformance("xyjson", [&jsonText]() {
        Document doc(jsonText);
    }, 10000);

    long long yyjson_total = measurePerformance("yyjson", [&jsonText]() {
        yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
        if (doc) {
            yyjson_doc_free(doc);
        }
    }, 10000);

    printComparison("小文件解析", "small.json (388B)", xyjson_total, yyjson_total, 1000);
}

// 测试 2: 中等文件解析对比
DEF_TAST(parse_medium, "解析中等文件对比")
{
    std::string jsonText = readFile("perf/datasets/medium.json");
    COUT(jsonText.empty() == false, true);

    long long xyjson_total = measurePerformance("xyjson", [&jsonText]() {
        Document doc(jsonText);
    }, 1000);

    long long yyjson_total = measurePerformance("yyjson", [&jsonText]() {
        yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
        if (doc) {
            yyjson_doc_free(doc);
        }
    }, 1000);

    printComparison("中等文件解析", "medium.json (30KB)", xyjson_total, yyjson_total, 1000);
}

// 测试 3: 大文件解析对比
DEF_TAST(parse_large, "解析大文件对比")
{
    std::string jsonText = readFile("perf/datasets/large.json");
    COUT(jsonText.empty() == false, true);

    long long xyjson_total = measurePerformance("xyjson", [&jsonText]() {
        Document doc(jsonText);
    }, 100);

    long long yyjson_total = measurePerformance("yyjson", [&jsonText]() {
        yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
        if (doc) {
            yyjson_doc_free(doc);
        }
    }, 100);

    printComparison("大文件解析", "large.json (2.1MB)", xyjson_total, yyjson_total, 1000);
}

// 测试 4: 文件读取+解析对比
DEF_TAST(parse_file, "文件读取+解析对比")
{
    long long xyjson_total = measurePerformance("xyjson", []() {
        std::ifstream file("perf/datasets/medium.json");
        std::stringstream buffer;
        buffer << file.rdbuf();
        Document doc(buffer.str());
    }, 1000);

    long long yyjson_total = measurePerformance("yyjson", []() {
        std::ifstream file("perf/datasets/medium.json");
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonText = buffer.str();

        yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
        if (doc) {
            yyjson_doc_free(doc);
        }
    }, 1000);

    printComparison("文件读取+解析", "medium.json (包含文件I/O)", xyjson_total, yyjson_total, 1000);
}

// 测试 5: 复杂嵌套解析对比
DEF_TAST(parse_complex, "复杂嵌套结构解析对比")
{
    std::string jsonText = R"json({
        "level1": {
            "level2": {
                "level3": {
                    "level4": {
                        "level5": {
                            "data": [1,2,3,4,5,6,7,8,9,10],
                            "values": {
                                "a": 1, "b": 2, "c": 3, "d": 4, "e": 5
                            }
                        }
                    }
                }
            }
        },
        "array": [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15],
        "object": {
            "k1": "v1", "k2": "v2", "k3": "v3", "k4": "v4", "k5": "v5"
        }
    })json";

    long long xyjson_total = measurePerformance("xyjson", [&jsonText]() {
        Document doc(jsonText);
    }, 10000);

    long long yyjson_total = measurePerformance("yyjson", [&jsonText]() {
        yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
        if (doc) {
            yyjson_doc_free(doc);
        }
    }, 10000);

    printComparison("复杂嵌套结构", "多层嵌套+数组+对象", xyjson_total, yyjson_total, 1000);
}
