#!/usr/bin/env perl
use strict;
use warnings;

# script/stat_perf.pl
# Usage:
#   perl script/stat_perf.pl [options] [test-case-names...]
#
# Options:
#   --help                 Print help message
#   --stat_run=N           Number of times to run perf_test (default: 1)
#   --stat_format=format  Output format: markdown, csv, tsv (default: markdown)
#   --runtime_ms=ms        Minimum runtime per test (ms) (default: 200)
#   --overhead_percent=N   Maximum allowed overhead percentage (default: 5)
#   --logfile=file         Save perf_test output to specified file
#
# Purpose:
#   Automatically run performance tests multiple times and generate statistical reports
#   Results are printed to STDOUT, debug info to STDERR

use Getopt::Long;
use File::Temp qw(tempfile);
use Cwd qw(abs_path);

# Default values
my $stat_run = 1;
my $stat_format = 'markdown';
my $runtime_ms = 200;
my $overhead_percent = 5;
my $logfile = '';
my @test_cases = ();
my $help = 0;
my $debug = 0;

# Parse command line arguments
GetOptions(
    'help' => \$help,
    'debug' => \$debug,
    'stat_run=i' => \$stat_run,
    'stat_format=s' => \$stat_format,
    'runtime_ms=i' => \$runtime_ms,
    'overhead_percent=i' => \$overhead_percent,
    'logfile=s' => \$logfile,
) or die "Error in command line arguments\n";

@test_cases = @ARGV;

if ($help) {
    print_help();
    exit 0;
}

# Validate parameters
$stat_run >= 0 or die "ERROR: stat_run must be non-negative\n";
$stat_format =~ /^(markdown|csv|tsv)$/ or die "ERROR: Invalid format '$stat_format'. Use markdown, csv, or tsv\n";
$runtime_ms > 0 or die "ERROR: runtime_ms must be positive\n";

# Determine log file path for stat_run == 0
if ($stat_run == 0) {
    # Use default log file if not specified
    $logfile = 'perf_results.log' unless $logfile;
    warn "INFO: stat_run=0, directly parsing log file: $logfile\n";
    
    # Check if log file exists
    my $logfile_path = abs_path($logfile) if $logfile =~ m{^[^/]}; # make relative path absolute
    $logfile_path = $logfile unless $logfile_path; # if absolute path already
    
    unless (-f $logfile_path) {
        die "ERROR: Log file '$logfile_path' not found. Please specify an existing log file with --logfile\n";
    }
    
    # Parse the log file directly
    my %results;
    parse_perf_log($logfile_path, \%results);
    
    # Generate statistical report
    print_statistical_report(\%results, $overhead_percent, $stat_format);
    
    warn "INFO: Performance test analysis completed from log file\n";
    exit 0;
}

# Normal execution path for stat_run >= 1

# Find repo root and perf_test executable
my $repo_root = find_repo_root();
my $perf_test = "$repo_root/build/perf_test";
-f $perf_test or die "ERROR: perf_test executable not found at $perf_test\n";

warn "INFO: Running performance test $stat_run times with runtime_ms=$runtime_ms, overhead_percent=$overhead_percent\n";

# Run performance tests and collect results
my %results = run_performance_tests($stat_run);

# Generate statistical report
print_statistical_report(\%results, $overhead_percent, $stat_format);

warn "INFO: Performance test analysis completed\n";
exit 0;

# Subroutines

sub print_help {
    print <<'HELP';
Performance Test Statistics Script

Usage:
  perl script/stat_perf.pl [options] [test-case-names...]

Options:
  --help                 Print this help message
  --stat_run=N          Number of times to run perf_test (0=parse existing log, default: 1)
  --stat_format=format  Output format: markdown, csv, tsv (default: markdown)
  --runtime_ms=ms       Minimum runtime per test (ms) (default: 200)
  --overhead_percent=N  Maximum allowed overhead percentage (default: 5)
  --logfile=file        Save perf_test output to specified file (or parse from file when stat_run=0)

Examples:
  perl script/stat_perf.pl
  perl script/stat_perf.pl --stat_run=10 --runtime_ms=500
  perl script/stat_perf.pl --stat_run=5 --logfile=perf_results.log
  perl script/stat_perf.pl access_array iterator_array
  perl script/stat_perf.pl --stat_run=0 --logfile=perf_results.log
  perl script/stat_perf.pl --stat_run=0 # Use default perf_results.log

Notes:
  - Results are printed to STDOUT in the specified format
  - Debug and progress information are printed to STDERR
  - The script will run all test cases if none are specified
  - If --logfile is specified, the perf_test output will be saved to that file
HELP
}

