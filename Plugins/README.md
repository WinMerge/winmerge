# WinMerge Plugins

This folders holds the WinMerge runtime plugins, source code and binaries both.

These filters are distributed in the `MergePlugins` subdirectory beneath the WinMerge executables.

## Quick description

Plugins use an ActiveX interface. Plugins may be written in any format that supports this interface.

Examples are available in :

 * C++ COM component
 * VB ActiveX dll
 * Scriptlets (VBScript, JScript)
 * Delphi

Limitation : Scriptlets only work for `EDITOR_SCRIPT` plugins.

**Properties** are used to present information concerning the plugin.  
**Methods** are used to process the data. Method names and syntax depends on events and **API** (see below).

## Events

### `EDITOR_SCRIPT`

In editor view, apply a function to the current selection.

### `PREDIFF`

Preprocess file before diffing : the plugin is not apply to the text displayed in the editor.
It is applied only to a copy of the left and right texts, and this copy are then scanned to create the difference list.
As now:

 * you may delete one column, change the names of variables...
 * you may not add/delete/move lines.

### `PACK_UNPACK`

Transform a file in a viewable format (for example, decompress a file...)

 * The editor displays the unpacked data.
 * Sometimes files may be packed again (zipped files...). An additional function is of course necessary.
 * If the author of the plugin created this function, file may be saved again in the compressed format.
 * Else the file can only be saved in a text format. To avoid problems, you are proposed to change the filename when saving a changed file.

## API

Some events have two API. One to exchange the data through a `BSTR` (memory) and one through input/ouput files.

| Event                | Data exchange                                                                                            |
|:---------------------|:---------------------------------------------------------------------------------------------------------|
| `FILE_PREDIFF`       | data are exchanged through an input and an output file                                                   |
| `BUFFER_PREDIFF`     | data are exchanged through a `BSTR`                                                                      |
| `FILE_PACK_UNPACK`   | data are exchanged through an input and an output file                                                   |
| `BUFFER_PACK_UNPACK` | data are exchanged through a `SafeArray` (`BSTR` not available as the packed data are possibly not text) |
| `EDITOR_SCRIPT`      | data are exchanged through a `BSTR`                                                                      |

You need to define only one API to handle an event. Define the one you prefer.

## Properties

| Name                | Mandatory                         | Events                   |
|:--------------------|:----------------------------------|:-------------------------|
| `PluginEvent`       | yes                               | all                      |
| `PluginDescription` | no                                | all                      |
| `PluginFileFilters` | no                                | `PACK_UNPACK`, `PREDIFF` |
| `PluginIsAutomatic` | if `PluginFileFilters` is defined | `PACK_UNPACK`, `PREDIFF` |

`PluginIsAutomatic` and `PluginFileFilters` are for automatic mode :

 * When `PluginIsAutomatic` is `false`, the plugin is never used in automatic mode.
 * When `PluginIsAutomatic` is `true`, `PluginFileFilters` is compared to the filename of both files. If one file matches the filter, the plugin is applied.

## Methods

| API                  | Method name                                                                                    |
|:---------------------|:-----------------------------------------------------------------------------------------------|
| `EDITOR_SCRIPT`      | function name is free **Note**: several functions may be defined in one `EDITOR_SCRIPT` plugin |
| `BUFFER_PREDIFF`     | `PrediffBufferW`                                                                               |
| `FILE_PREDIFF`       | `PrediffFile`                                                                                  |
| `BUFFER_PACK_UNPACK` | `UnpackBufferA`, `PackBufferA`                                                                 |
| `FILE_PACK_UNPACK`   | `UnpackFile`, `PackFile`                                                                       |

**Note**: `PACK_UNPACK` functions use an additional parameter. The value may be set during `UnpackBuffer`.  
When file is changed, the value is forwarded to `PackBuffer`. The goal is to pass a parameter from `UnpackBuffer` to `PackBuffer`.  

For example, the plugin may handle several compressed formats, and use this value to recompress a file in the format of the original.  
This parameter is mandatory for the function's syntax. But you don't have to set its value when you don't use it.

## Syntax

### Properties syntax

#### `PluginEvent`

| Language | Syntax                                                              |
|:--------:|:--------------------------------------------------------------------|
| C++      | `STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR * pVal)`        |
| VB       | `Public Property Get PluginEvent() As String`                       |
| VBScript | `Function get_PluginEvent()`                                        |
| JScript  | `function get_PluginEvent()`                                        |

#### `PluginDescription`

