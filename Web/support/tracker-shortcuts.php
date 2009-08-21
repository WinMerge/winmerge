<?php
  include('../engine/engine.inc');

  $page = new Page;
  $page->setDescription(__('Tracker Shortcuts are shorter and nicer URLs to Tracker items from our SF.net project page. And you can use this page to create a shortcut.'));
  $page->setKeywords(__('WinMerge, tracker, shortcuts, sourceforge, project page, bug, patch, support request, feature request, todo item'));
  $page->printHead(__('Tracker Shortcuts'), TAB_SUPPORT);

  $page->printHeading(__('Tracker Shortcuts'));
  $page->printPara(__('<em>Tracker Shortcuts</em> are shorter and nicer URLs to <a href="%1$s">Tracker</a> items from our <a href="%2$s">SF.net project page</a>.', 'http://sourceforge.net/tracker/?group_id=13216', 'http://sourceforge.net/projects/winmerge'));
  $page->printPara(__('You can use this page to create a shortcut:'));

  $types['bug'] = __('Bug');
  $types['patch'] = __('Patch');
  $types['support'] = __('Support Request');
  $types['rfe'] = __('Feature Request');
  $types['todo'] = __('Todo Item');
  $types['tracker'] = __('Tracker Item');
?>
<form action="tracker-shortcuts.php" method="post">
<table>
  <tr>
    <td><label for="type"><?php __e('Type:');?></label></td>
    <td>
      <select name="type" id="type" size="1">
<?php
  $post_type = isset($_POST['type']) ? $_POST['type'] : '';
  $post_id = isset($_POST['id']) ? $_POST['id'] : '';
  foreach ($types as $value => $name) { //For all types...
    if ($value == $post_type) $selected = ' selected="selected"'; else $selected = '';
    echo "        <option value=\"" . $value . "\"" . $selected . ">" . $name . "</option>\n";
  }
?>
      </select>
    </td>
    <td rowspan="2" valign="bottom">
      <input type="submit" name="submit" value="<?php __e('Generate');?>" />
    </td>
  </tr>
  <tr>
    <td><label for="id"><?php __e('ID:');?></label></td>
    <td>
      <input type="text" name="id" id="id" size="8" maxlength="8" value="<?php echo htmlentities($post_id); ?>" />
    </td>
  </tr>
</table>
</form>
<?php
  if (($post_type != '') && ($post_id > 0)) {
    echo "<p />\n";
    echo "<pre class=\"code\">http://winmerge.org/" . htmlentities($post_type) . "/" . htmlentities($post_id) . "</pre>\n";
    echo "<p />\n";
  }

  $page->printSubHeading(__('Subdomains'));
?>
<ul class="rssfeeditems">
  <li><a href="http://bugs.winmerge.org/">bugs.winmerge.org</a></li>
  <li><a href="http://feature-requests.winmerge.org/">feature-requests.winmerge.org</a></li>
  <li><a href="http://forums.winmerge.org/">forums.winmerge.org</a></li>
  <li><a href="http://lists.winmerge.org/">lists.winmerge.org</a></li>
  <li><a href="http://patches.winmerge.org/">patches.winmerge.org</a></li>
  <li><a href="http://project.winmerge.org/">project.winmerge.org</a></li>
  <li><a href="http://wiki.winmerge.org/">wiki.winmerge.org</a></li>
</ul>
<?php
  $page->printFoot();
?>