<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.4: Known Limitations', WINMERGE_2_4);
?>
<h2>Known Limitations</h2>
<ul>
  <li>
    <p><strong>Sometimes you cannot merge two files because the program says they are binary, even if they are mostly text files.</strong></p>
    <p><i>Workaround</i>: Use the new <code>EditBinaryFiles.dll</code> plugin available in <a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158">development version 2.5</a>.</p>
  </li>
  <li>
    <p><strong>There is no way to specify the codepage for a particular file.</strong></p>
    <p><i>Workaround</i>: Specify the default codepages for all files in Edit-&gt;Options-&gt;Codepage-&gt;Custom codepage.</p>
  </li>
  <li><p><strong>There is no 3-way merge.</strong></p></li>
  <li>
    <p><strong>There are no line numbers.</strong></p>
    <p><i>Workaround</i>: Click on desired line and look in lower right corner status bar to see line number.</p>
    <p><img src="images/statusbar.gif" alt="Status Bar"></p>
  </li>
  <li>
    <p><strong>Shell Extension does now work in 64-bit Windows</strong></p>
    <p>64-bit Windows versions cannot load WinMerge's Shell Extension. There is limitation in 64-bit Windows that 64-bit code cannot load 32-bit DLLs. Naturally Shell in 64-bit Windows is 64-bit code. And WinMerge Shell Extension is 32-bit DLL.</p>
    <p><i>Workaround</i>: There is no workaround. This will be fixed when 64-bit version from Shell Extension is released. There are no plans for that yet.</p>
  </li>
  <li>
    <p><strong>$ does not match end of line in linefilter regular expressions</strong></p>
    <p><i>Workaround</i>: Construct your regular expression so that you don't need to use $.</p>
  </li>
</ul>
<?php
  $page->printFoot();
?>