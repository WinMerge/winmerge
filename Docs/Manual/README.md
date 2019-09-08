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

## Building the manual

Our preferred build system uses the following tools:

 * [Java Runtime Environment](https://www.java.com/)
 * [Saxon](http://saxon.sourceforge.net/)
 * [DocBook DTD](https://www.docbook.org/xml/)
 * [XSL StyleSheets](https://sourceforge.net/project/showfiles.php?group_id=21935&package_id=16608)
 * [HTML Help Compiler](https://www.microsoft.com/en-us/download/details.aspx?id=21138)

The easiest way is to download [our manual build tools package v1](https://sourceforge.net/project/showfiles.php?group_id=13216&package_id=284332&release_id=614099).

The package contains required tools for manual building. [Java Runtime (JRE)](https://www.java.com/) is the only external requirement.

Installing the tools is easy - just unzip the tools package to folder `/Docs/Manual/{LANG}/build`.

### Batch files

The batch files are also located at the same `build` subfolder.

The main file is `build.bat`. Paths to the installed tools are written in the `configuration.bat` file.

 * `build.bat html` or `build_html.bat`  
   Creates the HTML files under the `html` subdirectory at the `build` directory.

 * `build.bat html withads` or `build_html.bat withads`  
   Creates the HTML files with adds under the `html` subdirectory at the `build` directory.

 * `build.bat htmlhelp` or `build_htmlhelp.bat`  
   Creates the CHM file under the `htmlhelp` subdirectory at the `build` directory.

 * `build.bat`  
   Is the same like `build.bat html`.
