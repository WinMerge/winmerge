<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Developers', WINMERGE_DEV);
?>
<h2>WinMerge developers</h2>
<p>This is a list of some of the current developers on WinMerge</p>
<?php
  $xmlarray = $page->convertXml2Array('developers.xml');
  
  print("<ul>\n");
  $developers = $xmlarray['DEVELOPERS'][0]['DEVELOPER'];
  $developers = $page->multisortArray($developers, 'NAME', SORT_ASC);
  for ($i = 0; $i < count($developers); $i++) { //For all developers...
    $developer = $developers[$i];
    $developer_name = $developer['NAME'][0]['VALUE'];
    $developer_username = $developer['USERNAME'][0]['VALUE'];
    $developer_role = $developer['ROLE'][0]['VALUE'];
    $developer_email = $developer['EMAIL'][0]['VALUE'];
    
    switch ($developer_role) {
      case 1: //Project Manager
        print("  <li><strong><a href=\"mailto:" . $developer_email . "\" title=\"Project Manager: " . $developer_name . " &lt;" . $developer_email . "&gt;\">" . $developer_name . "</a></strong></li>");
        break;
      case 2: //Developer
        print("  <li><a href=\"mailto:" . $developer_email . "\" title=\"Developer: " . $developer_name . " &lt;" . $developer_email . "&gt;\">" . $developer_name . "</a></li>");
        break;
      case 3: //Packager
        print("  <li><a href=\"mailto:" . $developer_email . "\" title=\"Packager: " . $developer_name . " &lt;" . $developer_email . "&gt;\">" . $developer_name . "</a></li>");
        break;
      case 4: //Web Designer
        print("  <li><a href=\"mailto:" . $developer_email . "\" title=\"Web Designer: " . $developer_name . " &lt;" . $developer_email . "&gt;\">" . $developer_name . "</a></li>");
        break;
    }
  }
  print("</ul>\n");
?>
<p>Look how to <a href="needed.php">develop</a> and 
<a href="l10n/index.php">translate WinMerge</a>
and get some hints for <a href="wincvs.php">WinCVS</a>.</p>
<?php
  $page->printFoot();
?>