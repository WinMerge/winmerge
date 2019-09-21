<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		version="1.0">

<!-- ********************************************************************
     WinMerge customization of the XSL DocBook stylesheet distribution.
     Contains customized templates and parameters for the HTML version 
     of help; all other styles are inherited from the WinMerge 
     common.xsl and the stock DocBook stylesheets in ../../Tools/xsl. 
     ******************************************************************** -->

<xsl:import href="../../Tools/xsl/html/chunk.xsl"/>
<xsl:import href="common.xsl"/>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
Custom parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- Define suffix to appear after title text in head/title element of 
every topic -->
<xsl:param name="headtitle.suffix" select="' - WinMerge Manual'"/>

<!-- Detect if we're displaying ads for the Web pages -->
<xsl:param name="withads">false</xsl:param>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/docbook.xsl 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  

<!--   Add headtitle.suffix to head title text -->
<xsl:template name="head.content">
  <xsl:param name="node" select="."/>
  <xsl:param name="title">
    <xsl:variable name="title">
      <xsl:apply-templates select="." mode="object.title.markup"/>
    </xsl:variable>
    <xsl:value-of select="normalize-space($title)"/>
    <xsl:value-of select="$headtitle.suffix"/>
  </xsl:param>

  <title>
    <xsl:copy-of select="$title"/>
  </title>

  <meta name="generator" content="DocBook {$DistroTitle} V{$VERSION}"/>
</xsl:template>

<!-- For web pages add banner image. Add Google ad scripts 
     if 'withads' param is set  to true. -->
<xsl:template name="user.header.content">
    <xsl:if test="$withads = 'true'">
      <xsl:call-template name="header.ads"/>
    </xsl:if>
</xsl:template>

</xsl:stylesheet>
