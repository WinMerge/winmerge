<?php
  include('../page.inc');

  $page = new Page;
  $page->setDescription('Tracker Shortcuts are shorter and nicer URLs to Tracker items from our SF.net project page. And you can use this page to create a shortcut.');
  $page->setKeywords('WinMerge, tracker, shortcuts, sourceforge, project page, bug, patch, support request, feature request, todo item');
  $page->printHead('Tracker Shortcuts', TAB_SUPPORT);
?>
<h2>Tracker Shortcuts</h2>
<p><em>Tracker Shortcuts</em> are shorter and nicer URLs to <a href="http://sourceforge.net/tracker/?group_id=13216">Tracker</a>
items from our <a href="http://sourceforge.net/projects/winmerge">SF.net project page</a>.</p>
<p>You can use this page to create a shortcut:</p>
<?php
  $types['bug'] = 'Bug';
  $types['patch'] = 'Patch';
  $types['support'] = 'Support Request';
  $types['rfe'] = 'Feature Request';
  $types['todo'] = 'Todo Item';
  $types['tracker'] = 'Tracker Item';
?>
<form action="tracker-shortcuts.php" method="post">
<table>
  <tr>
    <td><label for="type">Type:</label></td>
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
      <input type="submit" name="submit" value="Generate" />
    </td>
  </tr>
  <tr>
    <td><label for="id">ID:</label></td>
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
?>
<h3>Subdomains</h3>
<ul class="rssfeeditems">
  <li><a href="http://bugs.winmerge.org/">bugs.winmerge.org</a></li>
  <li><a href="http://forums.winmerge.org/">forums.winmerge.org</a></li>
  <li><a href="http://lists.winmerge.org/">lists.winmerge.org</a></li>
  <li><a href="http://patches.winmerge.org/">patches.winmerge.org</a></li>
  <li><a href="http://project.winmerge.org/">project.winmerge.org</a></li>
  <li><a href="http://wiki.winmerge.org/">wiki.winmerge.org</a></li>
</ul>
<?php
  $page->printFoot();
?>