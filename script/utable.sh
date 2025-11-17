#!/bin/bash
# 用管道将几个命令行工具连接起来更新 utest/README.md 文档的测试用例表
# 在项目根目录中运行 ./script/utable.sh

./build/utxyjson --List \
    | cat -n \
    | awk -F'\t' '{
        # 精简行号并删除 TAST 类型列（第3列）
        # $1=行号(带空格), $2=测试名, $3=TAST(删除), $4=文件, $5=行号, $6=描述
        printf "%d\t%s\t%s\t%s\t%s\n", NR, $2, $4, $5, $6
    }' \
    | ./script/tsv2mdtable.pl --head="序号,测试用例名称,文件,行号,描述" \
    | ./script/update_par.pl --begin="<!-- UTABLE_START -->" --end="<!-- UTABLE_END -->" --target=utest/README.md
