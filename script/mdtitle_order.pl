#!/usr/bin/perl
# 提取并编号 markdown 文档标题

use strict;
use warnings;
use Getopt::Long;

my $level = 2;     # 从几级标题开始编号，默认二级标题
my $depth = 4;     # 最多为几层标题编号
my $help = 0;
my $toc;           # --toc 选项：undef=输出完整文件，0=只输出标题行，-1=行号格式，n>0=对齐格式

# 解析命令行参数
GetOptions(
    "level=i" => \$level,
    "depth=i" => \$depth,
    "toc:i" => \$toc,
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

# 主循环，处理每一行
my $line_num = 0;
foreach my $line (@lines) {
    $line_num++;
    
    # 匹配标题行：以1-6个#开头，后跟空格和标题内容
    if ($line =~ /^(#{1,6})\s+(.+)$/) {
        my $hashes = $1;  # 保存 # 前缀
        my $header_level = length($hashes);
        my $title_content = $2;
        
        # 立即删除原有的编号（如果有）
        $title_content =~ s/^\d+(?:\.\d+)*\s+//;
        
        # 处理标题行
        process_title_line($hashes, $title_content, $header_level, $line_num);
    } else {
        # 非标题行：只在完整文件模式下输出
        if (!defined $toc) {
            print $line;
        }
    }
}

# 处理标题行
sub process_title_line {
    my ($hashes, $title_content, $header_level, $line_num) = @_;
    
    # 处理不编号的情况
    if ($level == 0) {
        print_title_output($hashes, "", $title_content, $line_num);
        return;
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
        
        print_title_output($hashes, $number, $title_content, $line_num);
    } else {
        # 不编号的标题
        print_title_output($hashes, "", $title_content, $line_num);
    }
}

# 生成标题编号
sub generate_number {
    my ($current_level, $start_level, $counters_ref) = @_;
    my @counters = @$counters_ref;
    
    my @parts;
    for (my $i = $start_level; $i <= $current_level; $i++) {
        push @parts, $counters[$i];
    }
    
    return join('.', @parts);
}

# 根据 toc 参数格式化输出标题行
sub print_title_output {
    my ($hashes, $number, $title_content, $line_num) = @_;
    
    # 如果没有 --toc 选项，按完整文件模式输出
    if (!defined $toc) {
        print "$hashes ";
        print "$number " if $number;
        print "$title_content";
        return;
    }
    
    # 根据 toc 参数处理不同格式
    if ($toc == -1) {
        # --toc=-1: 行号\t标题
        print "$line_num\t$hashes ";
        print "$number " if $number;
        print "$title_content\n";
    } elsif ($toc > 0) {
        # --toc=n (n>0): 标题 行号（对齐到第n列）
        my $title_line = "$hashes ";
        $title_line .= "$number " if $number;
        $title_line .= "$title_content";
        
        # 计算显示宽度（考虑中文）
        my $display_width = calculate_display_width($title_line);
        
        # 如果标题行长度小于指定列，填充空格
        if ($display_width < $toc) {
            my $spaces = " " x ($toc - $display_width);
            print "$title_line$spaces$line_num\n";
        } else {
            # 如果标题行太长，只加一个空格
            print "$title_line $line_num\n";
        }
    } else {
        # --toc 或 --toc=0: 只输出标题行
        print "$hashes ";
        print "$number " if $number;
        print "$title_content\n";
    }
}

# 计算字符串的显示宽度（考虑中文）
sub calculate_display_width {
    my ($str) = @_;
    
    # 统计中文字符（每个算2个宽度）和英文字符（每个算1个宽度）
    my $width = 0;
    
    # 按字符遍历字符串
    my @chars = split(//, $str);
    foreach my $char (@chars) {
        if ($char =~ /[\x{4e00}-\x{9fff}]/) {
            # 中文字符
            $width += 2;
        } else {
            # 英文字符和其他字符
            $width += 1;
        }
    }
    
    return $width;
}

# 显示帮助信息
sub print_help {
    print <<"HELP";
用法: mdtitle_order.pl [选项] [文件名]

提取并编号 markdown 文档标题

选项:
  --level=N    从几级标题开始编号（默认2，即##开头）
  --depth=N    最多为几层标题编号（默认4）
  --toc[=N]    控制输出格式（默认输出完整文件）
  --help       显示此帮助信息

--toc 参数说明:
  不带--toc选项      输出完整文件内容（标题行编号，非标题行原样输出）
  --toc 或 --toc=0   只输出标题行
  --toc=-1          输出"行号\\t标题"格式
  --toc=N (N>0)     标题后加行号，对齐到第N列（考虑中文宽度）

示例:
  # 输出完整重编号的文件
  cat docs/api.md | mdtitle_order.pl > new_api.md
  
  # 只输出标题行目录
  cat docs/api.md | mdtitle_order.pl --toc
  
  # 标题带行号格式
  cat docs/api.md | mdtitle_order.pl --toc=-1
  cat docs/api.md | mdtitle_order.pl --toc=40

编号格式:
  # 一级标题不编号
  ## 1 / ### 1.1 / #### 1.1.1 / ##### 1.1.1.1
  ## 2 / ### 2.1 / #### 2.1.1 / ##### 2.1.1.1

如果 --level=0 表示标题不编号，原有编号将被删除
HELP
}