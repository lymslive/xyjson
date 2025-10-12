#!/usr/bin/perl
# 该脚本会自动更新 include/xyjson.h 文件中的 TOC 部分
# 用法：在项目根目录运行 `perl script/gen_toc.pl`
# 须交互式确认更新原文件，会备份原文件为 xyjson.h.bak
# 也可尝试自动更新 `echo y | perl script/gen_toc.pl`

use strict;
use warnings; 

my $filename = "include/xyjson.h";
my $output_file = "include/xyjson.h";

# 子函数：查找所有分级标题
sub find_toc_entries {
    my ($lines_ref) = @_;
    my @entries = ();
    my $line_number = 0;
    
    foreach my $line (@$lines_ref) {
        $line_number++;
        
        # 使用正则表达式匹配注释标题
        if ($line =~ m{^/\*\s*@(Part|Section|Group)\s+(\d+(?:\.\d+)*):\s*(.+?)\s*\*/}) {
            my $level = $1;
            my $number = $2;
            my $title = $3;
            
            # 清理标题两端的空格
            $title =~ s/^\s+|\s+$//g;
            
            push @entries, {
                level => $level,
                number => $number,
                title => $title,
                original_line => $line_number
            };
        }
    }
    
    return @entries;
}

# 子函数：查找TOC范围
sub find_toc_range {
    my ($lines_ref) = @_;
    my $line_number = 0;
    my $toc_start_line = -1;
    my $toc_end_line = -1;
    my $toc_sep_line = 0;
    
    foreach my $line (@$lines_ref) {
        $line_number++;
        
        # 查找TOC开始
        if ($line =~ /^\/\* Table of Content/) {
            $toc_start_line = $line_number;
        }
        
        # 查找TOC结束（第二个分隔线）
        if ($toc_start_line != -1 && $toc_end_line == -1 && $line =~ /^\/\* ={60,}\s*\*\/$/) {
            if (++$toc_sep_line == 2) {
                $toc_end_line = $line_number;
                last;
            }
        }
    }
    
    return ($toc_start_line, $toc_end_line);
}

# 子函数：查找namespace行位置
sub find_namespace_line {
    my ($lines_ref) = @_;
    my $line_number = 0;
    
    foreach my $line (@$lines_ref) {
        $line_number++;
        if ($line =~ /^namespace yyjson/) {
            return $line_number;
        }
    }
    
    return -1;
}

# 子函数：生成TOC内容
sub generate_toc_content {
    my ($entries_ref, $line_offset) = @_;
    my @toc_lines = ();
    
    push @toc_lines, "/* Table of Content (TOC) */";
    push @toc_lines, "/* ======================================================================== */";
    
    foreach my $entry (@$entries_ref) {
        my $indent = ($entry->{level} eq 'Part') ? 0 :
                     ($entry->{level} eq 'Section') ? 2 : 4;
        
        my $prefix = (' ' x $indent) . "$entry->{level} $entry->{number}: $entry->{title}";
        
        my $total_width = 72;
        my $title_width = length($prefix);
        my $adjusted_line = $entry->{original_line} + ($line_offset || 0);
        my $line_num_str = "Line: $adjusted_line";
        my $line_num_width = length($line_num_str);
        
        my $space_count = $total_width - $title_width - $line_num_width;
        
        if ($space_count < 1) {
            my $available_title_width = $total_width - $line_num_width - 3;
            if ($available_title_width > 10) {
                $prefix = substr($prefix, 0, $available_title_width) . "...";
                $space_count = 1;
            } else {
                $prefix = "$entry->{level} $entry->{number}";
                $space_count = $total_width - length($prefix) - $line_num_width;
            }
        }
        
        my $toc_line = "/* $prefix" . (' ' x $space_count) . "$line_num_str */";
        push @toc_lines, $toc_line;
    }
    
    push @toc_lines, "/* ======================================================================== */";
    
    return @toc_lines;
}

# 子函数：调整TOC行号
sub adjust_toc_line_numbers {
    my ($toc_lines_ref, $line_offset, $toc_start_line) = @_;
    
    foreach my $i (2..$#$toc_lines_ref-1) {
        if ($toc_lines_ref->[$i] =~ /Line: (\d+)/) {
            my $original_line = $1;
            my $adjusted_line = $original_line;
            
            if ($toc_start_line != -1 && $original_line > $toc_start_line) {
                $adjusted_line = $original_line + $line_offset;
            }
            
            $toc_lines_ref->[$i] =~ s/Line: \d+/Line: $adjusted_line/;
        }
    }
}

# 子函数：验证TOC一致性
sub validate_toc_consistency {
    my ($before_entries_ref, $after_entries_ref, $line_offset) = @_;
    
    print "=== TOC一致性验证 ===\n";
    
    my $before_count = scalar(@$before_entries_ref);
    my $after_count = scalar(@$after_entries_ref);
    
    if ($before_count != $after_count) {
        print "✗ 标题数量不匹配: 更新前=$before_count, 更新后=$after_count\n";
        return 0;
    }
    
    print "✓ 标题数量一致: $before_count 个\n";
    
    # 检查每个标题是否一致
    my $all_match = 1;
    for my $i (0..$#{$before_entries_ref}) {
        my $before = $before_entries_ref->[$i];
        my $after = $after_entries_ref->[$i];
        
        my $expected_line = $before->{original_line} + $line_offset;
        
        if ($before->{level} ne $after->{level} || 
            $before->{number} ne $after->{number} || 
            $before->{title} ne $after->{title}) {
            print "✗ 标题 $i 不匹配:\n";
            print "  更新前: $before->{level} $before->{number}: $before->{title} (行: $before->{original_line})\n";
            print "  更新后: $after->{level} $after->{number}: $after->{title} (行: $after->{line})\n";
            $all_match = 0;
        } elsif ($after->{original_line} != $expected_line) {
            print "✗ 标题 $i 行号错误: 期望=$expected_line, 实际=$after->{line}\n";
            $all_match = 0;
        }
    }
    
    if ($all_match) {
        print "✓ 所有标题行号正确一致\n";
    }
    
    return $all_match;
}

