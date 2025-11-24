#!/usr/bin/env perl
use strict;
use warnings;

# script/todo.pl
# Usage:
#   perl script/todo.pl <todo-id>
#     - Print the section in task_todo.md that matches <todo-id>
#   perl script/todo.pl <todo-id> <task-id>
#     - Update the matching section by appending/merging a line:
#       "### DONE:<task-id>" (multiple IDs separated by spaces). If a DONE line
#       already exists, merge IDs (dedupe, preserve existing order, append new at end).
# Notes:
#   - Section start: lines like "## TODO:YYYY-MM-DD/n" (colon optional/any char/none between TODO and ID)
#   - Section end: next line starting with "## TODO" (any), or a line of only dashes "---...", or EOF
#   - No external CPAN modules required; pure perl5

my $repo_root = find_repo_root();
my $todo_path = "$repo_root/task_todo.md";

@ARGV >= 1 && @ARGV <= 2 or die_usage();
my ($todo_id, $task_id) = @ARGV;
$todo_id or die "ERROR: <todo-id> is empty\n";
if (defined $task_id) {
    $task_id =~ /\S/ or die "ERROR: <task-id> is empty\n";
}

open my $fh, '<', $todo_path or die "ERROR: cannot open $todo_path: $!\n";
my @lines = <$fh>;
close $fh;

my ($beg, $end) = locate_section(\@lines, $todo_id);
if (!defined $beg) {
    print STDERR "ERROR: TODO section not found for id: $todo_id\n";
    exit 2;
}

if (!defined $task_id) {
    # Read mode: print the section [beg, end)
    for my $i ($beg .. $end-1) {
        print $lines[$i];
    }
    exit 0;
}

# Update mode: merge/append DONE line
my $done_re = qr/^###\s*DONE\s*:\s*(.*)\S\s*$/;
my $done_line_idx;
my @done_ids;
for my $i ($beg .. $end-1) {
    if ($lines[$i] =~ $done_re) {
        $done_line_idx = $i;
        my $tail = $1 // '';
        @done_ids = grep { length } split /\s+/, $tail;
        last; # keep the first DONE line encountered; others (if any) will be removed later
    }
}

my @new_ids = @done_ids;
my %seen = map { $_ => 1 } @done_ids;
if (!$seen{$task_id}) {
    push @new_ids, $task_id;
}

my $newline = '### DONE:' . (@new_ids ? ' ' . join(' ', @new_ids) : '') . "\n";

if (defined $done_line_idx) {
    $lines[$done_line_idx] = $newline;
    # Remove any additional DONE lines within the range
    for (my $i = $done_line_idx + 1; $i < $end; $i++) {
        if ($lines[$i] =~ $done_re) {
            splice @lines, $i, 1;
            $end--; $i--; # adjust window after deletion
        }
    }
} else {
    # Insert before end
    splice @lines, $end, 0, ($newline);
    $end++;
    # Add a separating blank line if not EOF or next is not already blank
    if ($end < @lines && $lines[$end] !~ /^\s*$/) {
        splice @lines, $end, 0, ("\n");
        $end++;
    }
}

open my $ofh, '>', $todo_path or die "ERROR: cannot write $todo_path: $!\n";
print $ofh @lines;
close $ofh;

print "OK: DONE updated for $todo_id with $task_id\n";
exit 0;

sub die_usage {
    print STDERR <<'USAGE';
Usage:
  perl script/todo.pl <todo-id>
  perl script/todo.pl <todo-id> <task-id>

Examples:
  perl script/todo.pl 2025-10-16/1
  perl script/todo.pl 2025-10-16/1 20251016-123456
USAGE
    exit 1;
}

sub locate_section {
    my ($lines_ref, $id) = @_;
    my @lines = @$lines_ref;
    my $esc_id = quotemeta($id);
    # Start: '##' + spaces + 'TODO' + optional single char or nothing before ID
    my $start_re = qr/^##\s*TODO.?\s*$esc_id\b/;
    my $next_todo_re = qr/^##\s*TODO/;
    my $sep_re = qr/^---+\s*$/;

    my ($beg, $end);
    for my $i (0..$#lines) {
        if ($lines[$i] =~ $start_re) { $beg = $i; last; }
    }
    return (undef, undef) unless defined $beg;

    $end = scalar(@lines);
    for my $i ($beg+1 .. $#lines) {
        if ($lines[$i] =~ $next_todo_re || $lines[$i] =~ $sep_re) { $end = $i; last; }
    }
    return ($beg, $end);
}

# Find repository root by checking for .git directory
sub find_repo_root {
    require Cwd; require File::Basename; require File::Spec;
    my $dir = File::Basename::dirname(File::Spec->rel2abs($0));
    for (1..5) {
        my $candidate = File::Spec->catdir($dir, '..');
        $candidate = Cwd::realpath($candidate);
        if (-d File::Spec->catdir($dir, '.git')) {
            return $dir;
        }
        if (-d File::Spec->catdir($candidate, '.git')) {
            return $candidate;
        }
        $dir = $candidate;
    }
    # Fallback to current working directory
    return Cwd::getcwd();
}
