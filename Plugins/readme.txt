There are actually 4 API, and 3 different uses. 
Each API is designed by its event name : FILE_PACK_UNPACK, BUFFER_PACK_UNPACK, PREDIFFING, CONTEXT_MENU.

FILE_PACK_UNPACK	Handle a given file type, as we would for a zipped file
			Merging session : unpack the file to load it in display buffer, and repack it when the user 
			 saves a modified file. File is unpacked in the display buffer, so is it when diffing too.
			Directory comparison : WinMerge takes care to unpack files (see mode below).
			The unpacker plugin is memorized, and its packing function is used when saving. One integer 
			 value may be passed from the unpacker to the packer (subcode).
BUFFER_PACK_UNPACK	As FILE_PACK_UNPACK, but different interface

PREDIFFING		Preprocess one file before diffing
			Merging session : the preprocessing occurs on a copy of the display buffer. 
			 So it does not modify the file being merged.
			Directory comparison : WinMerge takes care to preprocess files.
			As now : you may delete one column, change the names of variables... 
			         you may not add/delete/move lines.

CONTEXT_MENU		For WinMerge editor, apply a function to the selection or at the cursor point
			Right-click in the editor to access a list of functions



For some events (FILE_PACK_UNPACK, BUFFER_PACK_UNPACK, PREDIFFING), plugins are automatically applied 
or chosen by the user :

automatic mode : WinMerge looks for the right plugin. The allowed plugins must be automatic,
and must recognize the file extension (see below for properties). 
For PACK_UNPACK, WinMerge applies the first plugin which matches these conditions. 
For PREDIFFING, WinMerge applies all the plugins that match these conditions (a first plugin
may delete the first column, and a second one may transform some variable names).

manual mode : open two files, and select the plugin in the open dialog. Or compare two directories,
select one file and in the menu 'Plugins'->'Edit with unpacker'.

TODO : Prediffing plugins may not yet be selected in manual mode.				





A plugin must expose properties :

property name		mandatory ?				events

PluginEvent 		yes					all
PluginDescription 	no					all
PluginFileFilters 	no					FILE_PACK_UNPACK, BUFFER_PACK_UNPACK, PREDIFFING
PluginIsAutomatic 	if PluginFileFilters is defined		FILE_PACK_UNPACK, BUFFER_PACK_UNPACK, PREDIFFING



and methods :

events			method name		mandatory ?

CONTEXT_MENU		Free function name	At least one

PREDIFFING		DiffingPreprocessW 	Mandatory (for VB, this one is easy to program)
			DiffingPreprocessA	Optional  (allow to rescan faster for non-unicode files)

FILE_PACK_UNPACK	UnpackFile		Optional 
			PackFile		Optional

BUFFER_PACK_UNPACK	UnpackBufferA		Optional
			PackBufferA    		Optional







