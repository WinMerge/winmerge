
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


How to write plugins quickly ?
==============================
Easiest plugins are scriptlets.
Just VBscript (or javascript probably) with an additionnal section <implement>. See examples.
But they are difficult to debug. And valid only for EDITOR_SCRIPT events.

VC++ plugins :
The most difficult to write when you do it from scratch. See in syntax.txt, there are three additionnal steps from normal COM dll.
But easy to write from an existing plugin.
1) Select a C++ plugin with the same API
2) Rename the files cpp,def,dsp,idl,rc : replace [name of old plugin] with [name of your plugin]
3) In all the files, replace all instances of [name of old plugin] with [name of your plugin]
4) Write your custom code : WinMergeScript.cpp holds all the important functions.


How to debug VC++ plugins ?
===========================
Easy with Visual Studio after you installed WinMerge source.

EDITOR_SCRIPT : 
1) Set a breakpoint at the beginning of safeInvokeA in Plugins.cpp.
2) Run WinMerge
3) Do all you need (open file, menu...) to call the plugin.
4) The breakpoint is triggered. The plugin interface is loaded at this moment. Open the file WinMergeScript.cpp
source of your plugin in the debugging session.
5) Set a breakpoint in this file at the beginning of your function.
6) F5. The breakpoint in your function is triggered.

PREDIFF, PACK_UNPACK : 
Same steps, point #1 only differs
1) Set a breakpoint at the beginning of safeInvokeW in Plugins.cpp.
note : safeInvokeW instead of safeInvokeA.
