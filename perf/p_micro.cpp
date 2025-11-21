/**
 * @file p_micro.cpp
 * @author xyjson project
 * @date 2025-11-21
 * @brief 一些简单测试
 */

#include "couttast/couttast.h"
#include "couttast/tastargv.hpp"
#include "xyjson.h"

static const char *s_pszSampleJson = "{\"arr\":[1,2,3,4,5], \"obj\":{\"k1\":1,\"k2\":2,\"k3\":3}}";

DEF_TOOL(micro_index_method, "measure time for index method")
{
    int warmup = 10;
    int repeat = 1000000;
    BIND_ARGV(warmup);
    BIND_ARGV(repeat);

    yyjson::Document doc(s_pszSampleJson);
    auto root = *doc;

    // warmup
    for(int i=0; i < warmup; i++) {
        volatile auto tmp1 = root.index("arr").index(1);
        (void)tmp1;
        volatile auto tmp2 = root.index("obj").index("k2");
        (void)tmp2;
    }

    TIME_TIC;
    for(int i=0; i< repeat; i++){
        volatile auto tmp1 = root.index("arr").index(1);
        (void)tmp1;
        volatile auto tmp2 = root.index("obj").index("k2");
        (void)tmp2;
    }
    TIME_TOC;
}

// pathto will call index condionally
DEF_TOOL(micro_path_method, "measure time for path method")
{
    int warmup = 10;
    int repeat = 2000000;
    BIND_ARGV(warmup);
    BIND_ARGV(repeat);

    yyjson::Document doc(s_pszSampleJson);
    auto root = *doc;

    // warmup
    for(int i=0; i < warmup; i++) {
        volatile auto tmp1 = root.pathto("arr").pathto(1);
        (void)tmp1;
        volatile auto tmp2 = root.pathto("obj").pathto("k2");
        (void)tmp2;
    }

    TIME_TIC;
    for(int i=0; i< repeat; i++){
        volatile auto tmp1 = root.pathto("arr").pathto(1);
        (void)tmp1;
        volatile auto tmp2 = root.pathto("obj").pathto("k2");
        (void)tmp2;
    }
    TIME_TOC;
}