| Language | Syntax                                                              |
|:--------:|:--------------------------------------------------------------------|
| C++      | `STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR * pVal)`  |
| VB       | `Public Property Get PluginDescription() As String`                 |
| VBScript | `Function get_PluginDescription()                                   |
| JScript  | `function get_PluginDescription()`                                  |

#### `PluginFileFilters`

String formed of fileFilters, separated with `;`

| Language | Syntax                                                              |
|:--------:|:--------------------------------------------------------------------|
| C++      | `STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR * pVal)`  |
| VB       | `Public Property Get PluginFileFilters() As String`                 |
| VBScript | `Function get_PluginFileFilters()                                   |
| JScript  | `function get_PluginFileFilters()`                                  |

#### `PluginIsAutomatic`

| Language | Syntax                                                                        |
|:--------:|:------------------------------------------------------------------------------|
| C++      | `STDMETHODIMP CWinMergeScript::get_PluginIsAutomatic(VARIANT_BOOL * pVal)`    |
| VB       | `Public Property Get PluginIsAutomatic() As Boolean`                          |
| VBScript | `Function get_PluginIsAutomatic()`                                            |
| JScript  | `function get_PluginIsAutomatic()`                                            |

### Methods syntax

#### `EDITOR_SCRIPT`

| Language | Functions parameters (function names are free)                                |
|:--------:|:------------------------------------------------------------------------------|
| C++      | `STDMETHOD(MakeUpper)([in] BSTR inputText, [out, retval] BSTR * outputText);` |
| VB       | `Public Function MakeUpper(text As String)`                                   |
| VBScript | `Function MakeUpper(Text)`                                                    |
| JScript  | `function MakeUpper(Text)`                                                    |

#### `FILE_PREDIFF`

| Language | Functions names               | Functions parameters                                                                                                       |
|:--------:|:------------------------------|:---------------------------------------------------------------------------------------------------------------------------|
| VC++     | `STDMETHOD(PrediffFile)`      | `([in] BSTR fileSrc, [in] BSTR fileDst, VARIANT_BOOL * pbChanged, INT * pSubcode, [out, retval] VARIANT_BOOL * pbSuccess)` |
| VB       | `Public Function PrediffFile` | `(BSTR fileSrc, BSTR fileDst, ByRef bChanged As Boolean, ByRef subcode As Long) As Boolean`                                |
| VBScript | `Function PrediffFile`        | `(fileSrc, fileDst, bChanged, subcode)`                                                                                    |
| JScript  | `function PrediffFile`        | `(fileSrc, fileDst, bChanged, subcode) { ...; var r = new ActiveXObject("Scripting.Dictionary"); r.Add(0, retval); r.Add(1, bChanged); r.Add(2, subcode); return r.Items(); }` |

#### `BUFFER_PREDIFF`

| Language | Functions names                  | Functions parameters                                                                                            |
|:--------:|:---------------------------------|:----------------------------------------------------------------------------------------------------------------|
| C++      | `STDMETHOD(PrediffBufferW)`      | `([in] BSTR * pText, [in] INT * pSize, [in] VARIANT_BOOL * pbChanged, [out, retval] VARIANT_BOOL * pbHandled);` |
| VB       | `Public Function PrediffBufferW` | `(ByRef text As String, ByRef size As Long, ByRef bChanged As Boolean) As Boolean`                              |
| VBScript | `Function PrediffBufferW`        | `(text, size, bChanged)`                                                                                    |
| JScript  | `function PrediffBufferW`        | `(text, size, bChanged) { ...; var r = new ActiveXObject("Scripting.Dictionary"); r.Add(0, retval); r.Add(1, text); r.Add(2, size); r.Add(3, bChanged); return r.Items(); }` |

#### `FILE_PACK_UNPACK`

