#!/usr/bin/perl
# tsv2mdtable.pl - 将制表符分隔的文本转 markdown 格式表格
#
# 用法:
#   tsv2mdtable.pl [--head=header1,header2,...] [file1 file2 ...]
#
# 参数:
#   --head=header1,header2,...  指定表头行，多个表头用逗号分隔
#                               如果未指定，默认使用输入的第一行作为表头
#   file1 file2 ...             输入文件名，如果省略则从标准输入读取
#
# 示例:
#   ./build/utxyjson --List | tsv2mdtable.pl --head=序号,测试用例名称,文件,行号,描述
#   cat data.tsv | tsv2mdtable.pl --head=Name,Age,City > output.md

use strict;
use warnings;

# 处理命令行参数，提取选项，保留位置参数
my $head;
my $help = 0;

while (@ARGV) {
    if ($ARGV[0] eq '--help' || $ARGV[0] eq '-h') {
        $help = 1;
        shift @ARGV;
    } elsif ($ARGV[0] =~ /^--head=(.+)$/) {
        $head = $1;
        shift @ARGV;
    } else {
        # 不是选项参数，停止处理，剩下的都是文件名参数
        last;
    }
}

if ($help) {
    print <<'HELP';
tsv2mdtable.pl - 将制表符分隔的文本转 markdown 格式表格

用法:
  tsv2mdtable.pl [--head=header1,header2,...] [file1 file2 ...]

参数:
  --head=header1,header2,...  指定表头行，多个表头用逗号分隔
                              如果未指定，默认使用输入的第一行作为表头
  file1 file2 ...             输入文件名，如果省略则从标准输入读取

示例:
  ./build/utxyjson --List | tsv2mdtable.pl --head=序号,测试用例名称,文件,行号,描述
  cat data.tsv | tsv2mdtable.pl --head=Name,Age,City > output.md
  tsv2mdtable.pl data.tsv > output.md

更多信息请查看脚本开头的详细注释。
HELP
    exit 0;
}

# 从文件或标准输入读取数据
my @lines;
while (my $line = <>) {
    chomp $line;
    push @lines, $line;
}

# 解析表头
my @headers;
if (defined $head) {
    @headers = split(/,/, $head);
} else {
    if (@lines) {
        @headers = split(/\t/, $lines[0]);
        shift @lines; # 移除表头行
    }
}

if (!@headers) {
    print STDERR "警告: 未找到表头数据\n";
    exit 0;
}

# 生成 markdown 表格
my $output = "";

# 生成表头行
$output .= "| " . join(" | ", @headers) . " |\n";
# 生成分隔行
$output .= "|" . ("------|" x @headers) . "\n";

# 生成数据行
my $row_num = 0;
foreach my $line (@lines) {
    next if $line =~ /^\s*$/; # 跳过空行

    my @cols = split(/\t/, $line);
    next unless @cols; # 跳过空行

    # 如果列数不匹配，用空值填充
    while (@cols < @headers) {
        push @cols, "";
    }

    # 转义管道符和特殊字符
    for my $col (@cols) {
        $col = "" unless defined $col;
        $col =~ s/\|/\\|/g;  # 转义管道符
        $col =~ s/\r//g;     # 移除回车符
    }

    $output .= "| " . join(" | ", @cols) . " |\n";
    $row_num++;
}

# 输出结果
print $output;

# 输出统计信息到标准错误
print STDERR "信息: 已处理 " . scalar(@lines) . " 行数据，生成 " . $row_num . " 行表格\n" if $row_num > 0;

1;