sub find_repo_root {
    # Find repository root by looking for task_todo.md
    require File::Basename;
    require File::Spec;
    
    my $dir = File::Basename::dirname(File::Spec->rel2abs($0));
    for (1..5) {
        my $candidate = File::Spec->catdir($dir, '..');
        $candidate = Cwd::realpath($candidate);
        if (-f File::Spec->catfile($dir, 'task_todo.md')) {
            return $dir;
        }
        if (-f File::Spec->catfile($candidate, 'task_todo.md')) {
            return $candidate;
        }
        $dir = $candidate;
    }
    # Fallback to current working directory
    return Cwd::getcwd();
}

sub run_performance_tests {
    my ($num_runs) = @_;
    my %results;
    
    # Determine log file
    my $logfile_handle;
    my $logfile_path;
    
    if ($logfile) {
        # Use specified log file
        $logfile_path = abs_path($logfile) if $logfile =~ m{^[^/]}; # make relative path absolute
        warn "INFO: Using specified log file: $logfile_path\n";
        
        # Open specified file for writing
        open $logfile_handle, '>', $logfile_path or die "ERROR: Cannot write to log file $logfile_path: $!\n";
    } else {
        # Create temporary log file
        ($logfile_handle, $logfile_path) = tempfile("perf_test_XXXXXX", SUFFIX => '.log', TMPDIR => 1, UNLINK => 1);
        warn "INFO: Created temporary log file: $logfile_path\n";
    }
    
    # Run performance tests multiple times
    for my $run (1..$num_runs) {
        warn "INFO: Running performance test iteration $run/$num_runs\n";
        
        # Build command line for perf_test
        my @cmd = ($perf_test);
        push @cmd, "--runtime_ms=$runtime_ms";
        push @cmd, "--overhead_percent=$overhead_percent";
        push @cmd, @test_cases if @test_cases;
        
        # Add separator for multiple runs
        if ($run > 1) {
            print $logfile_handle "\n" . "=" x 60 . "\n";
            print $logfile_handle "# Run $run\n";
            print $logfile_handle "=" x 60 . "\n\n";
        }
        
        # Run perf_test and capture output
        my $cmd_str = join(' ', @cmd);
        my $output = `$cmd_str 2>&1`;
        
        # Save output to log file
        print $logfile_handle $output;
    }
    
    close $logfile_handle;
    
    # Parse the complete log file
    parse_perf_log($logfile_path, \%results);
    
    # Only report if it's a user-specified file
    if ($logfile) {
        warn "INFO: Log file saved: $logfile_path\n";
    }
    
    return %results;
}

sub parse_perf_log {
    my ($logfile, $results_ref) = @_;
    
    open my $fh, '<', $logfile or die "ERROR: Cannot read log file $logfile: $!\n";
    
    my $current_case = '';
    my $current_overhead = 0;
    my $run_count = 0;
    
    while (my $line = <$fh>) {
        chomp $line;
        
        # Skip separator lines between runs
        if ($line =~ /^={60}$/ || $line =~ /^# Run \d+$/) {
            next;
        }
        
        # Check for test case start
        if ($line =~ /^##\s+run\s+(\w+)\(\)/) {
            $current_case = $1;
            $current_overhead = 0;
            next;
        }
        
        # Extract overhead percentage
        if ($line =~ /--\s+开销%:\s*([-+]?\d+(?:\.\d+)?)%/) {
            $current_overhead = $1;
            next;
        }
        
        # Check for test case end
        if ($line =~ /^<<\s+\[(PASS|FAIL)\]\s+(\d+)\s+(\w+)\s+/) {
            my $status = $1;
            my $case_name = $3;
            
            if ($current_case eq $case_name) {
                # Initialize data structure for this test case if not exists
                if (!exists $results_ref->{$case_name}) {
                    $results_ref->{$case_name} = {
                        overheads => [],
                        pass_count => 0,
                        fail_count => 0
                    };
                }
                
                # Store results
                push @{$results_ref->{$case_name}{overheads}}, $current_overhead;
                if ($status eq 'PASS') {
                    $results_ref->{$case_name}{pass_count}++;
                } else {
                    $results_ref->{$case_name}{fail_count}++;
                }
                
                $run_count++;
                warn "INFO: Run $run_count - $case_name: overhead=$current_overhead%, status=$status\n" if $debug;
            }
            
            $current_case = '';
            $current_overhead = 0;
        }
    }
    
    close $fh;
}