| Language | Functions names              | Functions parameters                                                                                                       |
|:--------:|:-----------------------------|:---------------------------------------------------------------------------------------------------------------------------|
| VC++     | `STDMETHOD(UnpackFile)`      | `([in] BSTR fileSrc, [in] BSTR fileDst, VARIANT_BOOL * pbChanged, INT * pSubcode, [out, retval] VARIANT_BOOL * pbSuccess)` |
| VC++     | `STDMETHOD(PackFile)`        | `([in] BSTR fileSrc, [in] BSTR fileDst, VARIANT_BOOL * pbChanged, INT pSubcode, [out, retval] VARIANT_BOOL * pbSuccess)`   |
| VB       | `Public Function UnpackFile` | `(BSTR fileSrc, BSTR fileDst, ByRef bChanged As Boolean, ByRef subcode As Long) As Boolean`                                |
| VB       | `Public Function PackFile`   | `(BSTR fileSrc, BSTR fileDst, ByRef bChanged As Boolean, subcode As Long) As Boolean`                                      |
| VBScript | `Function UnpackFile`        | `(fileSrc, fileDst, bChanged, subcode)`                                                                                    |
| VBScript | `Function PackFile`          | `(fileSrc, fileDst, bChanged, subcode)`                                                                                    |
| JScript  | `function UnpackFile`        | `(fileSrc, fileDst, bChanged, subcode) { ...; var r = new ActiveXObject("Scripting.Dictionary"); r.Add(0, retval); r.Add(1, bChanged); r.Add(2, subcode); return r.Items(); }` |
| JScript  | `function PackFile`          | `(fileSrc, fileDst, bChanged, subcode) { ...; var r = new ActiveXObject("Scripting.Dictionary"); r.Add(0, retval); r.Add(1, bChanged); return r.Items(); }` |

#### `BUFFER_PACK_UNPACK`

| Language | Functions names                 | Functions parameters                                                                                                                        |
|:--------:|:--------------------------------|:--------------------------------------------------------------------------------------------------------------------------------------------|
| VC++     | `STDMETHOD(UnpackBufferA)`      | `([in] SAFEARRAY ** pBuffer, [in] INT * pSize, [in] VARIANT_BOOL * pbChanged, [in] INT * pSubcode, [out, retval] VARIANT_BOOL * pbSuccess)` |
| VC++     | `STDMETHOD(PackBufferA)`        | `([in] SAFEARRAY ** pBuffer, [in] INT * pSize, [in] VARIANT_BOOL * pbChanged, [in] INT subcode, [out, retval] VARIANT_BOOL * pbSuccess)`    |
| VB       | `Public Function UnpackBufferA` | `(ByRef buffer() As Byte, ByRef size As Long, ByRef bChanged As Boolean, ByRef subcode As Long) As Boolean`                                 |
| VB       | `Public Function PackBufferA`   | `(ByRef buffer() As Byte, ByRef size As Long, ByRef bChanged As Boolean, subcode As Long) As Boolean`                                       |

## How to write plugins quickly ?

Easiest plugins are scriptlets.

Just VBscript (or JavaScript probably) with an additional section `<implement>`. See examples.  
But they are difficult to debug. And valid only for `EDITOR_SCRIPT` events.

### VC++ plugins

The most difficult to write when you do it from scratch. See in `Plugins/syntax.txt`, there are three additional steps from normal COM dll.

But easy to write from an existing plugin.

 1. Select a C++ plugin with the same API
 2. Rename the files `cpp,def,dsp,idl,rc` : replace \[_name of old plugin_\] with \[_name of your plugin_\]
 3. In all the files, replace all instances of \[_name of old plugin_\] with \[_name of your plugin_\]
 4. Write your custom code : `WinMergeScript.cpp` holds all the important functions.
 5. Generate new GUIDs and add to the `.idl` file.

### Additional steps to write a plugin in C++

 * do not register the dll : delete everything in 'settings'->'custom build'
 * do not register the dll : delete the file `.rgs`, and the registry section in the file `.rc`
 * do not register the dll : add `typeinfoex.h` + and make 3 changes in `WinMergeScript.h` (see commented lines)
 * `SAFEARRAY` : replace the interface in `.idl` :
   * `SAFEARRAY *` `SAFEARRAY(unsigned char)`
   * `SAFEARRAY **` `SAFEARRAY(unsigned char) *`

## How to debug VC++ plugins ?

Easy with Visual Studio after you installed WinMerge source.

### `EDITOR_SCRIPT`

 1. Set a breakpoint at the beginning of `safeInvokeA` in `Plugins.cpp`.
 2. Run WinMerge
 3. Do all you need (open file, menu...) to call the plugin.
 4. The breakpoint is triggered. The plugin interface is loaded at this moment. Open the file `WinMergeScript.cpp` source of your plugin in the debugging session.
 5. Set a breakpoint in this file at the beginning of your function.
 6. `F5`. The breakpoint in your function is triggered.

### `PREDIFF`, `PACK_UNPACK`

Same steps, point #1 only differs:

 1. Set a breakpoint at the beginning of `safeInvokeW` in `Plugins.cpp`.  
    
    **Note**: `safeInvokeW` instead of `safeInvokeA`.
