<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		version="1.0">

<!-- ********************************************************************
     WinMerge customization of the XSL DocBook stylesheet distribution.
     Contains customized templates and parameters for the HTML Help version 
     of help; all other styles are inherited from the WinMerge htmlhelp.xsl.
     ******************************************************************** -->

<xsl:import href="../../Shared/xsl/htmlhelp.xsl"/>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/param.xsl  parameters
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<xsl:param name="chunker.output.encoding" select="'Shift_JIS'"/>
<xsl:param name="l10n.gentext.language" select="'ja'"/>
<xsl:param name="l10n.gentext.default.language" select="'ja'"/>
<xsl:param name="htmlhelp.encoding" select="'Shift_JIS'"/>
<xsl:param name="htmlhelp.title">WinMerge ヘルプ</xsl:param>

</xsl:stylesheet>
