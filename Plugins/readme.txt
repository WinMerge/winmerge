There are actually 4 API, and 3 different uses. 
Each API is designed by its event name : FILE_PACK_UNPACK, BUFFER_PACK_UNPACK, PREDIFFING, CONTEXT_MENU.

FILE_PACK_UNPACK	Handle a given file type, 
			for example, unzip a file before loading
			Called when load (open) the file, and when saving
			The unpacker plugin is memorized, and its packing function is used when saving.
			One integer value may be passed from the unpacker to the packer (subcode).
BUFFER_PACK_UNPACK	As FILE_PACK_UNPACK, but different interface

PREDIFFING		preprocess the text before diffing, 
			As now : you may delete one column, change one variable name 
			         you may not add/delete/move lines
			Called before comparing files 
			Either comparing files in directory : the original file is preprocessed
			Or compare files being merged       : the modified text is preprocessed

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







