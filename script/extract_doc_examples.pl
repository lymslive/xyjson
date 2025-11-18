#!/usr/bin/env perl
# -*- coding: utf-8 -*-
#==============================================================================
# 脚本名称: extract_doc_examples.pl
# 脚本用途: 从 markdown 文档中提取示例代码到测试文件
#==============================================================================

use strict;
use warnings;
use utf8;
use open ':std', ':encoding(UTF-8)';
use Getopt::Long;

#==============================================================================
# 配置：默认值
#==============================================================================
my $TEST_FILE = '';
my $GENERATE_HEADER = 0;
my $DEBUG = 0;
my $DOC_FILE = '';  # 全局文档文件变量

#==============================================================================
# 解析命令行参数
#==============================================================================
sub parse_arguments {
    my $help = 0;
    
    my $result = GetOptions(
        'target=s' => \$TEST_FILE,
        'header'   => \$GENERATE_HEADER,
        'debug'    => \$DEBUG,
        'help'     => \$help
    );
    
    if ($help) {
        show_help();
        exit(0);
    }
    
    unless ($result) {
        die "错误: 参数解析失败\n";
    }
    
    return @ARGV;
}

#==============================================================================
# 解析文档文件，提取示例
#==============================================================================
sub parse_document_file {
    my ($doc_file) = @_;

    my @examples = ();
    my %seen_examples = ();

    my @lines = ();
    if (defined $doc_file && $doc_file ne '-') {
        open my $fh, '<', $doc_file or die "无法打开文件: $doc_file: $!";
        @lines = <$fh>;
        close $fh;
    } else {
        # 从标准输入读取
        @lines = <STDIN>;
    }

    my $current_example = undef;
    my $in_code_block = 0;
    my @code_lines = ();

    foreach my $line (@lines) {
        # 检测示例开始标记 <!-- example:name -->
        if ($line =~ /^\s*<!--\s*example:(\S+)\s*-->/) {
            warn "找到示例标记: $1\n" if $DEBUG;
            # 保存前一个示例
            if (defined $current_example && @code_lines) {
                # 检查是否重复，避免重复添加
                unless ($seen_examples{$current_example}) {
                    push @examples, {
                        name => $current_example,
                        code => [@code_lines]
                    };
                    $seen_examples{$current_example} = 1;
                    warn "保存示例: $current_example, 代码行数: " . scalar(@code_lines) . "\n" if $DEBUG;
                }
                @code_lines = ();
            }
            
            $current_example = $1;
            
            # 跳过 NO_TEST 示例
            if ($current_example eq 'NO_TEST') {
                warn "跳过 NO_TEST 示例\n" if $DEBUG;
                $current_example = undef;
            }
            
            $in_code_block = 0;
        }
        # 检测代码块开始 ```cpp
        elsif (defined $current_example && $line =~ /^\s*```cpp\s*$/) {
            warn "进入代码块\n" if $DEBUG;
            $in_code_block = 1;
        }
        # 检测代码块结束 ```
        elsif ($in_code_block && $line =~ /^\s*```\s*$/) {
            warn "退出代码块\n" if $DEBUG;
            $in_code_block = 0;
        }
        # 收集代码块内容
        elsif ($in_code_block && defined $current_example) {
            push @code_lines, $line;
        }
    }

    # 保存最后一个示例
    if (defined $current_example && @code_lines) {
        # 检查是否重复，避免重复添加
        unless ($seen_examples{$current_example}) {
            push @examples, {
                name => $current_example,
                code => [@code_lines]
            };
            $seen_examples{$current_example} = 1;
            warn "保存最后一个示例: $current_example, 代码行数: " . scalar(@code_lines) . "\n" if $DEBUG;
        }
    }
    
    warn "总共解析出 " . scalar(@examples) . " 个示例\n" if $DEBUG;

    return @examples;
}

#==============================================================================
# 处理代码片段，添加测试框架
#==============================================================================
sub process_code_for_test {
    my ($example_name, $code_lines_ref) = @_;
    my @test_code = ();
    
    # 构建描述字符串
    my $doc_file = $DOC_FILE // 'docs';
    my $description = "example from $doc_file";
    
    # 添加测试用例定义
    push @test_code, "DEF_TAST($example_name, \"$description\")\n";
    push @test_code, "{\n";
    
    # 添加 MARKDOWN_CODE_SNIPPET 标记
    push @test_code, "#ifdef MARKDOWN_CODE_SNIPPET\n";
    
    # 添加代码内容（缩进4个空格）
    foreach my $line (@$code_lines_ref) {
        push @test_code, "    $line";
    }
    
    # 结束 MARKDOWN_CODE_SNIPPET
    push @test_code, "#endif\n";
    
    push @test_code, "}\n\n";
    
    return @test_code;
}

#==============================================================================
# 生成测试文件头
#==============================================================================
sub generate_test_file_header {
    my @header = ();
    
	my $doc_file = $DOC_FILE // 'docs';
    my $doc_name = $doc_file;
    $doc_name =~ s/^docs\///;  # 去掉 docs/ 前缀
    $doc_name =~ s/\.md$//;    # 去掉 .md 后缀
    
    push @header, "/**\n";
    push @header, " * \@file t_$doc_name.cpp\n";
    push @header, " * \@author lymslive\n";
    push @header, " * \@date " . `date +"%Y-%m-%d"`;
    push @header, " * \@brief $doc_file 文档示例的单元测试\n";
    push @header, " */\n";
    push @header, "#include \"couttast/couttast.h\"\n";
    push @header, "#include \"xyjson.h\"\n";
    push @header, "#include <iostream>\n";
    push @header, "\n";
    push @header, "// use to mark code snippet from document\n";
    push @header, "#define MARKDOWN_CODE_SNIPPET\n";
    push @header, "\n";
    
    return @header;
}

#==============================================================================
# 合并到现有测试文件
#==============================================================================
sub merge_into_existing_file {
    my ($test_file, $examples_ref) = @_;
    
    open my $fh, '<', $test_file or die "无法打开文件: $test_file: $!";
    my @existing_lines = <$fh>;
    close $fh;
    
    my @new_lines = ();
    my $found_first_tast = 0;
    
    # 保留第一个 DEF_TAST 之前的内容（文件头部）
    foreach my $line (@existing_lines) {
        if ($line =~ /^\s*DEF_TAST/) {
            $found_first_tast = 1;
            last;  # 不再保留后续内容
        }
        push @new_lines, $line;
    }
    
    # 添加新的测试用例（保持原始顺序）
    foreach my $example (@$examples_ref) {
        my @test_case = process_code_for_test($example->{name}, $example->{code});
        push @new_lines, @test_case;
    }
    
    return @new_lines;
}

#==============================================================================
# 生成完整测试文件
#==============================================================================
sub generate_test_file_content {
    my ($examples_ref) = @_;
    my @test_content = ();
    
    # 添加文件头
    if ($GENERATE_HEADER) {
        my @header = generate_test_file_header();
        push @test_content, @header;
    }
    
    # 添加测试用例（保持原始顺序）
    foreach my $example (@$examples_ref) {
        my @test_case = process_code_for_test($example->{name}, $example->{code});
        push @test_content, @test_case;
    }
    
    return @test_content;
}

#==============================================================================
# 显示帮助信息
#==============================================================================
sub show_help {
    print <<'HELP';
=== xyjson 文档示例代码提取工具 ===

用法:
    perl ./script/extract_doc_examples.pl [文档文件] [选项]

参数:
    文档文件        可选，markdown 文档文件路径，或 - 表示标准输入

选项:
    --target=FILE   目标测试文件路径，如 utest/t_api.cpp
                    如果不指定，结果输出到标准输出
    --header        生成完整的测试文件头部（包括注释和头文件）
    --debug         调试模式，显示详细解析过程
    --help          显示此帮助信息

说明:
    - 从 markdown 文档中提取标记为 <!-- example:name --> 的示例代码
    - 生成对应的 DEF_TAST 测试用例
    - 跳过标记为 example:NO_TEST 的示例
    - 如果目标文件已存在，默认保留文件头部，替换测试用例主体

示例:
    # 提取 api.md 到 t_api.cpp，只生成测试用例
    perl extract_doc_examples.pl docs/api.md --target=utest/t_api.cpp
    
    # 提取 api.md 并生成完整测试文件
    perl extract_doc_examples.pl docs/api.md --target=utest/t_api.cpp --header
    
    # 从标准输入读取并输出到标准输出
    cat docs/api.md | perl extract_doc_examples.pl -

HELP
}

#==============================================================================
# 主函数
#==============================================================================
sub main {
    my @doc_files = parse_arguments();
    
    # 处理文档文件
    $DOC_FILE = $doc_files[0] // '';
    
    if ($DOC_FILE eq '' && !@doc_files) {
        show_help();
        exit(0);
    }
    
    warn "=== xyjson 文档示例代码提取工具 ===\n\n";
    
    # 解析文档文件
    my @examples = parse_document_file($DOC_FILE);
    
    warn "1. 解析文档文件: " . ($DOC_FILE eq '' ? 'stdin' : $DOC_FILE) . "\n";
    warn "   发现 " . scalar(@examples) . " 个示例\n\n";
    
    if (scalar(@examples) == 0) {
        warn "没有找到示例代码\n";
        exit(0);
    }
    
    # 列出所有示例
    warn "   示例列表:\n";
    foreach my $example (@examples) {
        warn "    - $example->{name}\n";
    }
    warn "\n";
    
    # 生成测试内容
    my @test_content = ();
    if ($TEST_FILE ne '' && -f $TEST_FILE && !$GENERATE_HEADER) {
        # 合并到现有文件
        @test_content = merge_into_existing_file($TEST_FILE, \@examples);
        warn "2. 合并到现有文件: $TEST_FILE\n";
    } else {
        # 生成完整文件
        @test_content = generate_test_file_content(\@examples);
        warn "2. 生成完整测试文件\n";
    }
    
    # 输出结果
    if ($TEST_FILE eq '') {
        # 输出到标准输出
        print @test_content;
    } else {
        # 写入文件
        open my $fh, '>', $TEST_FILE or die "无法写入文件: $TEST_FILE: $!";
        print $fh @test_content;
        close $fh;
        
        warn "   测试文件已写入: $TEST_FILE\n";
        warn "\n=== 提取完成 ===\n";
        warn "总处理: " . scalar(@examples) . " 个示例\n";
    }
}

# 执行主函数
main();

#==============================================================================
# 使用说明
#==============================================================================
=pod

=head1 NAME

extract_doc_examples.pl - xyjson 文档示例代码提取工具

=head1 SYNOPSIS

    perl ./script/extract_doc_examples.pl docs/api.md --target=utest/t_api.cpp
    perl ./script/extract_doc_examples.pl --help

=head1 DESCRIPTION

此脚本从 markdown 文档中提取示例代码到测试文件。

工作原理：
1. 解析文档中的示例标记 <!-- example:name -->
2. 提取每个示例的代码块（```cpp 包围的内容）
3. 为每个示例生成对应的 DEF_TAST 测试用例
4. 可选择输出到文件或标准输出

=head1 AUTHOR

xyjson 项目组

=cut
