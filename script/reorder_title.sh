#!/bin/bash
# 管道组合脚本：更新 markdown 文档标题编号

if [ $# -ne 1 ]; then
    echo "用法: $0 <markdown文件>"
    echo "示例: $0 docs/api.md"
    exit 1
fi

file_path="$1"

if [ ! -f "$file_path" ]; then
    echo "错误: 文件 '$file_path' 不存在"
    exit 1
fi

cat "$file_path" \
    | ./script/mdtitle_order.pl --toc \
    | ./script/mdtitle_update.pl --target="$file_path"

# 也可手动备份后只用 mdtitle_order.pl 重定向完成
# cp $file_path $file_path.bak
# cat $file_path.bak | mdtitle_order.pl > $file_path
