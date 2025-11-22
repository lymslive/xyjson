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

struct MeasureRead
{
    int size = 10;   // size of sample data
    int warm = 10;   // pre-loop times
    int loop = 1000; // loop times
    int loopN = 0;   // loop * 10^n

    yyjson::Document doc;
    yyjson::Value root;
    MeasureRead()
    {
        doc << s_pszSampleJson;
        root = *doc;
        BIND_ARGV(warm);
        BIND_ARGV(loop);
        BIND_ARGV(loopN);
    }

    size_t getLoop()
    {
        size_t n = loop;
        for (int i = 0; i < loopN; ++i)
        {
            n = n * 10;
        }
        return n;
    }
};

DEF_TOOL(micro_index_method, "measure time for index method")
{
    MeasureRead self;
    size_t loop = self.getLoop();

    // warmup
    for(int i=0; i < self.warm; i++) {
        volatile auto tmp1 = self.root.index("arr").index(1);
        (void)tmp1;
        volatile auto tmp2 = self.root.index("obj").index("k2");
        (void)tmp2;
    }

    TIME_TIC;
    for(int i=0; i< loop; i++){
        volatile auto tmp1 = self.root.index("arr").index(1);
        (void)tmp1;
        volatile auto tmp2 = self.root.index("obj").index("k2");
        (void)tmp2;
    }
    TIME_TOC;
}

// pathto will call index condionally
DEF_TOOL(micro_path_method, "measure time for path method")
{
    MeasureRead self;
    size_t loop = self.getLoop();

    // warmup
    for(int i=0; i < self.warm; i++) {
        volatile auto tmp1 = self.root.pathto("arr").pathto(1);
        (void)tmp1;
        volatile auto tmp2 = self.root.pathto("obj").pathto("k2");
        (void)tmp2;
    }

    TIME_TIC;
    for(int i=0; i< loop; i++){
        volatile auto tmp1 = self.root.pathto("arr").pathto(1);
        (void)tmp1;
        volatile auto tmp2 = self.root.pathto("obj").pathto("k2");
        (void)tmp2;
    }
    TIME_TOC;
}

DEF_TOOL(micro_getor, "measure time for getor method")
{
    MeasureRead self;
    size_t loop = self.getLoop();

    yyjson::Value v1 = self.doc / "arr" / 1;
    yyjson::Value v2 = self.doc / "obj" / "k2";
    COUTF(v1 | 0, 2);
    COUTF(v2 | 0, 2);

    // warmup
    for(int i=0; i < self.warm; i++) {
        volatile auto tmp1 = v1 | 0;
        (void)tmp1;
        volatile auto tmp2 = v2 | 0;
        (void)tmp2;
    }

    TIME_TIC;
    for(int i=0; i< loop; i++){
        volatile auto tmp1 = v1 | 0;
        (void)tmp1;
        volatile auto tmp2 = v2 | 0;
        (void)tmp2;
    }
    TIME_TOC;
}
