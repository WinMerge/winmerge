<?php
  include('../page.inc');
  
  $page = new Page;
  $page->printHead('WinMerge: Using WinCVS', WINMERGE_DEV);
?>
<h2>Using WinCVS</h2>
<p>After much trial and error, I finally seem to have hit the sweet spot
with WinCVS, and it works pretty well. To get access to the
anonymous CVS server, do the following (note that this does not require SSH).</p>

<ol>
  <li>Install <a href="http://www.wincvs.org" target="_top">WinCVS</a>.</li>
  <li>Create a directory on your system that will 
    contain the WinMerge and Common repositories (I call it <code>Merge</code> on my 
    system).</li>
  <li>In WinCVS, select "Change Browse Location" from 
    the View menu and navigate to the directory you just created. Your 
    base folder should now appear in the left pane of the WinCVS main window 
    and should be selected.</li>
  <li>Select "Checkout module" from the Create menu.</li>
  <li>Enter "<code>WinMerge</code>" (no quotes) in the "Enter 
    module name and path on the server" field. Note that 
    capitalization is important. 
  <li>Select the General tab.</li>
  <li>Enter "<code>:pserver:anonymous@cvs.sourceforge.net:/cvsroot/winmerge</code>" 
    in the "Enter the CVSROOT" field (again, no quotes).</li>
  <li>Select "passwd file on the CVS server" in the 
    Authentication section.</li>
  <li>Click OK. This should download a copy of 
    the source code into a subdirectory called <code>WinMerge</code>.</li>
</ol>
<?php
  $page->printFoot();
?>