#!/usr/bin/perl

use strict;
use warnings;

# 脚本功能：从 utxyjson --List 输出生成 markdown 表格
# 用法：perl script/utable.pl [--output README.md]

my $readme_file = "utest/README.md";
my $test_binary = "./build/utxyjson";

# 处理命令行参数
for (my $i = 0; $i < @ARGV; $i++) {
    if ($ARGV[$i] eq "--output" && $i+1 < @ARGV) {
        $readme_file = $ARGV[++$i];
    } elsif ($ARGV[$i] eq "--binary" && $i+1 < @ARGV) {
        $test_binary = $ARGV[++$i];
    } elsif ($ARGV[$i] eq "--help") {
        print "Usage: $0 [--output README.md] [--binary ./build/utxyjson]\n";
        print "       --output=- 表示输出到标准输出\n";
        exit(0);
    }
}

# 检查测试二进制文件是否存在
unless (-e $test_binary) {
    die "错误: 测试二进制文件 '$test_binary' 不存在。请先运行 'make build' 构建项目。\n";
}

# 运行 utxyjson --List 获取测试用例列表
my @lines = `$test_binary --List 2>&1`;
if ($? != 0) {
    die "错误: 运行 '$test_binary --List' 失败。请确保测试程序已正确构建。\n";
}

# 解析测试用例数据
my @test_cases;
foreach my $line (@lines) {
    chomp $line;
    # 格式: 用例名\t类型\t文件名\t行号\t描述
    my ($name, $type, $file, $line, $desc) = split(/\t/, $line);
    
    # 跳过空行和标题行
    next if !$name || $name =~ /^\s*$/ || $type ne 'TAST';
    
    push @test_cases, {
        name => $name,
        file => $file,
        line => $line,
        desc => $desc
    };
}

# 生成 markdown 表格
my $table = generate_markdown_table(\@test_cases);

# 更新 README.md 文件或输出到标准输出
if ($readme_file eq "-") {
    print $table, "\n";
    print "测试用例表已输出到标准输出\n";
} else {
    update_readme($readme_file, $table);
    print "测试用例表已成功更新到 $readme_file\n";
}

sub generate_markdown_table {
    my ($test_cases) = @_;
    
    my $output = "| 序号 | 测试用例名称 | 文件 | 行号 | 描述 |\n";
    $output .= "|------|-------------|------|------|------|\n";
    
    my $counter = 1;
    foreach my $case (@$test_cases) {
        $output .= sprintf("| %-4d | `%s` | `%s` | %-4s | %s |\n",
            $counter++,
            $case->{name},
            $case->{file},
            $case->{line},
            $case->{desc}
        );
    }
    
    return $output;
}

sub update_readme {
    my ($filename, $new_table) = @_;
    
    # 读取原文件内容
    open(my $fh, '<', $filename) or die "无法打开文件 $filename: $!";
    my @lines = <$fh>;
    close($fh);
    
    # 查找表格的开始和结束标记
    my $start_marker = "<!-- UTABLE_START -->";
    my $end_marker = "<!-- UTABLE_END -->";
    
    my $start_idx = -1;
    my $end_idx = -1;
    
    for (my $i = 0; $i < @lines; $i++) {
        if ($lines[$i] =~ /\Q$start_marker\E/) {
            $start_idx = $i;
        }
        if ($lines[$i] =~ /\Q$end_marker\E/) {
            $end_idx = $i;
            last;
        }
    }
    
    # 如果没有找到标记，则寻找原有的表格位置
    if ($start_idx == -1 || $end_idx == -1) {
        # 查找原有的表格位置
        for (my $i = 0; $i < @lines; $i++) {
            if ($lines[$i] =~ /^\|.*测试用例名称.*文件.*行号.*描述/) {
                $start_idx = $i - 1; # 标题前一行
                # 寻找表格结束位置
                for (my $j = $i; $j < @lines; $j++) {
                    if ($lines[$j] !~ /^\|/ && $lines[$j] !~ /^[-|\s]*$/) {
                        $end_idx = $j - 1;
                        last;
                    }
                }
                last;
            }
        }
    }
    
    # 如果仍然找不到合适的位置，则在文件末尾添加
    if ($start_idx == -1 || $end_idx == -1) {
        $start_idx = @lines;
        $end_idx = @lines;
        push @lines, "\n## 测试用例一览表\n\n";
    }
    
    # 替换表格内容
    splice @lines, $start_idx, $end_idx - $start_idx + 1;
    
    # 将表格内容按行分割并正确插入
    my @table_lines = split(/\r?\n/, $new_table);
    splice @lines, $start_idx, 0, (
        "$start_marker\n",
        (map { "$_\n" } @table_lines),
        "$end_marker\n"
    );
    
    # 写回文件
    open($fh, '>', $filename) or die "无法写入文件 $filename: $!";
    print $fh @lines;
    close($fh);
}

1;