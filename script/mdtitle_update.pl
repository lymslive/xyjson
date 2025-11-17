#!/usr/bin/perl
# 更新 markdown 文档标题

use strict;
use warnings;
use Getopt::Long;

my $target_file = '';
my $help = 0;

# 解析命令行参数
GetOptions(
    "target=s" => \$target_file,
    "help" => \$help
);

if ($help) {
    print_help();
    exit 0;
}

unless ($target_file) {
    die "错误: 必须指定 --target 参数\n";
}

# 检查目标文件是否存在
unless (-e $target_file) {
    die "错误: 目标文件 '$target_file' 不存在\n";
}

# 读取输入的标题行（来自管道）
my @title_lines = <>;

# 读取目标文件
open(my $target_fh, '<', $target_file) or die "无法打开文件 '$target_file': $!\n";
my @target_lines = <$target_fh>;
close($target_fh);

# 逐行比对并更新标题
my @updated_lines = merge_titles(\@target_lines, \@title_lines);

# 检查是否有实际改动
if (arrays_equal(\@target_lines, \@updated_lines)) {
    print "没有检测到改动，文件保持不变\n";
    exit 0;
}

# 备份原文件
my $backup_file = "$target_file.bak";
copy_file($target_file, $backup_file);

# 写入更新后的文件
open(my $out_fh, '>', $target_file) or die "无法写入文件 '$target_file': $!\n";
print $out_fh @updated_lines;
close($out_fh);

print "文件 '$target_file' 已更新，原文件备份为 '$backup_file'\n";

sub merge_titles {
    my ($target_lines_ref, $title_lines_ref) = @_;
    my @target_lines = @$target_lines_ref;
    my @title_lines = @$title_lines_ref;
    
    my @updated_lines;
    my $title_index = 0;
    
    foreach my $target_line (@target_lines) {
        # 如果是标题行
        if ($target_line =~ /^(#{1,6})\s+(.+)$/) {
            # 如果还有新的标题行可用
            if ($title_index < scalar @title_lines) {
                my $title_line = $title_lines[$title_index];
                
                # 检查是否是标题行
                if ($title_line =~ /^(#{1,6})\s+(.+)$/) {
                    # 使用新的标题行
                    push @updated_lines, $title_line;
                    $title_index++;
                } else {
                    # 新标题行不是标题，保持原目标行
                    push @updated_lines, $target_line;
                }
            } else {
                # 没有新的标题行，保持原目标行
                push @updated_lines, $target_line;
            }
        } else {
            # 非标题行，直接保持
            push @updated_lines, $target_line;
        }
    }
    
    # 如果还有未使用的标题行，添加到文件末尾
    while ($title_index < scalar @title_lines) {
        my $title_line = $title_lines[$title_index];
        if ($title_line =~ /^(#{1,6})\s+(.+)$/) {
            push @updated_lines, "\n" if @updated_lines > 0;
            push @updated_lines, $title_line;
        }
        $title_index++;
    }
    
    return @updated_lines;
}

sub arrays_equal {
    my ($arr1_ref, $arr2_ref) = @_;
    my @arr1 = @$arr1_ref;
    my @arr2 = @$arr2_ref;
    
    return 0 if scalar @arr1 != scalar @arr2;
    
    for (my $i = 0; $i < @arr1; $i++) {
        return 0 if $arr1[$i] ne $arr2[$i];
    }
    
    return 1;
}

sub copy_file {
    my ($source, $dest) = @_;
    open(my $in, '<', $source) or die "无法读取文件 '$source': $!\n";
    open(my $out, '>', $dest) or die "无法写入文件 '$dest': $!\n";
    while (my $line = <$in>) {
        print $out $line;
    }
    close($in);
    close($out);
}

sub print_help {
    print <<"HELP";
用法: mdtitle_update.pl [选项] [标题输入]

更新 markdown 文档的标题

选项:
  --target=文件  指定要更新的目标文件名
  --help         显示此帮助信息

示例:
  cat titles.txt | mdtitle_update.pl --target=docs/api.md
  mdtitle_update.pl --target=docs/api.md < titles.txt

功能说明:
- 从标准输入读取标题行（通常是 mdtitle_order.pl 的输出）
- 逐行比对目标文档和标题行
- 保持目标文档的非标题行原样
- 按照顺序更新对应的标题行
- 如果标题行比目标文档多，在末尾添加新标题
- 更新前会检测是否实际有改动
- 如果有改动，会先备份原文件
HELP
}