# testdiff.pl
# Self-tests for diffutils diff & WinMerge
#
# Created: 2006-01-21, Perry Rapp
# Edited:  2006-01-23, Perry Rapp
#

use strict;
use warnings;

my $testCount=0;
my $failCount=0;

# diff2winmergeU translates gnu diff style arguments (eg "-b") to WinMerge style arguments (eg "/ignorews:1")

# WinMerge arguments used:
#  "/noprefs" do not load options from registry
#  "/noninteractive" close as soon as file compare completes
#  "/minimize" keep main window minimized
#  "/ub" do not add files to MRU (neither left nor right)

my $DIFF = "..\\Build\\MergeUnicodeDebug\\diff2winmergeU.exe ..\\Build\\MergeUnicodeDebug\\WinMergeU.exe /noprefs /noninteractive /minimize /ub";

# Global options
my $noisy=0; # 1 to echo every test
my $abort=0; # 1 to abort at first failure

# Predeclare subs

sub processArgs;
sub testdiffs;
sub test3set;
sub testpair;
sub testdiff;
sub rpterr;

# Main Code

 processArgs;

 testdiffs;

 # Pause so user can review output
 print "Failure count: $failCount (of $testCount) -- Press <enter>\n";
 my $temp = <STDIN>;


# Subroutines below

sub testdiffs {

  #  -i  --ignore-case  Ignore case differences in file contents.
  #  -E  --ignore-tab-expansion  Ignore changes due to tab expansion.
  #  -b  --ignore-space-change  Ignore changes in the amount of white space.
  #  -w  --ignore-all-space  Ignore all white space.
  #  -B  --ignore-blank-lines  Ignore changes whose lines are all blank.
  #  -q  (this is used to fill in as blank argument)

  test3set "t001", "-q", 1;
  test3set "t001", "-i", 0;
  test3set "t001", "-E", 1;
  test3set "t001", "-b", 1;
  test3set "t001", "-w", 1;
  test3set "t001", "-B", 1;
  test3set "t001", "-Ei", 0;
  test3set "t001", "-Bi", 0;
  test3set "t001", "-bi", 0;

  test3set "t002", "-q", 1;
  test3set "t002", "-i", 1;
  test3set "t002", "-E", 1;
  test3set "t002", "-b", 0;
  test3set "t002", "-w", 0;
  test3set "t002", "-B", 1;
  test3set "t002", "-Ei", 1;
  test3set "t002", "-Bi", 1;
  test3set "t002", "-bi", 0;

  test3set "t003", "-q", 1;
  test3set "t003", "-i", 1;
  test3set "t003", "-E", 1;
  test3set "t003", "-b", 1;
  test3set "t003", "-w", 1;
  test3set "t003", "-B", 1;
  test3set "t003", "-Ei", 1;
  test3set "t003", "-Bi", 1;
  test3set "t003", "-bi", 1;

  test3set "t004", "-q", 1;
  test3set "t004", "-i", 1;
  test3set "t004", "-E", 1;
  test3set "t004", "-b", 1;
  test3set "t004", "-w", 1;
  test3set "t004", "-B", 0;
  test3set "t004", "-Ei", 1;
  test3set "t004", "-Bi", 0;
  test3set "t004", "-bi", 1;

  # two line file differing only in whitespace at end of line
  test3set "bug1406950", "-q", 1;
  test3set "bug1406950", "-B", 1;
  test3set "bug1406950", "-b", 0;
  test3set "bug1406950", "-w", 0;
}

# Pass in file base names, flags to use, and expected result (0 or 1)
# This runs u/ and w/ and m/ versions all 3
sub test3set {
  my $fbase = $_[0];
  my $flags = $_[1];
  my $result = $_[2];

  testpair "u/$fbase", $flags, $result;
  testpair "w/$fbase", $flags, $result;
  testpair "m/$fbase", $flags, $result;
}

# Pass in file base names, flags to use, and expected result (0 or 1)
sub testpair {
  my $fbase = $_[0];
  my $flags = $_[1];
  my $result = $_[2];

  testdiff $fbase . "a.txt", $fbase . "b.txt", $flags, $result;
}

# Pass in files to compare, flags to use, and expected result (0 or 1)
sub testdiff {
  my $file1 = $_[0];
  my $file2 = $_[1];
  my $flags = $_[2];
  my $result = $_[3];

  if ($noisy == 1) { 
    my $ordCount = $testCount+1;
    print "$ordCount) diffing $flags $file1 $file2 (expect $result)\n";
  }


  system("$DIFF $flags $file1 $file2");
  my $exit_value  = $? >> 8;

  $testCount = $testCount + 1;

  if ($exit_value != $result) {
     rpterr("$flags $file1 vs $file2 => $result ($exit_value)\n");
  }

}

sub processArgs {
  my $argnum;
  foreach $argnum (0 .. $#ARGV) {
    if ($ARGV[$argnum] eq "noisy") { $noisy=1; }
    if ($ARGV[$argnum] eq "abort") { $abort=1; }
  }  
}

sub rpterr {
  print "FAILED TEST: @_";
  $failCount = $failCount + 1;
  if ($abort != 0) {
    print "Aborting after $testCount tests\n";
    exit; 
  }
}

