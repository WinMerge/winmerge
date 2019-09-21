# User Manuals in DocBook/XML

Our Manuals are written in DocBook and we generate HTML and CHM manuals for distributing.

We have currently manuals in the following languages:

 * [English](EN)
 * [Japanese](JP)

## Useful DocBook resources

 * [Official DocBook homepage](https://www.docbook.org/)
 * [DocBook Wiki](https://github.com/docbook/wiki/wiki)

## Editing the manual

The user's manual is XML, so you can use XML editor or your favourite text editor.

**Note that XML requires document to be well formed, unlike HTML, before it can be processed.**

See these tutorials for editing DocBook:

 * [NewbieDoc DocBook guide](http://newbiedoc.sourceforge.net/metadoc/docbook-guide.html.en)
 * [Customising HTML output with DSSL](http://docbook.sourceforge.net/release/dsssl/current/doc/html/)
 * [Customising print formats with DSSL](http://docbook.sourceforge.net/release/dsssl/current/doc/print/)
 * [DocBook XSL: The Complete Guide](http://www.sagehill.net/docbookxsl/index.html)

### Important files

 * `{LANG}\_Copyrights.xml` *Update copyright information’s*
 * `{LANG}\_VersionNumbers.xml` *Update WinMerge version number and Manual revision*
 * `{LANG}\xsl\html.xsl` *Update WinMerge version number at parameter `headtitle.suffix`*

## Building the manual

Our preferred build system uses the following tools:

 * [xsltproc](http://xmlsoft.org/XSLT/)
 * [DocBook DTD](https://www.docbook.org/xml/)
 * [XSL StyleSheets](https://sourceforge.net/project/showfiles.php?group_id=21935&package_id=16608)
 * [HTML Help Compiler](https://www.microsoft.com/en-us/download/details.aspx?id=21138)

The easiest way is to download [our manual build tools package v2](https://github.com/WinMerge/winmerge/releases/download/winmerge_manual_another_build_tools_v2/winmerge_manual_another_build_tools_v2.zip). The package contains required tools for manual building.

Installing the tools is easy - just unzip the tools package to folder `\Docs\Manual\Tools`.

### Batch files

The batch files are located at the `\Docs\Manual` folder.

Every language has his one batch files. Paths to the installed tools are written in the `configuration.bat` file.

 * `build_html_en.bat`  
   Creates the English HTML files *without ads* under the folder `\Build\Manual\html-en`.

 * `build_html_en.bat withads`  
   Creates the English HTML files *with adds* under the folder `\Build\Manual\html-en`.

 * `build_htmlhelp_en.bat`  
   Creates the English CHM file under the folder `Build\Manual\htmlhelp`.

The Japanese batch files ends with `_jp` for example.
