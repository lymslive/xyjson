#!/usr/bin/perl
# 提取并编号 markdown 文档标题

use strict;
use warnings;
use Getopt::Long;

my $level = 2;     # 从几级标题开始编号，默认二级标题
my $depth = 4;     # 最多为几层标题编号
my $help = 0;

# 解析命令行参数
GetOptions(
    "level=i" => \$level,
    "depth=i" => \$depth,
    "help" => \$help
);

if ($help) {
    print_help();
    exit 0;
}

# 读取输入文件或标准输入
my @lines = <>;

# 用于存储标题编号
my @counters = (0) x 6;  # 支持最多6级标题
my $current_level = 0;

foreach my $line (@lines) {
    # 匹配标题行：以1-6个#开头，后跟空格和标题内容
    if ($line =~ /^(#{1,6})\s+(.+)$/) {
        my $hashes = $1;  # 保存 # 前缀
        my $header_level = length($hashes);
        my $title_content = $2;
        
        # 立即删除原有的编号（如果有）
        $title_content =~ s/^\d+(?:\.\d+)*\s+//;
        
        # 处理不编号的情况
        if ($level == 0) {
            print "$hashes $title_content\n";
            next;
        }
        
        # 只对指定级别及以上的标题进行编号
        if ($header_level >= $level && $header_level <= $level + $depth - 1) {
            # 重置更低级别的计数器
            if ($header_level < $current_level) {
                for (my $i = $header_level + 1; $i <= 6; $i++) {
                    $counters[$i] = 0;
                }
            }
            
            # 递增当前级别的计数器
            $counters[$header_level]++;
            $current_level = $header_level;
            
            # 生成编号
            my $number = generate_number($header_level, $level, \@counters);
            
            print "$hashes $number $title_content\n";
        } else {
            # 不编号的标题
            print "$hashes $title_content\n";
        }
    }
    # 非标题行不输出
}

sub generate_number {
    my ($current_level, $start_level, $counters_ref) = @_;
    my @counters = @$counters_ref;
    
    my @parts;
    for (my $i = $start_level; $i <= $current_level; $i++) {
        push @parts, $counters[$i];
    }
    
    return join('.', @parts);
}

sub print_help {
    print <<"HELP";
用法: mdtitle_order.pl [选项] [文件名]

提取并编号 markdown 文档标题

选项:
  --level=N    从几级标题开始编号（默认2，即##开头）
  --depth=N    最多为几层标题编号（默认4）
  --help       显示此帮助信息

示例:
  cat docs/api.md | mdtitle_order.pl
  mdtitle_order.pl --level=2 --depth=4 docs/api.md

编号格式:
  # 一级标题不编号
  ## 1 / ### 1.1 / #### 1.1.1 / ##### 1.1.1.1
  ## 2 / ### 2.1 / #### 2.1.1 / ##### 2.1.1.1

如果 --level=0 表示标题不编号，原有编号将被删除
HELP
}