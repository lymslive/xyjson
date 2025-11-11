#!/usr/bin/env perl
# -*- coding: utf-8 -*-
#==============================================================================
# 脚本名称: sync_doc_examples.pl
# 脚本用途: 将测试用例中的示例代码反向同步到文档中
# 关联文件:
#   - utest/t_readme.cpp -> README.md
#   - utest/t_usage.cpp -> docs/usage.md
#==============================================================================

use strict;
use warnings;
use utf8;
use open ':std', ':encoding(UTF-8)';

#==============================================================================
# 配置：测试文件与文档文件的映射
# 格式：测试文件路径 => 文档文件路径
#==============================================================================
my %FILE_MAPPING = (
    'utest/t_readme.cpp' => 'README.md',
    'utest/t_usage.cpp'  => 'docs/usage.md',
    # 可以在这里添加更多映射
    # 'utest/t_other.cpp' => 'docs/other.md',
);

#==============================================================================
# 解析测试文件，提取用例
#==============================================================================
sub parse_test_file {
    my ($file) = @_;

    my %test_cases = ();

    open my $fh, '<', $file or die "无法打开文件: $file: $!";
    my @lines = <$fh>;
    close $fh;

    my $current_test = undef;
    my $in_snippet = 0;
    my @snippet_lines = ();

    foreach my $line (@lines) {
        # 检测 DEF_TAST 定义
        if ($line =~ /^\s*DEF_TAST\s*\(\s*(\w+)\s*,\s*"([^"]+)"\s*\)/) {
            # 保存前一个用例
            if (defined $current_test && @snippet_lines) {
                $test_cases{$current_test}{code} = [@snippet_lines];
                @snippet_lines = ();
            }
            $current_test = $1;
            $test_cases{$current_test}{desc} = $2;
            $test_cases{$current_test}{file} = $file;
            $in_snippet = 0;
        }
        # 检测进入代码片段
        elsif ($line =~ /^\s*#\s*ifdef\s+MARKDOWN_CODE_SNIPPET\s*$/) {
            $in_snippet = 1;
        }
        # 检测离开代码片段
        elsif ($line =~ /^\s*#\s*endif\s*$/) {
            if ($in_snippet) {
                $in_snippet = 0;
            }
        }
        # 收集代码片段行
        elsif ($in_snippet && $current_test) {
            push @snippet_lines, $line;
        }
    }

    # 保存最后一个用例
    if (defined $current_test && @snippet_lines) {
        $test_cases{$current_test}{code} = [@snippet_lines];
    }

    return %test_cases;
}

