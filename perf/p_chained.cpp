/**
 * @file p_chained.cpp
 * @author xyjson project
 * @date 2025-11-07
 * @brief 组合操作性能测试 - 简化版
 */

#include "couttast/couttast.h"
#include "xyjson.h"
#include <yyjson.h>
#include <string>
#include "perf_common.h"

using namespace yyjson;
using perf::measurePerformance;
using perf::printComparison;

// 测试 1: 深度链式访问对比
DEF_TAST(chained_deep_path, "深度链式访问对比")
{
    std::string jsonText = R"json({
        "level1": {
            "level2": {
                "level3": {
                    "level4": {
                        "level5": {
                            "value": 42
                        }
                    }
                }
            }
        }
    })json";

    Document doc(jsonText);

    long long xyjson_total = measurePerformance("xyjson", [&doc]() {
        int value = doc / "level1" / "level2" / "level3" / "level4" / "level5" / "value" | 0;
    }, 10000);

    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);

    long long yyjson_total = measurePerformance("yyjson", [&]() {
        yyjson_val* l1 = yyjson_obj_get(yy_root, "level1");
        yyjson_val* l2 = l1 ? yyjson_obj_get(l1, "level2") : NULL;
        yyjson_val* l3 = l2 ? yyjson_obj_get(l2, "level3") : NULL;
        yyjson_val* l4 = l3 ? yyjson_obj_get(l3, "level4") : NULL;
        yyjson_val* l5 = l4 ? yyjson_obj_get(l4, "level5") : NULL;
        yyjson_val* value = l5 ? yyjson_obj_get(l5, "value") : NULL;
        long long val = value ? yyjson_get_sint(value) : 0;
    }, 10000);

    yyjson_doc_free(yy_doc);

    printComparison("深度链式访问", "5层嵌套链式访问", xyjson_total, yyjson_total, 10000);
}

// 测试 2: 解析+访问组合对比
DEF_TAST(chained_parse_and_access, "解析+访问组合对比")
{
    std::string jsonText = R"json({
        "data": {
            "value": 42
        }
    })json";

    long long xyjson_total = measurePerformance("xyjson", [&jsonText]() {
        Document doc(jsonText);
        int value = doc / "data" / "value" | 0;
    }, 10000);

    long long yyjson_total = measurePerformance("yyjson", [&jsonText]() {
        yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
        if (doc) {
            yyjson_val* root = yyjson_doc_get_root(doc);
            yyjson_val* data = yyjson_obj_get(root, "data");
            yyjson_val* value = data ? yyjson_obj_get(data, "value") : NULL;
            long long val = value ? yyjson_get_sint(value) : 0;
            yyjson_doc_free(doc);
        }
    }, 10000);

    printComparison("解析+访问组合", "解析后立即访问", xyjson_total, yyjson_total, 10000);
}
