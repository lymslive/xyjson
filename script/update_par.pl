#!/usr/bin/perl
# update_par.pl - 更新文档的一个段落，支持指定起始行模式
#
# 用法:
#   update_par.pl --target=file [--begin=pattern] [--end=pattern] [input_file]
#
# 参数:
#   --target=file    待更新的目标文件（必选）
#   --begin=pattern 起始行标记（支持正则表达式）
#   --end=pattern   结束行标记（支持正则表达式）
#   input_file       可选输入文件（默认从标准输入读取）
#
# 行为:
#   - 查找目标文件中匹配 --begin 和 --end 的行
#   - 将这两个标记之间的内容替换为输入的内容
#   - 起始与结束标记行保留原样不修改
#   - 如果省略 --begin 或 --end，默认使用空行作为标记
#   - 更新前先比较内容，相同则不更新
#   - 更新前先备份原文件（添加 .bak 后缀）
#
# 示例:
#   cat new_content.txt | update_par.pl --begin="<!-- UTABLE_START -->" --end="<!-- UTABLE_END -->" --target=README.md
#   cat table.md | update_par.pl --target=doc.md table.md
#   echo "new text" | update_par.pl --target=doc.txt
#   update_par.pl --target=file.txt input.txt

use strict;
use warnings;
use File::Copy;

# 处理命令行参数
my ($target_file, $begin, $end);
my $help = 0;

while (@ARGV) {
    my $arg = shift @ARGV;
    if ($arg eq '--help' || $arg eq '-h') {
        $help = 1;
        # 跳过剩余参数，直接进入帮助处理
        last;
    } elsif ($arg =~ /^--target=(.+)$/) {
        $target_file = $1;
    } elsif ($arg =~ /^--begin=(.+)$/) {
        $begin = $1;
    } elsif ($arg =~ /^--end=(.+)$/) {
        $end = $1;
    } else {
        # 不是选项参数，放回 @ARGV 并停止处理
        unshift @ARGV, $arg;
        last;
    }
}

if ($help) {
    print <<'HELP';
update_par.pl - 更新文档的一个段落，支持指定起始行模式

用法:
  update_par.pl --target=file [--begin=pattern] [--end=pattern] [input_file]

参数:
  --target=file    待更新的目标文件（必选）
  --begin=pattern  起始行标记（支持正则表达式）
  --end=pattern    结束行标记（支持正则表达式）
  input_file       可选输入文件（默认从标准输入读取）

行为:
  - 查找目标文件中匹配 --begin 和 --end 的行
  - 将这两个标记之间的内容替换为输入的内容
  - 起始与结束标记行保留原样不修改
  - 如果省略 --begin 或 --end，默认使用空行作为标记
  - 更新前先比较内容，相同则不更新
  - 更新前先备份原文件（添加 .bak 后缀）

示例:
  cat new_content.txt | update_par.pl --begin="<!-- UTABLE_START -->" --end="<!-- UTABLE_END -->" --target=README.md
  cat table.md | update_par.pl --target=doc.md table.md
  echo "new text" | update_par.pl --target=doc.txt
  update_par.pl --target=file.txt input.txt

更多信息请查看脚本开头的详细注释。
HELP
    exit 0;
}

# 检查目标文件
if (!$target_file) {
    print STDERR "错误: 未指定目标文件，请使用 --target=file 参数\n";
    print STDERR "用法: $0 --target=file [--begin=pattern] [--end=pattern] [input_file]\n";
    exit 1;
}

unless (-e $target_file) {
    print STDERR "错误: 目标文件 '$target_file' 不存在\n";
    exit 1;
}

# 读取更新内容
# 使用 <> 操作符自动处理标准输入或文件名参数
my @new_content = <>;

# 移除末尾的空行，但保留一个
while (@new_content > 1 && $new_content[-1] =~ /^\s*$/ && $new_content[-2] =~ /^\s*$/) {
    pop @new_content;
}

# 读取目标文件
open(my $fh, '<', $target_file) or die "错误: 无法打开目标文件 '$target_file': $!\n";
my @lines = <$fh>;
close($fh);

# 查找起始和结束位置
my $start_idx = -1;
my $end_idx = -1;

if ($begin) {
    # 使用正则表达式查找起始位置
    for (my $i = 0; $i < @lines; $i++) {
        if ($lines[$i] =~ /$begin/) {
            $start_idx = $i;
            last;
        }
    }
} else {
    # 查找第一个空行
    for (my $i = 0; $i < @lines; $i++) {
        if ($lines[$i] =~ /^\s*$/) {
            $start_idx = $i;
            last;
        }
    }
}

if ($end) {
    # 使用正则表达式查找结束位置
    for (my $i = $start_idx + 1; $i < @lines; $i++) {
        if ($lines[$i] =~ /$end/) {
            $end_idx = $i;
            last;
        }
    }
} else {
    # 查找第二个空行
    my $empty_count = 0;
    for (my $i = $start_idx + 1; $i < @lines; $i++) {
        if ($lines[$i] =~ /^\s*$/) {
            $empty_count++;
            if ($empty_count >= 2) {
                $end_idx = $i;
                last;
            }
        }
    }
}

# 检查是否找到标记
if ($start_idx == -1) {
    print STDERR "错误: 未找到起始标记";
    print STDERR " '$begin'" if $begin;
    print STDERR "\n";
    exit 1;
}
if ($end_idx == -1) {
    print STDERR "错误: 未找到结束标记";
    print STDERR " '$end'" if $end;
    print STDERR "\n";
    exit 1;
}

# 提取要替换的内容
my @old_content = @lines[$start_idx + 1 .. $end_idx - 1];

# 比较内容是否相同
my $need_update = 0;
if (@old_content != @new_content) {
    $need_update = 1;
} else {
    for (my $i = 0; $i < @old_content; $i++) {
        if ($old_content[$i] ne $new_content[$i]) {
            $need_update = 1;
            last;
        }
    }
}

if (!$need_update) {
    print STDERR "信息: 内容相同，无需更新 '$target_file'\n";
    exit 0;
}

# 备份原文件
my $backup_file = "$target_file.bak";
if (-e $backup_file) {
    unlink($backup_file) or print STDERR "警告: 无法删除旧备份文件 '$backup_file': $!\n";
}
if (!copy($target_file, $backup_file)) {
    print STDERR "错误: 备份文件失败: $!\n";
    exit 1;
}
print STDERR "信息: 已备份原文件到 '$backup_file'\n";

# 替换内容
my @new_lines = (
    @lines[0 .. $start_idx],
    @new_content,
    @lines[$end_idx .. $#lines]
);

# 写入新内容
open($fh, '>', $target_file) or die "错误: 无法写入目标文件 '$target_file': $!\n";
print $fh @new_lines;
close($fh);

print STDERR "信息: 已成功更新 '$target_file'\n";

1;
