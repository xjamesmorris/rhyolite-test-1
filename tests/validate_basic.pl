#!/usr/bin/env perl

use strict;
use warnings;
use File::Spec;

my $program_name = 'apple2_c_to_f.bas';
my $program_path = File::Spec->rel2abs($program_name);

sub record_result {
    my ($name, $passed) = @_;
    print sprintf("%-40s %s\n", $name, $passed ? 'PASS' : 'FAIL');
    return $passed;
}

sub read_program {
    my ($path) = @_;
    open my $fh, '<', $path or die "Unable to read $path: $!\n";
    local $/ = undef;
    my $content = <$fh>;
    close $fh;
    return $content;
}

my $content = read_program($program_path);
my $all_passed = 1;

$all_passed &= record_result('Program file exists', -e $program_path);
$all_passed &= record_result('BASIC header present', $content =~ /REM\s+CELSIUS\s+TO\s+FAHRENHEIT/i);
$all_passed &= record_result('Celsius input is captured', $content =~ /INPUT\s+C\b/i);
$all_passed &= record_result('Conversion formula present', $content =~ /F\s*=\s*C\s*\*\s*9\s*\/\s*5\s*\+\s*32/i);
$all_passed &= record_result('Colored output is configured', $content =~ /COLOR\s*=/i);
$all_passed &= record_result('Program loops for another conversion', $content =~ /GOTO\s+10/i);

exit($all_passed ? 0 : 1);
