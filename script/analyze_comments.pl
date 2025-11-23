#!/usr/bin/perl
#
# 分析 xyjson.h 头文件的代码、注释和空行统计
# 用法: perl script/analyze_comments.pl [filename]
#

use strict;
use warnings;

my $filename = shift || 'include/xyjson.h';

# 检查文件是否存在
unless (-e $filename) {
    die "错误: 文件 '$filename' 不存在\n";
}

# 初始化统计变量
my ($total_lines, $code_lines, $comment_lines, $blank_lines) = (0, 0, 0, 0);
my $in_block_comment = 0;

open(my $fh, '<', $filename) or die "无法打开文件 '$filename': $!\n";

while (my $line = <$fh>) {
    $total_lines++;
    
    # 去除行首尾空白
    my $trimmed = $line;
    $trimmed =~ s/^\s+|\s+$//g;
    
    # 空行判断
    if ($trimmed eq '') {
        $blank_lines++;
        next;
    }
    
    # 块注释处理
    if ($in_block_comment) {
        $comment_lines++;
        if ($trimmed =~ /\*\/\s*$/) {
            $in_block_comment = 0;
        }
        next;
    }
    
    # 块注释开始
    if ($trimmed =~ /^\/\*\*/ || $trimmed =~ /^\/\*/) {
        $comment_lines++;
        $in_block_comment = 1 unless $trimmed =~ /\*\/\s*$/;
        next;
    }
    
    # 行注释
    if ($trimmed =~ /^\/\//) {
        $comment_lines++;
        next;
    }
    
    # 预处理指令（也视为代码）
    if ($trimmed =~ /^#/) {
        $code_lines++;
        next;
    }
    
    # 其他情况视为代码行
    $code_lines++;
}

close($fh);

# 计算百分比
my $code_percent = sprintf("%.1f", ($code_lines / $total_lines) * 100);
my $comment_percent = sprintf("%.1f", ($comment_lines / $total_lines) * 100);
my $blank_percent = sprintf("%.1f", ($blank_lines / $total_lines) * 100);

# 输出统计结果
print "\n" . "=" x 60 . "\n";
print "文件: $filename 注释完善统计报告\n";
print "=" x 60 . "\n";
printf "总行数:       %5d 行\n", $total_lines;
printf "代码行数:     %5d 行 (%5.1f%%)\n", $code_lines, $code_percent;
printf "注释行数:     %5d 行 (%5.1f%%)\n", $comment_lines, $comment_percent;
printf "空行数:       %5d 行 (%5.1f%%)\n", $blank_lines, $blank_percent;
print "=" x 60 . "\n";

# 输出注释密度（注释/代码比例）
my $comment_density = sprintf("%.2f", $comment_lines / $code_lines);
printf "注释密度:     %.2f (注释行/代码行)\n", $comment_density;

# 评估注释质量
print "\n注释质量评估:\n";
if ($comment_percent >= 25) {
    print "✅ 注释覆盖率优秀 (>=25%)\n";
} elsif ($comment_percent >= 15) {
    print "✅ 注释覆盖率良好 (>=15%)\n";
} else {
    print "⚠️  注释覆盖率有待提高 (<15%)\n";
}

if ($comment_density >= 0.5) {
    print "✅ 注释密度良好 (>=0.5)\n";
} else {
    print "⚠️  注释密度需要提高 (<0.5)\n";
}

print "=" x 60 . "\n";
