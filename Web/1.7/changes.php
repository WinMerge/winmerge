<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 1.7: Change History', WINMERGE_1_7);
?>
<h2>Change History</h2>
<H3>1.7.1 (10/25/2000)</H3>
<UL>
  <LI>Fixed bug #121116&nbsp; </LI></UL>
<H3>1.6.0.174 (10/25/2000)</H3>
<UL>
  <LI>Finally fixed the LanguageDLL problem, where some machines were 
  unable to load the DLLs.&nbsp; Turns out that specifying /NOENTRY to the 
  linker did the trick.&nbsp; Really all that's needed is new .LANG files, 
  but I added some extra logging to the main app. (Much thanks to <FONT 
  size=3>Cesar de Azevedo Gil for the testing help)</FONT> </LI></UL>
<H3>1.6.0.168 (3/11/2000)</H3>
<UL>
  <LI>Changed binary file detection to be more locale-specific 
  <LI>Fixed splitter-centering code </LI></UL>
<H3>1.6.0.166 (11/29/99)</H3>
<UL>
  <LI>Fixed crash when comparing 1 text &amp; 1 binary file 
  <LI>Now displays proper notification in above condition 
  <LI>Added mousewheel support 
  <LI>Window sizes are now restored </LI></UL>
<H3>1.6.0.163 (11/9/99)</H3>
<UL>
  <LI>Changed method for detecting language of resource DLLs.&nbsp; 
  Hopefully fixes some problems where certain machines can't detect the 
  language (thanks to Gil Andre for much debugging help). </LI></UL>
<H3>1.6.0.161 (11/2/99)</H3>
<UL>
  <LI>Rebuilt using VC++ 6.0.&nbsp; Requires <A 
  href="http://winmerge.sourceforge.net/bin/mfc6dll.zip">updated DLLs</A>. 

  <LI>Also rebuilt all language DLLs. </LI></UL>
<H3>1.5.1.141 (7/23/99)</H3>
<UL>
  <LI>No longer crashes when more that 2 files are specified on command 
  line 
  <LI>Now accepts 3 files on command line for seamless integration with 
  products such as Perforce.&nbsp; First 2 arguments are files to compare, 
  third argument is the filename where the diff file is saved.&nbsp; If 
  the third arg is a directory name, then all changed files are saved to 
  that location. </LI></UL>
<H3>1.5.1.135 (6/11/99)</H3>
<UL>
  <LI>No longer crashes on binary files 
  <LI>Changed "Non-text files are different" to "Binary files are 
  different" </LI></UL>
<H3>1.5.0.134 (6/8/99)</H3>
<UL>
  <LI>Did work on command line file specification.&nbsp; You can now 
  specify one or two files or directories from the command line. (Thanks 
  to <A href="mailto:Bob@pqsystems.com">Bob Sledge</A>)
  <UL>
    <LI>If one is a file &amp; one is a dir, the filename is appended to 
    the dirname &amp; a single file comparison is performed 
    <LI>If only one argument is specified, the Open dialog is displayed. 
    <LI>If one of the arguments specifies a non-existing entity,&nbsp; the 
    Open dialog is displayed. </LI></UL>
  <LI>You can now drag files from Explorer into the edit fields in the 
  Open dialog (Thanks to <A href="mailto:Bob@pqsystems.com">Bob 
  Sledge</A>) 
  <LI>Now reports that files are identical for single file compares 
  <LI>Fixed bug that caused differences within the first 40 characters to 
  not be reported in the directory listing 
  <LI>Added ability to ignore case &amp; blank lines to preferences 
</LI></UL>
<H3>1.4.0.128 (4/23/99)</H3>
<UL>
  <LI>Extensions field now defaults to *.* 
  <LI>Recursive option works again </LI></UL>
<H3>1.3.0.124 (4/16/99)</H3>
<UL>
  <LI>Open dialog now allows you to filter directories by multiple file 
  extensions(ie: *.c;*.cpp;*.h) 
  <LI>Fixed crash in diff code for large files with lots of changes 
  <LI>Cleaned up small memory leak in initial diff code. </LI></UL>
<H3>1.2.0.110 (4/5/99)</H3>
<UL>
  <LI>Font can now be changed for difference views 
  <LI>Added "Copy all diffs to other side" item to diff view's context 
  menu 
  <LI>Added popup menu to directory view 
  <LI>Added ability to change UI language via resource DLLs </LI></UL>
<H3>1.1.0.90 (3/6/99)</H3>
<UL>
  <LI>Added buttons for copying all diffs to other side (Thanks to <A 
  href="mailto:Simon.Dick@HEXAGON.zeneca.com">Dick Simon</A> for the 
  suggestion) 
  <LI>Difference views now properly preserve embedded tabs (Thanks to <A 
  href="mailto:jvivona@kpi-tech.com">Joe Vivona</A> and <A 
  href="mailto:rogersfn@us.ibm.com">Fred Rogers</A>) 
  <LI>Fixed bug in the Open dialog that caused paths with a \ at the end 
  to not be considered as a directory, and therefore, the recursive 
  checkbox would not be enabled (Thanks to <A 
  href="mailto:mcnair@anet-dfw.com">Dewayne McNair</A>) 
  <LI>Fixed bug in Open dialog that caused \Directory Selection to be 
  returned to the edit fields in some cases (Thanks to <A 
  href="mailto:mcnair@anet-dfw.com">Dewayne McNair</A>) </LI></UL>
<H3>1.0.0.65 (1/27/99)</H3>
<UL>
  <LI>Open dialog was initializing both fields the same on the second time 
  around, but has now been fixed (Thanks to <A 
  href="mailto:jvivona@kpi-tech.com">Joe Vivona</A>) 
  <LI>Added preference setting for specifying Tab size </LI></UL>
<H3>1.0.0.63 (1/13/99)</H3>
<UL>
  <LI>Fixed problem that was causing crash for some binary files on some 
  machines (Thanks to <A href="mailto:DOlender@SmartHealth.ca">Doug 
  Olender</A>) </LI></UL>
<H3>1.0.0.52 (1/11/99)</H3>
<UL>
  <LI>Added command line specification of filenames (Thanks to <A 
  href="mailto:hchen@aspentech.com">Hern Chen</A>) 
  <LI>Added -r command line option for recursion 
  <LI>Fixed problem with loading debug libraries 
  <LI>Added version display on splash screen </LI></UL>
<?php
  $page->printFoot();
?>