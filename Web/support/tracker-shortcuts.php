<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Tracker Shortcuts', WINMERGE_SUP);
?>
<h2>Tracker Shortcuts</h2>
<p><em>Tracker Shortcuts</em> are shorter and nicer URLs to <a href="http://sourceforge.net/tracker/?group_id=13216">Tracker</a>
items from our <a href="http://sourceforge.net/projects/winmerge">SF.net project page</a>.</p>
<p>You can use this page, to create a shortcut:</p>
<?php
  $types['bug'] = 'Bug';
  $types['patch'] = 'Patch';
  $types['support'] = 'Support Request';
  $types['rfe'] = 'Feature Request';
  $types['todo'] = 'Todo Item';
?>
<form action="tracker-shortcuts.php" method="post">
<table>
  <tr>
    <td><label for="type">Type:</label></td>
    <td>
      <select name="type" id="type" size="1">
<?php
  foreach ($types as $value => $name) { //For all types...
    if ($value == $_POST['type']) $selected = ' selected="selected"'; else $selected = '';
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
      <input type="text" name="id" id="id" size="8" maxlength="8" value="<?php echo htmlentities($_POST['id']); ?>" />
    </td>
  </tr>
</table>
</form>
<?php
  if (isset($_POST['type']) && ($_POST['type'] != '') && isset($_POST['id']) && ($_POST['id'] > 0)) {
    echo "<p />\n";
    echo "<p style=\"font-size:larger;\"><code>http://winmerge.org/" . htmlentities($_POST['type']) . "/" . htmlentities($_POST['id']) . "</code></p>\n";
    echo "<p />\n";
  }
  $page->printFoot();
?>