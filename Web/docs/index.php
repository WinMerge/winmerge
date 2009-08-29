<?php
  include('../engine/engine.inc');

  $page = new Page;
  $page->setDescription(__('Documentation from WinMerge like manual, release notes, change log and Development Wiki.'));
  $page->setKeywords(__('WinMerge, documentation, manual, release notes, known issues, change log, Development Wiki'));
  $page->printHead(__('Documentation'), TAB_DOCS);

  $page->printHeading(__('Documentation'));
  $page->printLinkedSubHeading(__('Manual'), 'manual/');
  $page->printPara(__('The <a href="%s">manual</a> explains how to use WinMerge, and documents its capabilities and limitations.', 'manual/'));
  $page->printLinkedSubHeading(__('Release Notes'), 'releasenotes.php');
  $page->printPara(__('The <a href="%1$s">release notes</a> are a short summary of important changes, enhancements, bug fixes and <a href="%2$s">known issues</a> in the current WinMerge release.', 'releasenotes.php', 'releasenotes.php#issues'));
  $page->printLinkedSubHeading(__('Change Log'), 'changelog.php');
  $page->printPara(__('The <a href="%s">change log</a> is a more complete list of changes in the last WinMerge releases.', 'changelog.php'));
  $page->printLinkedSubHeading(__('Development Wiki'), '/Wiki/');
  $page->printPara(__('The <a href="%s">Development Wiki</a> contains much information about the WinMerge development.', '/Wiki/'));

  $page->printFoot();
?>