# testdiff.pl
# Self-tests for diffutils diff & WinMerge
#
# Created: 2006-01-21, Perry Rapp
# Edited:  2006-02-07, Perry Rapp
#

use strict;
use warnings;

my $testCount=0;
my $failCount=0;
my $failmsgs="";
my %testBuilds = ();

# diff2winmergeU translates gnu diff style arguments (eg "-b") to WinMerge style arguments (eg "/ignorews:1")

# WinMerge arguments used:
#  "/noprefs" do not load options from registry
#  "/noninteractive" close as soon as file compare completes
#  "/minimize" keep main window minimized
#  "/ub" do not add files to MRU (neither left nor right)

my @testprogs = (
  [ "MergeUnicodeDebug", "diff2winmergeU.exe", "WinMergeU.exe" ],
  [ "MergeUnicodeRelease", "diff2winmergeU.exe", "WinMergeU.exe" ],
  [ "MergeDebug", "diff2winmerge.exe", "WinMerge.exe" ],
  [ "MergeRelease", "diff2winmerge.exe", "WinMerge.exe" ]
 );

my $wmargs = " /noprefs /noninteractive /minimize /ub";
# "..\\Build\\MergeUnicodeDebug\\diff2winmergeU.exe ..\\Build\\MergeUnicodeDebug\\WinMergeU.exe /noprefs /noninteractive /minimize /ub";
my $DIFF = $wmargs;

# Global options
my $noisy=0; # 1 to echo every test
my $abort=0; # 1 to abort at first failure
my $testMergeUnicodeDebug=0; # 1 to test this build
my $testMergeUnicodeRelease=0; # 1 to test this build
my $testMergeDebug=0; # 1 to test this build
my $testMergeRelease=0; # 1 to test this build
my $testMergeAll=0; # 1 to test all builds
# Predeclare subs

sub checkRequiredPrograms;
sub processArgs;
sub testdiffs;
sub test3set;
sub testpair;
sub testdiff;
sub rpterr;
sub doUsage;

# Main Code

 processArgs;

 checkRequiredPrograms;

 testdiffs;

 # Pause so user can review output
 print "\nFailure count: $failCount (of $testCount) -- Press <enter>\n";
 my $temp = <STDIN>;


# Subroutines below

