<?php
  include('../engine/engine.inc');

  $page = new Page;
  $page->setDescription(__('The easiest way to get informations about the latest stable version from WinMerge for your web site is the Portable Application Description file, which you find here.'));
  $page->setKeywords(__('WinMerge, PAD file, Portable Application Description'));
  $page->printHead(__('PAD File'), TAB_DOWNLOADS);
  
  $page->printHeading(__('PAD File'));
  $page->printPara(__('The easiest way to get informations about the latest stable version from WinMerge for your web site is the <a href="%s">Portable Application Description</a> file, which you find here:', 'http://www.asp-shareware.org/pad/'));
?>
<pre class="code"><a href="http://winmerge.org/WinMergePAD.xml">http://winmerge.org/WinMergePAD.xml</a></pre>
<?php
  $page->printPara(__('If you can not work with the <a href="%s">PAD file</a>, you can use the following informations:', '/WinMergePAD.xml'));
  
  $padfile = @simplexml_load_file('../WinMergePAD.xml');
  if ($padfile) { //if PAD file was valid...
    $company_info = $padfile->Company_Info;
    $program_info = $padfile->Program_Info;
    $file_info = $program_info->File_Info;
    $program_descriptions = $padfile->Program_Descriptions->English;
    $web_info = $padfile->Web_Info;

    $program_name = (string)$program_info->Program_Name;
    $program_version = (string)$program_info->Program_Version;
    $release_date_month = (string)$program_info->Program_Release_Month;
    $release_date_day = (string)$program_info->Program_Release_Day;
    $release_date_year = (string)$program_info->Program_Release_Year;
    $release_status = (string)$program_info->Program_Release_Status;
    $download = (string)$web_info->Download_URLs->Primary_Download_URL;
    $file_size_mb = (int)$file_info->File_Size_MB;
    $program_os_support = (string)$program_info->Program_OS_Support;
    $program_language = (string)$program_info->Program_Language;
    $program_type = (string)$program_info->Program_Type;
    print("<div class=\"padfile\">\n");
    print("<h2>" . $program_name . " " . $program_version . "</h2>\n");
    print("<dl>\n");
    print("  <dt>" . __('Release Date') . "</dt>\n");
    print("  <dd>" . $release_date_month . "/" . $release_date_day . "/" . $release_date_year . "</dd>\n");
    print("  <dt>" . __('Release Status') . "</dt>\n");
    print("  <dd>" . $release_status . "</dd>\n");
    print("  <dt>" . __('Download') . "</dt>\n");
    print("  <dd><a href=\"" . $download . "\">" . $download . "</a> (" . $file_size_mb . " MB)</dd>\n");
    print("  <dt>" . __('Operating Systems') . "</dt>\n");
    print("  <dd>" . str_replace(',', ', ', $program_os_support) . "</dd>\n");
    print("  <dt>" . __('Supported Languages') . "</dt>\n");
    print("  <dd>" . str_replace(',', ', ', $program_language) . "</dd>\n");
    print("  <dt>" . __('Type') . "</dt>\n");
    print("  <dd>" . $program_type . "</dd>\n");
    print("</dl>\n");

    $description_80 = (string)$program_descriptions->Char_Desc_80;
    $description_250 = (string)$program_descriptions->Char_Desc_250;
    $description_450 = (string)$program_descriptions->Char_Desc_450;
    $keywords = (string)$program_descriptions->Keywords;
    print("<h3>" . __('English Descriptions') . "</h3>\n");
    print("<dl>\n");
    print("  <dt>" . __('English Description (%d chars)', 80) . "</dt>\n");
    print("  <dd>" . $description_80 . "</dd>\n");
    print("  <dt>" . __('English Description (%d chars)', 250) . "</dt>\n");
    print("  <dd>" . $description_250 . "</dd>\n");
    print("  <dt>" . __('English Description (%d chars)', 450) . "</dt>\n");
    print("  <dd>" . $description_450 . "</dd>\n");
    print("  <dt>" . __('English Keywords') . "</dt>\n");
    print("  <dd>" . str_replace(',', ', ', $keywords) . "</dd>\n");
    print("</dl>\n");

    $info_url = (string)$web_info->Application_URLs->Application_Info_URL;
    $screenshot_url = (string)$web_info->Application_URLs->Application_Screenshot_URL;
    $icon_url = (string)$web_info->Application_URLs->Application_Icon_URL;
    $pad_url = (string)$web_info->Application_URLs->Application_XML_File_URL;
    print("<h3>" . __('Web Addresses') . "</h3>\n");
    print("<dl>\n");
    print("  <dt>" . __('Information') . "</dt>\n");
    print("  <dd><a href=\"" . $info_url . "\">" . $info_url . "</a></dd>\n");
    print("  <dt>" . __('Screenshot') . "</dt>\n");
    print("  <dd><a href=\"" . $screenshot_url . "\">" . $screenshot_url . "</a></dd>\n");
    print("  <dt>" . __('Icon') . "</dt>\n");
    print("  <dd><a href=\"" . $icon_url . "\">" . $icon_url . "</a></dd>\n");
    print("  <dt>" . __('PAD File') . "</dt>\n");
    print("  <dd><a href=\"" . $pad_url . "\">" . $pad_url . "</a></dd>\n");
    print("</dl>\n");

    $company_name = (string)$company_info->Company_Name;
    $company_website_url = (string)$company_info->Company_WebSite_URL;
    $contact_first_name = (string)$company_info->Contact_Info->Contact_First_Name;
    $contact_last_name = (string)$company_info->Contact_Info->Contact_Last_Name;
    $contact_email = (string)$company_info->Contact_Info->Contact_Email;
    $support_email = (string)$company_info->Support_Info->Support_Email;
    print("<h3>" . __('Company') . "</h3>\n");
    print("<dl>\n");
    print("  <dt>" . __('Name') . "</dt>\n");
    print("  <dd><a href=\"" . $company_website_url . "\">" . $company_name . "</a></dd>\n");
    print("  <dt>" . __('Contact') . "</dt>\n");
    print("  <dd><a href=\"mailto:" . $contact_email . "\">" . $contact_first_name . " " . $contact_last_name . "</a></dd>\n");
    print("  <dt>" . __('Support') . "</dt>\n");
    print("  <dd><a href=\"mailto:" . $support_email . "\">" . $support_email . "</a></dd>\n");
    print("</dl>\n");
    print("</div> <!-- .padfile -->\n");
  }

  $page->printFoot();
?>