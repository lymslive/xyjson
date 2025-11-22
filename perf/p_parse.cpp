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
using perf::relativePerformance;

DEF_TAST(parse_small, "解析小文件对比")
{
    std::string jsonText = readFile("perf/datasets/small.json");
    COUT(jsonText.empty() == false, true);
    
    bool passed = relativePerformance(
        "xyjson parse",
        [&jsonText]() {
            Document doc(jsonText);
            COUTF(doc.isValid(), true);
        },
        "yyjson parse",
        [&jsonText]() {
            yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
            COUTF(doc != nullptr, true);
            if (doc) {
                yyjson_doc_free(doc);
            }
        },
        10000
    );
    
    COUT(passed, true);
}

DEF_TAST(parse_medium, "解析中等文件对比")
{
    std::string jsonText = readFile("perf/datasets/medium.json");
    COUT(jsonText.empty() == false, true);
    
    bool passed = relativePerformance(
        "xyjson parse",
        [&jsonText]() {
            Document doc(jsonText);
            COUTF(doc.isValid(), true);
        },
        "yyjson parse",
        [&jsonText]() {
            yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
            COUTF(doc != nullptr, true);
            if (doc) {
                yyjson_doc_free(doc);
            }
        },
        1000
    );
    
    COUT(passed, true);
}

DEF_TAST(parse_large, "解析大文件对比")
{
    std::string jsonText = readFile("perf/datasets/large.json");
    COUT(jsonText.empty() == false, true);
    
    bool passed = relativePerformance(
        "xyjson parse",
        [&jsonText]() {
            Document doc(jsonText);
            COUTF(doc.isValid(), true);
        },
        "yyjson parse",
        [&jsonText]() {
            yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
            COUTF(doc != nullptr, true);
            if (doc) {
                yyjson_doc_free(doc);
            }
        },
        100
    );
    
    COUT(passed, true);
}

DEF_TAST(parse_file, "文件读取+解析对比")
{
    bool passed = relativePerformance(
        "xyjson file+parse",
        []() {
            std::ifstream file("perf/datasets/medium.json");
            std::stringstream buffer;
            buffer << file.rdbuf();
            Document doc(buffer.str());
            COUTF(doc.isValid(), true);
        },
        "yyjson file+parse",
        []() {
            std::ifstream file("perf/datasets/medium.json");
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string jsonText = buffer.str();

            yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
            COUTF(doc != nullptr, true);
            if (doc) {
                yyjson_doc_free(doc);
            }
        },
        1000
    );
    
    COUT(passed, true);
}

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
    
    bool passed = relativePerformance(
        "xyjson complex parse",
        [&jsonText]() {
            Document doc(jsonText);
            COUTF(doc.isValid(), true);
        },
        "yyjson complex parse",
        [&jsonText]() {
            yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
            COUTF(doc != nullptr, true);
            if (doc) {
                yyjson_doc_free(doc);
            }
        },
        10000
    );
    
    COUT(passed, true);
}