sub testdiffs {

  #  -i  --ignore-case  Ignore case differences in file contents.
  #  -E  --ignore-tab-expansion  Ignore changes due to tab expansion.
  #  -b  --ignore-space-change  Ignore changes in the amount of white space.
  #  -w  --ignore-all-space  Ignore all white space.
  #  -B  --ignore-blank-lines  Ignore changes whose lines are all blank.
  #  -g  --ignore-line_terminators Treat all line terminations as same (not yet implemented in diffutils)
  #  -q  (this is used to fill in as blank argument)

  # Each call to test3set does the test for all 3 platforms (unix, mac, win)

  # files differ only in case
  test3set "t001", "-q", 1;
  test3set "t001", "-i", 0;
  test3set "t001", "-E", 1;
  test3set "t001", "-b", 1;
  test3set "t001", "-w", 1;
  test3set "t001", "-B", 1;
  test3set "t001", "-g", 1;
  test3set "t001", "-Ei", 0;
  test3set "t001", "-Bi", 0;
  test3set "t001", "-bi", 0;

  # files differ only in length of middle whitespace
  test3set "t002", "-q", 1;
  test3set "t002", "-i", 1;
  test3set "t002", "-E", 1;
  test3set "t002", "-b", 0;
  test3set "t002", "-w", 0;
  test3set "t002", "-B", 1;
  test3set "t001", "-g", 1;
  test3set "t002", "-Ei", 1;
  test3set "t002", "-Bi", 1;
  test3set "t002", "-bi", 0;

  # 2nd file has line broken into two lines, so always different for WinMerge
  test3set "t003", "-q", 1;
  test3set "t003", "-i", 1;
  test3set "t003", "-E", 1;
  test3set "t003", "-b", 1;
  test3set "t003", "-w", 1;
  test3set "t003", "-B", 1;
  test3set "t001", "-g", 1;
  test3set "t003", "-Ei", 1;
  test3set "t003", "-Bi", 1;
  test3set "t003", "-bi", 1;

  # 2nd file has extra blank line in middle
  test3set "t004", "-q", 1;
  test3set "t004", "-i", 1;
  test3set "t004", "-E", 1;
  test3set "t004", "-b", 1;
  test3set "t004", "-w", 1;
  test3set "t004", "-B", 0;
  test3set "t001", "-g", 1;
  test3set "t004", "-Ei", 1;
  test3set "t004", "-Bi", 0;
  test3set "t004", "-bi", 1;

  # two line file differing only in whitespace at end of line
  test3set "bug1406950", "-q", 1;
  test3set "bug1406950", "-B", 1;
  test3set "bug1406950", "-b", 0;
  test3set "bug1406950", "-w", 0;

  # empty file against itself
  testdiff "mixed/empty.txt", "mixed/empty.txt", "-q", 0;
  testdiff "mixed/empty.txt", "mixed/empty.txt", "-w", 0;
  testdiff "mixed/empty.txt", "mixed/empty.txt", "-B", 0;

  # empty file against file with one empty win line  
  testdiff "mixed/empty.txt", "mixed/empty_w.txt", "-q", 1;
  testdiff "mixed/empty.txt", "mixed/empty_w.txt", "-b", 1;
  testdiff "mixed/empty.txt", "mixed/empty_w.txt", "-w", 1;
  testdiff "mixed/empty.txt", "mixed/empty_w.txt", "-B", 0;
  testdiff "mixed/empty.txt", "mixed/empty_w.txt", "-g", 1;
  testdiff "mixed/empty.txt", "mixed/empty_w.txt", "-Bw", 0;
  testdiff "mixed/empty.txt", "mixed/empty_w.txt", "-Bg", 0;

  # empty file against file with one empty unix line  
  testdiff "mixed/empty.txt", "mixed/empty_u.txt", "-q", 1;
  testdiff "mixed/empty.txt", "mixed/empty_u.txt", "-b", 1;
  testdiff "mixed/empty.txt", "mixed/empty_u.txt", "-w", 1;
  testdiff "mixed/empty.txt", "mixed/empty_u.txt", "-B", 0;
  testdiff "mixed/empty.txt", "mixed/empty_u.txt", "-g", 1;
  testdiff "mixed/empty.txt", "mixed/empty_u.txt", "-Bw", 0;
  testdiff "mixed/empty.txt", "mixed/empty_u.txt", "-Bg", 0;

  # empty file against file with one empty mac line  
  testdiff "mixed/empty.txt", "mixed/empty_m.txt", "-q", 1;
  testdiff "mixed/empty.txt", "mixed/empty_m.txt", "-b", 1;
  testdiff "mixed/empty.txt", "mixed/empty_m.txt", "-w", 1;
  testdiff "mixed/empty.txt", "mixed/empty_m.txt", "-B", 0;
  testdiff "mixed/empty.txt", "mixed/empty_m.txt", "-g", 1;
  testdiff "mixed/empty.txt", "mixed/empty_m.txt", "-Bw", 0;
  testdiff "mixed/empty.txt", "mixed/empty_m.txt", "-Bg", 0;

  # empty line win file vs. empty line unix file
  testdiff "mixed/empty_w.txt", "mixed/empty_u.txt", "-q", 1;
  testdiff "mixed/empty_w.txt", "mixed/empty_u.txt", "-w", 1;
  testdiff "mixed/empty_w.txt", "mixed/empty_u.txt", "-B", 0;
  testdiff "mixed/empty_w.txt", "mixed/empty_u.txt", "-g", 0;
  testdiff "mixed/empty_w.txt", "mixed/empty_u.txt", "-Bg", 0;

  # empty line win file vs. empty line mac file
  testdiff "mixed/empty_w.txt", "mixed/empty_m.txt", "-q", 1;
  testdiff "mixed/empty_w.txt", "mixed/empty_m.txt", "-w", 1;
  testdiff "mixed/empty_w.txt", "mixed/empty_m.txt", "-B", 0;
  testdiff "mixed/empty_w.txt", "mixed/empty_m.txt", "-g", 0;
  testdiff "mixed/empty_w.txt", "mixed/empty_m.txt", "-Bg", 0;

  # empty line unix file vs. empty line mac file
  testdiff "mixed/empty_u.txt", "mixed/empty_m.txt", "-q", 1;
  testdiff "mixed/empty_u.txt", "mixed/empty_m.txt", "-w", 1;
  testdiff "mixed/empty_u.txt", "mixed/empty_m.txt", "-B", 0;
  testdiff "mixed/empty_u.txt", "mixed/empty_m.txt", "-g", 0;
  testdiff "mixed/empty_u.txt", "mixed/empty_m.txt", "-Bg", 0;

  # files differ only in case & platform, win vs. unix
  testdiff "w/t001a.txt", "u/t001b.txt", "-q", 1;
  testdiff "w/t001a.txt", "u/t001b.txt", "-i", 1;
  testdiff "w/t001a.txt", "u/t001b.txt", "-w", 1;
  testdiff "w/t001a.txt", "u/t001b.txt", "-B", 1;
  testdiff "w/t001a.txt", "u/t001b.txt", "-g", 1;
  testdiff "w/t001a.txt", "u/t001b.txt", "-gi", 0;

  # files differ only in case & platform, win vs. mac
  testdiff "w/t001a.txt", "m/t001b.txt", "-q", 1;
  testdiff "w/t001a.txt", "m/t001b.txt", "-i", 1;
  testdiff "w/t001a.txt", "m/t001b.txt", "-w", 1;
  testdiff "w/t001a.txt", "m/t001b.txt", "-B", 1;
  testdiff "w/t001a.txt", "m/t001b.txt", "-g", 1;
  testdiff "w/t001a.txt", "m/t001b.txt", "-gi", 0;

  # files differ only in case & platform, unix vs. mac
  testdiff "u/t001a.txt", "m/t001b.txt", "-q", 1;
  testdiff "u/t001a.txt", "m/t001b.txt", "-i", 1;
  testdiff "u/t001a.txt", "m/t001b.txt", "-w", 1;
  testdiff "u/t001a.txt", "m/t001b.txt", "-B", 1;
  testdiff "u/t001a.txt", "m/t001b.txt", "-g", 1;
  testdiff "u/t001a.txt", "m/t001b.txt", "-gi", 0;

  if ($noisy == 1) { 
    # reprint all failure messages, because they were lost in the noise
    print $failmsgs;
  }
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

  # Test each supported build
  for my $i ( 0 .. $#testprogs ) {
    my $build = $testprogs[$i][0];
    if (exists $testBuilds{$build} and $testBuilds{$build} == 1) {
      my $diff2merge = getDiff2merge($i);
      my $winmerge = getWinMerge($i);
      my $cmd = "$diff2merge $winmerge $wmargs";

      if ($noisy == 1) {
        my $ordCount = $testCount+1;
        print "$ordCount) diffing $flags $file1 $file2 (expect $result) [$winmerge]\n";
      }

      system("$cmd $flags $file1 $file2");
      my $exit_value  = $? >> 8;

      $testCount = $testCount + 1;

      if ($exit_value != $result) {
         rpterr("$flags $file1 vs $file2 => $result ($exit_value) [$winmerge ]");
      }
    }
  }
}