#==============================================================================
# 处理代码片段
#==============================================================================
sub process_code_snippet {
    my ($lines_ref) = @_;
    my @processed = ();

    foreach my $line (@$lines_ref) {
        my $processed_line = $line;

        # 删除 COUT, DESC, CODE 等宏语句（仅测试用）
        if ($processed_line =~ /^\s*(COUT|DESC|CODE|COUT_PTR)\s*\(/) {
            next;  # 跳过这行
        }

        # 转换 //+ 注释为正常语句（在文档中要取消注释）
        if ($processed_line =~ /^\/\/\+/) {
			# 顶格的 //+ 替换为空格
			$processed_line =~ s/^\/\/\+/   /;
        }
		elsif ($processed_line =~ /\/\/\+\s*/) {
			# 非顶格的 //+ 删除
			$processed_line =~ s/^\/\/\+\s*//;
		}

        # 移除前导空白（统一移除 4 个空格缩进）
        $processed_line =~ s/^\s{4}//;

        push @processed, $processed_line;
    }

    # 压缩末尾空行
    while (@processed && $processed[-1] =~ /^\s*$/) {
        pop @processed;
    }

    return @processed;
}

#==============================================================================
# 同步单个文档文件
#==============================================================================
sub sync_single_document {
    my ($doc_file, $test_cases_ref) = @_;

    open my $fh, '<', $doc_file or die "无法打开文档文件: $doc_file: $!";
    my @lines = <$fh>;
    close $fh;

    my @new_lines = ();
    my $in_example = 0;
    my $current_example_name = undef;
    my $updated_count = 0;
    my @mismatched_tests = ();

    my $i = 0;
    while ($i < @lines) {
        my $line = $lines[$i];

        # 检测示例开始标记 <!-- example:name -->
        if ($line =~ /^\s*<!--\s*example:(\S+)\s*-->/) {
            $current_example_name = $1;
            $in_example = 1;
            push @new_lines, $line;

            # 检查是否为 NO_TEST 标记
            if ($current_example_name eq 'NO_TEST') {
                $in_example = 0;
                $current_example_name = undef;
                $i++;
                next;
            }

            # 查找对应的测试用例
            if (exists $test_cases_ref->{$current_example_name}) {
                # 找到代码块开始 ```cpp
                $i++;
                if ($i < @lines && $lines[$i] =~ /^\s*```cpp\s*$/) {
                    push @new_lines, $lines[$i];  # 添加 ```cpp 行
                    $i++;

                    # 收集原有代码块内容
                    my @old_code = ();
                    my $line_idx = $i;
                    while ($line_idx < @lines && $lines[$line_idx] !~ /^\s*```\s*$/) {
                        push @old_code, $lines[$line_idx];
                        $line_idx++;
                    }

                    # 添加处理后的代码
                    my @processed_code = process_code_snippet($test_cases_ref->{$current_example_name}{code});

                    # 检查是否有实际更新
                    my $has_update = 0;
                    if (@processed_code != @old_code) {
                        $has_update = 1;
                    } else {
                        for (my $j = 0; $j < @processed_code; $j++) {
                            if ($processed_code[$j] ne $old_code[$j]) {
                                $has_update = 1;
                                last;
                            }
                        }
                    }

                    if ($has_update) {
                        $updated_count++;
                        print "  更新示例: $current_example_name\n";
                    }

                    push @new_lines, @processed_code;
                    $i = $line_idx;  # 移动到 ``` 结束标记位置

                    # 添加 ``` 结束标记
                    if ($i < @lines) {
                        push @new_lines, $lines[$i];
                    }

                    # 完成此示例的处理，重置状态
                    $in_example = 0;
                    $current_example_name = undef;
                }
            } else {
                # 测试文件中未找到对应用例，保留原有代码块不变
                push @mismatched_tests, $current_example_name;
                $i++;
                if ($i < @lines && $lines[$i] =~ /^\s*```cpp\s*$/) {
                    push @new_lines, $lines[$i];  # 添加 ```cpp 行
                    $i++;
                    # 保留原有代码块内容
                    while ($i < @lines && $lines[$i] !~ /^\s*```\s*$/) {
                        push @new_lines, $lines[$i];
                        $i++;
                    }
                    if ($i < @lines) {
                        push @new_lines, $lines[$i];  # 添加 ``` 结束标记
                    }
                    # 完成此示例的处理，重置状态
                    $in_example = 0;
                    $current_example_name = undef;
                }
            }
        }
        else {
            push @new_lines, $line;
        }

        $i++;
    }

    # 报告不匹配的示例
    if (@mismatched_tests) {
        print "  警告: 以下示例在测试文件中未找到对应用例:\n";
        foreach my $example (@mismatched_tests) {
            print "    - $example\n";
        }
    }

    return (\@new_lines, $updated_count);
}

#==============================================================================
# 同步文档（主流程）
#==============================================================================
sub sync_document {
    my ($test_file, $doc_file) = @_;

    # 解析测试文件
    my %test_cases = parse_test_file($test_file);
    print "1. 解析测试文件: $test_file\n";
    print "   发现 " . scalar(keys %test_cases) . " 个测试用例\n\n";

    # 同步文档
    print "2. 同步文档: $doc_file\n";
    my ($new_lines_ref, $updated_count) = sync_single_document($doc_file, \%test_cases);

    if ($updated_count > 0) {
        print "   共更新 $updated_count 个示例\n";

        # 备份原文件
        my $backup = "$doc_file.bak";
        system("cp", $doc_file, $backup);
        print "   原文件已备份为: $backup\n";

        # 写入新内容
        open my $fh, '>', $doc_file or die "无法写入文件: $doc_file: $!";
        print $fh @$new_lines_ref;
        close $fh;
    } else {
        print "   无更新\n";
    }

    return $updated_count;
}

#==============================================================================
# 显示帮助信息
#==============================================================================
sub show_help {
    print <<'HELP';
=== xyjson 文档示例代码反向同步工具 ===

用法:
    perl ./script/sync_doc_examples.pl [test_file]

参数:
    test_file    可选参数，指定要同步的测试文件名
                 如: utest/t_readme.cpp 或 utest/t_usage.cpp

说明:
    - 不带参数时，默认同步所有配置的文档
    - 带参数时，只同步指定的测试文件对应的文档
    - 示例代码从测试文件中提取，删除测试宏，转换注释

支持的测试文件:
HELP

    foreach my $test_file (sort keys %FILE_MAPPING) {
        my $doc_file = $FILE_MAPPING{$test_file};
        print "    $test_file -> $doc_file\n";
    }

    print "\n";
}

#==============================================================================
# 主函数
#==============================================================================
sub main {
    my $specified_test = $ARGV[0] // '';

    # 显示帮助
    if ($specified_test eq '--help' || $specified_test eq '-h') {
        show_help();
        exit(0);
    }

    print "=== xyjson 文档示例代码反向同步工具 ===\n\n";

    # 如果指定了测试文件，只处理一个
    if ($specified_test) {
        unless (exists $FILE_MAPPING{$specified_test}) {
            die "错误: 未知的测试文件: $specified_test\n请使用 --help 查看支持的测试文件\n";
        }

        my $doc_file = $FILE_MAPPING{$specified_test};

        # 检查文件存在性
        unless (-f $specified_test) {
            die "错误: 测试文件不存在: $specified_test\n";
        }
        unless (-f $doc_file) {
            die "错误: 文档文件不存在: $doc_file\n";
        }

        # 同步对应文档
        my $updated_count = sync_document($specified_test, $doc_file);

        print "\n=== 同步完成 ===\n";
        print "总更新: $updated_count 个示例\n";
    }
    # 默认处理所有文件
    else {
        my $total_updated = 0;

        # 按配置顺序处理所有映射
        foreach my $test_file (sort keys %FILE_MAPPING) {
            my $doc_file = $FILE_MAPPING{$test_file};

            # 检查文件存在性
            unless (-f $test_file) {
                print "警告: 跳过不存在的测试文件: $test_file\n";
                next;
            }
            unless (-f $doc_file) {
                print "警告: 跳过不存在的文档文件: $doc_file\n";
                next;
            }

            my $updated_count = sync_document($test_file, $doc_file);
            $total_updated += $updated_count;
            print "\n";
        }

        print "=== 同步完成 ===\n";
        print "总更新: $total_updated 个示例\n";
    }
}

# 执行主函数
main();

#==============================================================================
# 使用说明
#==============================================================================
=head1 NAME

sync_doc_examples.pl - xyjson 文档示例代码同步工具

=head1 SYNOPSIS

    perl ./script/sync_doc_examples.pl           # 同步所有文档
    perl ./script/sync_doc_examples.pl utest/t_readme.cpp  # 只同步 README.md
    perl ./script/sync_doc_examples.pl --help     # 显示帮助信息

=head1 DESCRIPTION

此脚本将 utest/ 目录下的测试用例代码反向同步到文档中。

工作原理：
1. 解析指定的测试文件中的 DEF_TAST 测试用例
2. 提取每个用例中 #ifdef MARKDOWN_CODE_SNIPPET 包围的代码块
3. 处理代码：删除 COUT/DESC/CODE 等测试宏，转换 //+ 注释为正常语句
4. 将处理后的代码同步到对应文档的示例位置

支持的功能：
- 单个文档更新：指定特定测试文件
- 批量更新：不带参数时更新所有配置的文档
- 智能检测：只更新真正有变化的示例
- 备份机制：自动备份原文档文件

文件映射在脚本开头的 %FILE_MAPPING 中配置。

=head1 AUTHOR

xyjson 项目组

=cut
