
Events 
======
Please read first readme_users.txt

Plugin selection
================
Please read first readme_users.txt


Quick description
=================
Plugins use an ActiveX interface.
Plugins may be written in any format that supports this interface. Examples are available in :
	C++ COM component
	VB ActiveX dll
	scriptlets (VBS,JS)
	Delphi

Properties are used to present information concerning the plugin.
Methods are used to process the data. Method names and syntax depends on events and API (see below).

Properties
==========
name	  		mandatory				events

PluginEvent 		yes					all
PluginDescription 	no					all
PluginFileFilters 	no					PACK_UNPACK, PREDIFF
PluginIsAutomatic 	if PluginFileFilters is defined		PACK_UNPACK, PREDIFF

PluginIsAutomatic and PluginFileFilters are for automatic mode :

When PluginIsAutomatic is false, the plugin is never used in automatic mode.
When PluginIsAutomatic is true, PluginFileFilters is compared to the filename of both files. If one file
matches the filter, the plugin is applied.

API
===
Some events have two API. 
One to exchange the data through a BSTR (memory) and one through input/ouput files.

FILE_PREDIFF	    	data are exchanged through an input and an output file
BUFFER_PREDIFF	    	data are exchanged through a BSTR
FILE_PACK_UNPACK	data are exchanged through an input and an output file
BUFFER_PACK_UNPACK      data are exchanged through a SafeArray (BSTR not available as the packed data are
    			possibly not text)
EDITOR_SCRIPT	    	data are exchanged through a BSTR

You need to define only one API to handle an event. Define the one you prefer.

Methods
=======
API			method name

EDITOR_SCRIPT		function name is free	    

Note : several functions may be defined in one EDITOR_SCRIPT plugin

BUFFER_PREDIFF		PrediffBufferW
FILE_PREDIFF            PrediffFile

BUFFER_PACK_UNPACK      UnpackBufferA
                        PackBufferA
FILE_PACK_UNPACK        UnpackFile
			PackFile

Note : PACK_UNPACK functions use an additionnal parameter. The value may be set during UnpackBuffer.
When file is changed, the value is forwarded to PackBuffer.
The goal is to pass a parameter from UnpackBuffer to PackBuffer.
For example, the plugin may handle several compressed formats, and use this value to recompress a file in
the format of the original.
This parameter is mandatory for the functions syntax. But you don't have to set its value when you don't use it.

Syntax
======
See syntax.txt
