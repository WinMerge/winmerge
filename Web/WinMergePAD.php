<?php
  include('page.inc');

  $page = new Page;
  $page->printHead('WinMerge: PAD file', WINMERGE_USR);
?>
<h2>PAD file</h2>
<p>The easiest way to get informations about the latest stable version from WinMerge for your web site is the <a href="http://www.asp-shareware.org/pad/">Portable Application Description</a> file, which you find here:</p>
<pre class="code"><a href="http://winmerge.org/WinMergePAD.xml">http://winmerge.org/WinMergePAD.xml</a></pre>
<p>If you can't work with the <a href="WinMergePAD.xml">PAD file</a>, you can use the following informations:</p>
<?php
  $padfile = $page->convertXml2Array('WinMergePAD.xml');
  $company_info = $padfile['XML_DIZ_INFO'][0]['COMPANY_INFO'][0];
  $program_info = $padfile['XML_DIZ_INFO'][0]['PROGRAM_INFO'][0];
  $file_info = $program_info['FILE_INFO'][0];
  $program_descriptions = $padfile['XML_DIZ_INFO'][0]['PROGRAM_DESCRIPTIONS'][0]['ENGLISH'][0];
  $web_info = $padfile['XML_DIZ_INFO'][0]['WEB_INFO'][0];

  $program_name = $program_info['PROGRAM_NAME'][0]['VALUE'];
  $program_version = $program_info['PROGRAM_VERSION'][0]['VALUE'];
  $release_date_month = $program_info['PROGRAM_RELEASE_MONTH'][0]['VALUE'];
  $release_date_day = $program_info['PROGRAM_RELEASE_DAY'][0]['VALUE'];
  $release_date_year = $program_info['PROGRAM_RELEASE_YEAR'][0]['VALUE'];
  $release_status = $program_info['PROGRAM_RELEASE_STATUS'][0]['VALUE'];
  $download = $web_info['DOWNLOAD_URLS'][0]['PRIMARY_DOWNLOAD_URL'][0]['VALUE'];
  $file_size_mb = $file_info['FILE_SIZE_MB'][0]['VALUE'];
  $program_os_support = $program_info['PROGRAM_OS_SUPPORT'][0]['VALUE'];
  $program_language = $program_info['PROGRAM_LANGUAGE'][0]['VALUE'];
  $program_type = $program_info['PROGRAM_TYPE'][0]['VALUE'];
  print("<h2>" . $program_name . " " . $program_version . "</h2>\n");
  print("<dl>\n");
  print("  <dt>Release Date:</dt>\n");
  print("  <dd>" . $release_date_month . "/" . $release_date_day . "/" . $release_date_year . "</dd>\n");
  print("  <dt>Release Status:</dt>\n");
  print("  <dd>" . $release_status . "</dd>\n");
  print("  <dt>Download:</dt>\n");
  print("  <dd><a href=\"" . $download . "\">" . $download . "</a> (" . $file_size_mb . " MB)</dd>\n");
  print("  <dt>Operating Systems:</dt>\n");
  print("  <dd>" . str_replace(',', ', ', $program_os_support) . "</dd>\n");
  print("  <dt>Supported Languages:</dt>\n");
  print("  <dd>" . str_replace(',', ', ', $program_language) . "</dd>\n");
  print("  <dt>Type:</dt>\n");
  print("  <dd>" . $program_type . "</dd>\n");
  print("</dl>\n");

  $description_80 = $program_descriptions['CHAR_DESC_80'][0]['VALUE'];
  $description_250 = $program_descriptions['CHAR_DESC_250'][0]['VALUE'];
  $description_450 = $program_descriptions['CHAR_DESC_450'][0]['VALUE'];
  $keywords = $program_descriptions['KEYWORDS'][0]['VALUE'];
  print("<h3>Descriptions</h3>\n");
  print("<dl>\n");
  print("  <dt>Description (80 chars):</dt>\n");
  print("  <dd>" . $description_80 . "</dd>\n");
  print("  <dt>Description (250 chars):</dt>\n");
  print("  <dd>" . $description_250 . "</dd>\n");
  print("  <dt>Description (450 chars):</dt>\n");
  print("  <dd>" . $description_450 . "</dd>\n");
  print("  <dt>Keywords:</dt>\n");
  print("  <dd>" . str_replace(',', ', ', $keywords) . "</dd>\n");
  print("</dl>\n");

  $info_url = $web_info['APPLICATION_URLS'][0]['APPLICATION_INFO_URL'][0]['VALUE'];
  $screenshot_url = $web_info['APPLICATION_URLS'][0]['APPLICATION_SCREENSHOT_URL'][0]['VALUE'];
  $icon_url = $web_info['APPLICATION_URLS'][0]['APPLICATION_ICON_URL'][0]['VALUE'];
  $pad_url = $web_info['APPLICATION_URLS'][0]['APPLICATION_XML_FILE_URL'][0]['VALUE'];
  print("<h3>Web Addresses</h3>\n");
  print("<dl>\n");
  print("  <dt>Information:</dt>\n");
  print("  <dd><a href=\"" . $info_url . "\">" . $info_url . "</a></dd>\n");
  print("  <dt>Screenshot:</dt>\n");
  print("  <dd><a href=\"" . $screenshot_url . "\">" . $screenshot_url . "</a></dd>\n");
  print("  <dt>Icon:</dt>\n");
  print("  <dd><a href=\"" . $icon_url . "\">" . $icon_url . "</a></dd>\n");
  print("  <dt>PAD file:</dt>\n");
  print("  <dd><a href=\"" . $pad_url . "\">" . $pad_url . "</a></dd>\n");
  print("</dl>\n");

  $company_name = $company_info['COMPANY_NAME'][0]['VALUE'];
  $company_website_url = $company_info['COMPANY_WEBSITE_URL'][0]['VALUE'];
  $contact_first_name = $company_info['CONTACT_INFO'][0]['CONTACT_FIRST_NAME'][0]['VALUE'];
  $contact_email = $company_info['CONTACT_INFO'][0]['CONTACT_EMAIL'][0]['VALUE'];
  $support_email = $company_info['SUPPORT_INFO'][0]['SUPPORT_EMAIL'][0]['VALUE'];
  print("<h3>Company</h3>\n");
  print("<dl>\n");
  print("  <dt>Name:</dt>\n");
  print("  <dd><a href=\"" . $company_website_url . "\">" . $company_name . "</a></dd>\n");
  print("  <dt>Contact:</dt>\n");
  print("  <dd><a href=\"mailto:" . $contact_email . "\">" . $contact_first_name . "</a></dd>\n");
  print("  <dt>Support:</dt>\n");
  print("  <dd><a href=\"mailto:" . $support_email . "\">" . $support_email . "</a></dd>\n");
  print("</dl>\n");

  $page->printFoot();
?>