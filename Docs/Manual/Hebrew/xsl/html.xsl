<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		version="1.0">

<!-- ********************************************************************
     WinMerge customization of the XSL DocBook stylesheet distribution.
     Contains customized templates and parameters for the HTML version 
     of help; all other styles are inherited from the WinMerge html.xsl.
     ******************************************************************** -->

<xsl:import href="../../Shared/xsl/html.xsl"/>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/param.xsl  parameters
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<xsl:param name="chunker.output.encoding" select="'iso-8859-8'"/>
<xsl:param name="l10n.gentext.language" select="'he'"/>
<xsl:param name="l10n.gentext.default.language" select="'he'"/>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
Custom parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- Define suffix to appear after title text in head/title element of 
every topic -->
<xsl:param name="headtitle.suffix" select="' - WinMerge 2.16 Manual'"/>

</xsl:stylesheet>
