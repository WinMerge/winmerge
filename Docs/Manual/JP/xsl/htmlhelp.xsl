<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		version="1.0">

<!-- ********************************************************************
     WinMerge customization of the XSL DocBook stylesheet distribution.
     Contains customized templates and parameters for the HTML Help version 
     of help; all other styles are inherited from the WinMerge 
     common.xsl and the stock DocBook stylesheets in ../../Tools/xsl. 
     ******************************************************************** -->

<xsl:import href="../../Tools/xsl/htmlhelp/htmlhelp.xsl"/>
<xsl:import href="common.xsl"/>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/param.xsl  parameters
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<xsl:param name="chunker.output.encoding" select="'Shift_JIS'"/>
<xsl:param name="l10n.gentext.language" select="'ja'"/>
<xsl:param name="l10n.gentext.default.language" select="'ja'"/>
<xsl:param name="suppress.navigation" select="1"/>
<xsl:param name="generate.toc">set nop</xsl:param>
<xsl:param name="use.id.as.filename" select="1"/>
<xsl:param name="html.stylesheet">css/help.css</xsl:param>
<xsl:param name="htmlhelp.encoding" select="'Shift_JIS'"/>
<xsl:param name="htmlhelp.use.hhk" select="1"/>
<xsl:param name="htmlhelp.default.topic">htmlhelp/About_Doc.html</xsl:param> 
<xsl:param name="htmlhelp.title">WinMerge Help</xsl:param>
<xsl:param name="htmlhelp.show.menu" select="1"/>

</xsl:stylesheet>
