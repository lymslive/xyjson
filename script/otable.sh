#!/bin/bash
# 用管道将几个命令行工具连接起来更新 docs/api.md 文档的操作符重载表
# 在项目根目录中运行 ./script/otable.sh

cat docs/operator.csv \
    | ./script/csv2mdtable.pl \
    | ./script/update_par.pl --begin="<!-- begin operator table -->" --end="<!-- end operator table -->" --target=docs/api.md
