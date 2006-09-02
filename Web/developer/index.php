<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Developers', WINMERGE_DEV);
?>
<h2>WinMerge developers</h2>
<p>Following is a list of the current developers on the WinMerge project:</p>
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
<p>Look here for more information on how to <a href="needed.php">develop</a> and/or 
<a href="l10n/index.php">translate</a> WinMerge, 
and read how to use <a href="wincvs.php">WinCVS</a> with our project.</p>
<?php
  $page->printFoot();
?>