sub print_statistical_report {
    my ($results_ref, $max_overhead, $format) = @_;
    
    my @cases = sort keys %$results_ref;
    
    if ($format eq 'markdown') {
        print_markdown_table($results_ref, $max_overhead, \@cases);
    } elsif ($format eq 'csv') {
        print_csv_table($results_ref, $max_overhead, \@cases);
    } elsif ($format eq 'tsv') {
        print_tsv_table($results_ref, $max_overhead, \@cases);
    }
}

sub print_markdown_table {
    my ($results_ref, $max_overhead, $cases_ref) = @_;
    
    # Calculate maximum case name length for dynamic column width
    my $max_case_length = 10; # minimum width
    foreach my $case (@$cases_ref) {
        my $length = length($case);
        $max_case_length = $length if $length > $max_case_length;
    }
    $max_case_length += 2; # Add some padding
    
    print "# 性能测试统计报告\n\n";
    
    # Build header with dynamic case name column width
    printf "| %-${max_case_length}s     | %-13s   | %-13s   | %-13s   | %-10s  | %-8s |\n",
           "测试用例", "最小开销(%)", "最大开销(%)", "平均开销(%)", "通过率(%)", "最终结果";
    
    # Build separator line with dynamic widths
    printf "|-%s-|-%s-|-%s-|-%s-|-%s-|-%s-|\n",
           "-" x $max_case_length, "-" x 13, "-" x 13, "-" x 13, "-" x 10, "-" x 8;
    
    foreach my $case (@$cases_ref) {
        my $data = $results_ref->{$case};
        my $overheads = $data->{overheads};
        
        my ($min_overhead, $max_overhead_val, $avg_overhead) = calc_stats(@$overheads);
        my $pass_rate = ($data->{pass_count} / scalar(@$overheads)) * 100;
        my $final_result = $avg_overhead <= $max_overhead ? 'PASS' : 'FAIL';
        
        printf "| %-${max_case_length}s | %8.2f      | %8.2f      | %8.2f      | %7.1f    | %-8s |\n",
               $case, $min_overhead, $max_overhead_val, $avg_overhead, $pass_rate, $final_result;
    }
    
    print "\n*阈值: ${max_overhead}%*\n";
}

sub print_csv_table {
    my ($results_ref, $max_overhead, $cases_ref) = @_;
    
    print "测试用例,最小开销(%),最大开销(%),平均开销(%),通过率(%),最终结果\n";
    
    foreach my $case (@$cases_ref) {
        my $data = $results_ref->{$case};
        my $overheads = $data->{overheads};
        
        my ($min_overhead, $max_overhead_val, $avg_overhead) = calc_stats(@$overheads);
        my $pass_rate = ($data->{pass_count} / scalar(@$overheads)) * 100;
        my $final_result = $avg_overhead <= $max_overhead ? 'PASS' : 'FAIL';
        
        printf "%s,%.2f,%.2f,%.2f,%.1f,%s\n",
               $case, $min_overhead, $max_overhead_val, $avg_overhead, $pass_rate, $final_result;
    }
}

sub print_tsv_table {
    my ($results_ref, $max_overhead, $cases_ref) = @_;
    
    print "测试用例\t最小开销(%)\t最大开销(%)\t平均开销(%)\t通过率(%)\t最终结果\n";
    
    foreach my $case (@$cases_ref) {
        my $data = $results_ref->{$case};
        my $overheads = $data->{overheads};
        
        my ($min_overhead, $max_overhead_val, $avg_overhead) = calc_stats(@$overheads);
        my $pass_rate = ($data->{pass_count} / scalar(@$overheads)) * 100;
        my $final_result = $avg_overhead <= $max_overhead ? 'PASS' : 'FAIL';
        
        printf "%s\t%.2f\t%.2f\t%.2f\t%.1f\t%s\n",
               $case, $min_overhead, $max_overhead_val, $avg_overhead, $pass_rate, $final_result;
    }
}

# Utility functions for statistical calculations

sub calc_stats {
    my @values = @_;
    return (undef, undef, undef) unless @values;
    
    my $min = $values[0];
    my $max = $values[0];
    my $sum = 0;
    
    foreach my $val (@values) {
        $min = $val if $val < $min;
        $max = $val if $val > $max;
        $sum += $val;
    }
    
    my $avg = $sum / scalar(@values);
    return ($min, $max, $avg);
}

1;
