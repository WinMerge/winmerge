<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		version="1.0">

<!-- ********************************************************************
     build_tour.xsl
     WinMerge customization of the XSL DocBook stylesheet distribution.
     Contains customized templates and parameters for generating 
     a single HTML document; all other styles are inherited from the WinMerge 
     build_common.xsl and the stock DocBook stylesheets in ../build/xsl. 
 
     See build_common.xsl for editing instructions.
     ******************************************************************** 
     Changes:
     Created July 2009
     ******************************************************************** -->

<xsl:import href="../build/xsl/html/docbook.xsl"/>
<xsl:import href="build_common.xsl"/>

<xsl:output method="html"
            encoding="ISO-8859-1"
            indent="yes"/>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
Custom parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- Define suffix to appear after title text in head/title element of 
every topic -->
<xsl:param name="headtitle.suffix" select="' - WinMerge 2.12 Quicktour'"/>

<!-- Detect if we're displaying ads for the Web pages -->
<xsl:param name="withads">false</xsl:param>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
     html/param.xsl  parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<xsl:param name="section.autolabel" select="0"/>
<xsl:param name="generate.toc">book toc</xsl:param> 
<xsl:param name="generate.meta.abstract" select="0"/>


<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/docbook.xsl 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- Link to the appropriate css stylesheets -->
<xsl:template name="user.head.content">
  <link rel="stylesheet" type="text/css" href="css/tour.css" />
  <link rel="stylesheet" type="text/css" media="print" href="css/print.css" />
</xsl:template>

<!-- For web pages add banner image and Google ad scripts -->
<xsl:template name="user.header.content">
    <div class="header"><a href="http://www.winmerge.org">
      <img src="images/head.gif" alt="winmerge.org"/></a>
    </div>
    <xsl:if test="$withads = 'true'">
      <xsl:call-template name="header.ads"/>
    </xsl:if>
</xsl:template>

</xsl:stylesheet>
