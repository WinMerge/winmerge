There are actually 3 different events (uses) and 5 different API.
Each API is designed by its name. This name defines the event and the format of the data.


Events
======
PREDIFF		     	Preprocess one file before diffing : the plugin is not apply to the
			text displayed in the editor. It is applied only to a copy of the left
			and right texts, and this copy are then scanned to create the difference list.
			As now : you may delete one column, change the names of variables...
			         you may not add/delete/move lines.

PACK_UNPACK		Transform a file in a viewable format (for example, decompress a file...)
			The editor displays the unpacked data.
			The unpacker plugin is memorized, and the packing function is used when saving. One integer
			 value may be passed from the unpacker to the packer (subcode).

CONTEXT_MENU		For WinMerge editor, apply a function to the current selection


API
===
FILE_PREDIFF	    	data are exchanged through an input and an output file
BUFFER_PREDIFF	    	data are exchanged through a BSTR
FILE_PACK_UNPACK	data are exchanged through an input and an output file
BUFFER_PACK_UNPACK      data are exchanged through a SafeArray (BSTR not available as the packed data are
    			possibly not text)
CONTEXT_MENU	    	data are exchanged through a BSTR


Plugin selection
================
CONTEXT_MENU
Right-click in the editor to access a list of functions

PACK_UNPACK and PREDIFF
Two modes are available

manual mode : open two files, and select the plugin in the open dialog. Or compare two directories,
select one file and in the menu 'Plugins'->'Edit with unpacker' or 'Plugins'->'Edit with prediffer'

TODO : 'Plugins'->'Edit with prediffer' and allow changing prediffer without leaving the session

automatic mode : each plugin may define some file extensions it is intended for (see properties below).
This information is is used to automatically apply a plugin to a file.
Note : only the first matching plugin is applied
Note2 : the plugin must match the extension of either the right or the left file, and is then
applied to both files.


Properties
==========
name	  		mandatory				events

PluginEvent 		yes					all
PluginDescription 	no					all
PluginFileFilters 	no					PACK_UNPACK, PREDIFF
PluginIsAutomatic 	if PluginFileFilters is defined		PACK_UNPACK, PREDIFF


Methods
=======
API			method name

CONTEXT_MENU		function name is free	    Note : several functions may be defined in one plugin

BUFFER_PREDIFF		PrediffBufferW
FILE_PREDIFF            PrediffFile

BUFFER_PACK_UNPACK      UnpackBufferA
                        PackBufferA
FILE_PACK_UNPACK        UnpackFile
			PackFile





