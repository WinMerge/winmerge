2004-01-21 Kimmo
 Fix also compile of "Win32 Release MinDependency" target
  ShellExtension: ShellExtension.dsp

2004-01-15 Kimmo
 Make ShellExtension compile after CVS reorg
  ShellExtension: ShellExtension.dsp stdafx.h stdafx.cpp
 
2003-11-03 Kimmo
 PATCH: [ 834526 ] Check that file exist before executing in ShellExtension
  ShellExtension: ShellExtension.dsp WinMergeShell.h WinMergeShell.cpp

2003-08-17 Kimmo
 Updated comments for ShellExtension: added comment about used registry values etc
  ShellExtension:
   ShellExtension.cpp WinMergeShell.h WinMergeShell.cpp
 Removed two generated files from CVS (Christian spotted those)
  ShellExtension: 
   ShellExtension.h DllData.cp
   
2003-07-29 Kimmo
 Fix Register.bat comments. Batch files use ":" for comments...
  ShellExtension: Register.bat

2003-07-26 Kimmo
 BUG: [ 778166 ] Shell integration missing quotes (2.1)
  ShellExtension: WinMergeShell.cpp

2003-07-25 Kimmo
 PATCH: [ 773296 ] Add shell integration
  Added new module ShellExtension