# 子函数：重新生成文件内容
sub regenerate_file_content {
    my ($lines_ref, $toc_lines_ref, $toc_start_line, $toc_end_line, $namespace_line) = @_;
    my @new_content = ();
    my $current_line = 0;
    
    foreach my $line (@$lines_ref) {
        $current_line++;
        
        # 如果有旧TOC，在旧位置插入；否则在namespace前插入（再加一个空行）
        if ($toc_start_line != -1) {
            if ($current_line == $toc_start_line) {
                push @new_content, $_ . "\n" foreach @$toc_lines_ref;
                next;
            }
            elsif ($current_line > $toc_start_line && $current_line <= $toc_end_line){
                next;
            }
        }
        
        if ($current_line == $namespace_line) {
            if ($toc_start_line == -1) {
                push @new_content, $_ . "\n" foreach @$toc_lines_ref;
                push @new_content, "\n";
            }
        }
        
        push @new_content, $line;
    }
    
    return @new_content;
}

# 主程序
print "=== xyjson.h TOC生成器 ===\n\n";

# 读取文件内容
open(my $fh, '<', $filename) or die "无法打开文件 $filename: $!";
my @lines = <$fh>;
close($fh);

# 查找TOC范围、分级标题和namespace位置
my ($toc_start_line, $toc_end_line) = find_toc_range(\@lines);
my @toc_entries = find_toc_entries(\@lines);
my $namespace_line = find_namespace_line(\@lines);

# 计算TOC大小和偏移量
my $new_toc_size = scalar(@toc_entries) + 3; # 三行固定文本
my $old_toc_size = 0;

if ($toc_start_line != -1 && $toc_end_line != -1) {
    $old_toc_size = $toc_end_line - $toc_start_line + 1;
    print "检测到旧TOC: 从行 $toc_start_line 到 $toc_end_line, 大小 $old_toc_size 行\n";
} else {
    print "未检测到旧TOC\n";
}

my $line_offset = $new_toc_size - $old_toc_size;

# 生成TOC内容
my @toc_lines = generate_toc_content(\@toc_entries, 0);

# 调整行号
adjust_toc_line_numbers(\@toc_lines, $line_offset, $toc_start_line);

print "\nTOC统计信息:\n";
print "- 新TOC大小: $new_toc_size 行\n";
print "- 旧TOC大小: $old_toc_size 行\n";
print "- 行号偏移量: $line_offset\n";
print "- namespace位置: $namespace_line\n";
print "- 找到标题: " . scalar(@toc_entries) . " 个\n";

# 显示新TOC预览
print "\n新TOC预览 (前5行):\n";
for my $i (0..4) {
    print "$toc_lines[$i]\n" if $i < @toc_lines;
}
if (@toc_lines > 5) {
    print "... (共 $new_toc_size 行)\n";
}

# 询问是否更新
print "\n是否要将TOC更新到文件 $output_file? (y/N): ";
my $answer = <STDIN>;
chomp($answer) if defined $answer;

if ($answer =~ /^y/i) {
    # 备份原文件
    my $backup_file = "$output_file.bak";
    open(my $src, '<', $output_file) or die "无法打开文件 $output_file: $!";
    open(my $dst, '>', $backup_file) or die "无法创建备份文件 $backup_file: $!";
    print $dst $_ while <$src>;
    close($src);
    close($dst);
    
    # 重新生成文件内容
    my @new_content = regenerate_file_content(\@lines, \@toc_lines, $toc_start_line, $toc_end_line, $namespace_line);
    
    # 写入新文件
    open(my $out, '>', $output_file) or die "无法写入文件 $output_file: $!";
    print $out $_ foreach @new_content;
    close($out);
    
    print "\nTOC已成功更新到 $output_file\n";
    print "原文件已备份为 $backup_file\n";
    
    # 验证更新后的TOC
    print "\n=== 验证更新结果 ===\n";
    
    open(my $verify_fh, '<', $output_file) or die "无法验证文件: $!";
    my @new_lines = <$verify_fh>;
    close($verify_fh);
    
    my @after_entries = find_toc_entries(\@new_lines);
    
    if (validate_toc_consistency(\@toc_entries, \@after_entries, $line_offset)) {
        print "\n✅ TOC更新验证成功！\n";
    } else {
        print "\n⚠️ TOC更新存在不一致，请检查\n";
    }
    
    # 显示标题统计
    print "\n标题统计:\n";
    print "- Part: " . scalar(grep { $_->{level} eq 'Part' } @after_entries) . " 个\n";
    print "- Section: " . scalar(grep { $_->{level} eq 'Section' } @after_entries) . " 个\n";
    print "- Group: " . scalar(grep { $_->{level} eq 'Group' } @after_entries) . " 个\n";
} else {
    print "\n未更新文件。\n";
}

print "\n=== 完成 ===\n";

1;