#!/usr/bin/perl
# csv2mdtable.pl - 将 CSV 格式的文本转 markdown 格式表格
#
# 用法:
#   csv2mdtable.pl [--head=header1,header2,...] [file1 file2 ...]
#
# 参数:
#   --head=header1,header2,...  指定表头行，多个表头用逗号分隔
#                               如果未指定，默认使用输入的第一行作为表头
#   file1 file2 ...             输入文件名，如果省略则从标准输入读取
#
# 注意事项:
#   - 生成的表格前后会添加空行，以确保与前后内容（如HTML注释）有清晰分隔
#   - 这对于Jekyll/kramdown正确解析表格特别重要，避免被前面的注释行干扰
#   - 空行也有助于确保后续的内容不会误认为是表格的一部分
#
# 示例:
#   cat docs/operator.csv | csv2mdtable.pl
#   cat data.csv | csv2mdtable.pl --head=Name,Age,City > output.md

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
csv2mdtable.pl - 将 CSV 格式的文本转 markdown 格式表格

用法:
  csv2mdtable.pl [--head=header1,header2,...] [file1 file2 ...]

参数:
  --head=header1,header2,...  指定表头行，多个表头用逗号分隔
                              如果未指定，默认使用输入的第一行作为表头
  file1 file2 ...             输入文件名，如果省略则从标准输入读取

示例:
  cat docs/operator.csv | csv2mdtable.pl
  cat data.csv | csv2mdtable.pl --head=Name,Age,City > output.md
  csv2mdtable.pl data.csv > output.md

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
        @headers = parse_csv_line($lines[0]);
        shift @lines; # 移除表头行
    }
}

if (!@headers) {
    print STDERR "警告: 未找到表头数据\n";
    exit 0;
}

# 生成 markdown 表格
my $output = "";

# 添加表格前的空行，确保与前面的内容（如注释）有清晰的分隔
$output .= "\n";

# 生成表头行
$output .= "| " . join(" | ", @headers) . " |\n";
# 生成分隔行
$output .= "|" . ("------|" x @headers) . "\n";

# 生成数据行
my $row_num = 0;
foreach my $line (@lines) {
    next if $line =~ /^\s*$/; # 跳过空行

    my @cols = parse_csv_line($line);
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

# 添加表格后的空行，确保与后面的内容有清晰的分隔
$output .= "\n";

# 输出结果
print $output;

# 输出统计信息到标准错误
print STDERR "信息: 已处理 " . scalar(@lines) . " 行数据，生成 " . $row_num . " 行表格\n" if $row_num > 0;

1;

# 解析 CSV 行，处理引号转义
sub parse_csv_line {
    my ($line) = @_;
    my @cols;
    my $current = "";
    my $in_quotes = 0;

    for (my $i = 0; $i < length($line); $i++) {
        my $char = substr($line, $i, 1);

        if ($char eq '"') {
            if ($in_quotes && $i + 1 < length($line) && substr($line, $i + 1, 1) eq '"') {
                # 双引号转义
                $current .= '"';
                $i++; # 跳过下一个引号
            } else {
                # 切换引号状态
                $in_quotes = !$in_quotes;
            }
        } elsif ($char eq ',' && !$in_quotes) {
            # 分隔符
            push @cols, $current;
            $current = "";
        } else {
            $current .= $char;
        }
    }
    # 添加最后一列
    push @cols, $current;

    return @cols;
}
