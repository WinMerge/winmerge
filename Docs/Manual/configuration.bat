rem Java...
set docbook_java_exe=java.exe
set docbook_java_parameters=-Xms8m -Xmx256m

rem Saxon...
set docbook_saxon_jar=.\Shared\saxon\saxon.jar
set docbook_saxon_xsl=.\Shared\xsl\extensions\saxon655.jar

rem Xerces2 Java Parser...
set DBFACTORY=org.apache.xerces.jaxp.DocumentBuilderFactoryImpl
set SPFACTORY=org.apache.xerces.jaxp.SAXParserFactoryImpl
set XINCLUDE=org.apache.xerces.parsers.XIncludeParserConfiguration
set docbook_xerces_jar=.\Shared\xerces\xercesImpl.jar

rem HTML Help Compiler...
set docbook_hhc_exe=.\Shared\hhc\hhc.exe

rem Build directory...
rem Use "." for the current directory.
set docbook_build_path=..\..\Build\Manual