sub processArgs {
  my $argnum;
  my $arg;
  my $buildset=0;
  foreach $argnum (0 .. $#ARGV) {
    $arg = $ARGV[$argnum];
    if ($arg eq "noisy") { $noisy=1; }
    if ($arg eq "abort") { $abort=1; }
    if ($arg eq "--help" or $arg eq "/?") { doUsage; }
    if ($arg eq "MergeUnicodeDebug" or $arg eq "UnicodeDebug") {
      $testBuilds{'MergeUnicodeDebug'} = 1;
      $buildset = 1;
    }
    if ($arg eq "MergeUnicodeRelease" or $arg eq "UnicodeRelease") {
      $testBuilds{'MergeUnicodeRelease'} = 1;
      $buildset = 1;
    }
    if ($arg eq "MergeDebug" or $arg eq "Debug") {
      $testBuilds{'MergeDebug'} = 1;
      $buildset = 1;
    }
    if ($arg eq "MergeRelease" or $arg eq "Release") {
      $testBuilds{'MergeRelease'} = 1;
      $buildset = 1;
    }
    if ($arg eq "all") { $testMergeAll=1; }
  }
  # if no build chosen, all are tested
  if ($buildset == 0) {
      $testMergeAll=1;
  }
  if ($testMergeAll==1) {
    $testBuilds{'MergeUnicodeDebug'} = 1;
    $testBuilds{'MergeUnicodeRelease'} = 1;
    $testBuilds{'MergeDebug'} = 1;
    $testBuilds{'MergeRelease'} = 1;
  }
}

sub doUsage {
  print "testdiff [noisy] [abort] [all] [UnicodeDebug] [UnicodeRelease] [Debug] [Release]";
  exit;
}

sub rpterr {
  $failCount = $failCount + 1;
  my $msg = "FAILED TEST ($failCount): @_";
  print $msg;
  $failmsgs = "$failmsgs\n$msg";
  if ($abort != 0) {
    print "Aborting after $testCount tests\n";
    exit; 
  }
}

sub checkRequiredPrograms {

  my $missing = "";
  for my $i ( 0 .. $#testprogs ) {

    my $build = $testprogs[$i][0];
    if (exists $testBuilds{$build} and $testBuilds{$build} == 1) {

      my $diff2merge = getDiff2merge($i);
      my $WinMerge = getWinMerge($i);
      -e $diff2merge or $missing = "$missing\n$diff2merge";
      -e $WinMerge or $missing = "$missing\n$WinMerge";
    }
  }
  if (length($missing)>0) {
    die "Missing required files:$missing\n";
  }
}

# Request diff2merge path
# Takes argument from 0-3 for build to test
sub getDiff2merge {
  "..\\..\\Tools\\Build\\$testprogs[$_[0]][0]\\$testprogs[$_[0]][1]";
}

# Request WinMerge path
# Takes argument from 0-3 for build to test
sub getWinMerge {
  "..\\..\\Build\\$testprogs[$_[0]][0]\\$testprogs[$_[0]][2]";
}
