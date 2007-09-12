<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.6: Known Limitations', WINMERGE_2_6);
?>
<h2>Known Limitations</h2>
<ul>
  <li>
    <p><strong>Sometimes you cannot merge two files because the program says they are binary, even if they are mostly text files.</strong></p>
    <p><i>Workaround</i>: Use the new <code>EditBinaryFiles.dll</code> plugin.</p>
  </li>
  <li>
    <p><strong>There is no way to specify the codepage for a particular file.</strong></p>
    <p><i>Workaround</i>: Specify the default codepages for all files in Edit-&gt;Options-&gt;Codepage-&gt;Custom codepage.</p>
  </li>
  <li><p><strong>There is no 3-way merge.</strong></p></li>
  <li>
    <p><strong>$ does not match end of line in linefilter regular expressions</strong></p>
    <p><i>Workaround</i>: Construct your regular expression so that you don't need to use $.</p>
  </li>
  <li>
    <p><strong>Filters only applied when using full compare</strong></p>
    <p>Line filtering is only applied in folder compare when using <i>Full Contents</i>-compare method.</p>
    <p>If you are using any other compare method, line filters are not applied. Files marked different
    in folder compare can get status changed to identical when opening them to file compare.</p>
  </li>
  <li>
    <p><strong>Some plugins shipped with WinMerge require Visual Basic runtime library to work</strong></p>
    <p><i>Workaround</i>: You can download the required file <code><a href="http://www.dll-files.com/dllindex/dll-files.shtml?msvbvm60">msvbvm60.dll</a></code>
       or use the <a href="http://www.microsoft.com/downloads/details.aspx?familyid=7b9ba261-7a9c-43e7-9117-f673077ffb3c">Run-Time Redistribution Pack</a> from Microsoft.</p>
    <p><i>Workaround 2</i>: If you don't need the plugin, you can safely remove it.</p>
  </li>
  <li>
    <p><strong>VSS integration cannot add new files to VSS</strong></p>
    <p>If you need to add a new file, you must add it via VSS client program.</p>
  </li>
  <li>
    <p><strong>Unset WinMerge integration from ClearCase</strong></p>
    <p>To remove WinMerge integration, you'll need to edit one file by hand:</p>
    <ol>
      <li>Find ClearCase installation folder<br />
          Usually it is something like <code>C:\Program Files\Rational\ClearCase</code></li>
      <li>Go to subfolder <code>lib\mgrs\</code></li>
      <li>Open file <code>map</code> in there to editor program</li>
      <li>Comment-out the line starting with "<code>text_file_delta xcompare</code>", containing "<code>WinMergeU.exe</code>"</li>
      <li>Uncomment the line starting with "<code>text_file_delta xcompare</code>", containing "<code>cleardiffmrg.exe</code>"</li>
    </ol>
  </li>
</ul>
<?php
  $page->printFoot();
?>