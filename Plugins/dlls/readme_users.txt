There are actually 3 different events.


Events
======
EDITOR_SCRIPT		In editor view, apply a function to the current selection

PREDIFF		     	Preprocess file before diffing : the plugin is not apply to the
			text displayed in the editor. It is applied only to a copy of the left
			and right texts, and this copy are then scanned to create the difference list.
			As now : you may delete one column, change the names of variables...
			         you may not add/delete/move lines.

PACK_UNPACK		Transform a file in a viewable format (for example, decompress a file...)
			The editor displays the unpacked data.
			Sometimes files may be packed again (zipped files...). An additionnal function is
			of course necessary.
			If the author of the function has created this function, file maybe be saved again in
			the compressed format.
			Else the file can only be saved in a text format. To avoid problems, you are proposed to
			change the filename when saving a changed file. 


Plugin selection
================
EDITOR_SCRIPT
In main menu, "Edit"->"Scripts"->


PACK_UNPACK

For directory view :
1) Check "Plugins"->"Automatic unpacking" in the menu.
Note : file are not compared again. You need to press F5 to update the results of the comparison.

For editor view : 
1) Open dialog : select the two files then the plugin. 
2) From the directory view when "Plugins"->"Automatic unpacking" is checked : select one file, double click to
open it. The plugin used during the directory comparison is used to open the editor.
3) From the directory view : select one file, then menu "Plugins"->"Edit with unpacker". An additionnal dialog
offers the choice of the plugin before opening the editor view. This plugin is forgotten when you close the editor view.

How works "Automatic unpacking" ?
Each plugin defines some file extensions it is intended for (see PluginFileFilters in the section "properties" in readme_developpers.txt).
This information is is used to automatically apply a plugin to a file.
Note : only the first matching plugin is applied
Note2 : the plugin must match the extension of either the right or the left file, and is then
applied to both files.

PREDIFF

For directory view :
1) Select one/several files
2) Right-click to open the context menu
3) "Plugin settings"->"Prediffer settings"->...
4) select "Auto prediffer" or "No prediffer"
Note : files are not compared again. You need to press F5 to update the results of the comparison.

For editor view :
1) From the directory view when "Auto prediffer" is checked for the file : select this file, double click to
open it. The plugin used during the directory comparison is used to open the editor.
2) During session, menu "Plugins"->"Prediffer"->... The active prediffer is checked.
Plugins are shared in two categories : 
- suggested plugins : current files match the PluginFileFilters (see below)
- other plugins
Note : the files are compared again after the selection. No need to press F5.

How works "Auto prediffer" ?
Same rule as for "Automatic unpacking".

