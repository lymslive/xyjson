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
using perf::relativePerformance;

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
    yyjson_doc* yy_doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
    yyjson_val* yy_root = yyjson_doc_get_root(yy_doc);
    
    bool passed = relativePerformance(
        "xyjson deep path",
        [&doc]() {
            int value = doc / "level1" / "level2" / "level3" / "level4" / "level5" / "value" | 0;
            COUTF(value, 42);
        },
        "yyjson deep path",
        [yy_doc]() {
            yyjson_val* root = yyjson_doc_get_root(yy_doc);
            yyjson_val* l1 = yyjson_obj_get(root, "level1");
            yyjson_val* l2 = l1 ? yyjson_obj_get(l1, "level2") : NULL;
            yyjson_val* l3 = l2 ? yyjson_obj_get(l2, "level3") : NULL;
            yyjson_val* l4 = l3 ? yyjson_obj_get(l3, "level4") : NULL;
            yyjson_val* l5 = l4 ? yyjson_obj_get(l4, "level5") : NULL;
            yyjson_val* value = l5 ? yyjson_obj_get(l5, "value") : NULL;
            int val = yyjson_is_int(value) ? yyjson_get_int(value) : 0;
            COUTF(val, 42);
        },
        10000
    );

    yyjson_doc_free(yy_doc);
    COUT(passed, true);
}

DEF_TAST(chained_parse_and_access, "解析+访问组合对比")
{
    std::string jsonText = R"json({
        "data": {
            "value": 42
        }
    })json";
    
    bool passed = relativePerformance(
        "xyjson parse+access",
        [&jsonText]() {
            Document doc(jsonText);
            int value = doc / "data" / "value" | 0;
            COUTF(value, 42);
        },
        "yyjson parse+access",
        [&jsonText]() {
            yyjson_doc* doc = yyjson_read(jsonText.c_str(), jsonText.size(), 0);
            if (doc) {
                yyjson_val* root = yyjson_doc_get_root(doc);
                yyjson_val* data = yyjson_obj_get(root, "data");
                yyjson_val* value = data ? yyjson_obj_get(data, "value") : NULL;
                int val = yyjson_is_int(value) ? yyjson_get_int(value) : 0;
                yyjson_doc_free(doc);
                COUTF(val, 42);
            }
        },
        10000
    );
    
    COUT(passed, true);
}
