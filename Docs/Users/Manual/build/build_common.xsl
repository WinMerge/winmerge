<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0">

<!-- ********************************************************************
     build-common.xsl

     WinMerge customization of the XSL DocBook stylesheet distribution.
     Imported by other WinMerge build*.xsl files, so they use the parameters 
     and templates here unless they have their own versions. 

     Do not modify any stock template directly in the DocBook XSL library. 
     Instead, copy the template to this file or one of the other build*.xsl 
     files, and modify only the copy. Any custom template supersedes the  
     stock template of the same name. 

     Please put any custom template in a section that identifies the 
     source file in the stock XSL library. That makes it easy to maintain 
     if the DocBook stock template changes. Likewise, any new ones (not based 
     on stock templates) should go in a section  marked, "New templates".
     ******************************************************************** 
     Changes
     Created July 2008
     3 Aug 2009: Added 'header.ads' custom template
     ******************************************************************** -->
<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/param.xsl  parameters
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<xsl:param name="suppress.navigation" select="0"/> 
<xsl:param name="header.rule" select="0"/>
<xsl:param name="footer.rule" select="0"/>
<xsl:param name="use.extensions" select="0"/>
<xsl:param name="tablecolumns.extensions" select="0"/>
<xsl:param name="admon.graphics" select="1"/>
<xsl:param name="admon.graphics.path">images/</xsl:param>
<xsl:param name="admon.graphics.extension">.gif</xsl:param>
<xsl:param name="section.autolabel" select="1"/>
<xsl:param name="appendix.autolabel">A</xsl:param>
<xsl:param name="use.id.as.filename" select="1"/>
<xsl:param name="chunker.output.indent" select="'yes'"></xsl:param>
<xsl:param name="chunk.section.depth" select="0" />
<xsl:param name="generate.toc">book toc,title article nop</xsl:param>
<xsl:param name="toc.section.depth">1</xsl:param>
<xsl:param name="runinhead.default.title.end.punct">:</xsl:param>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
common/local.l10n.xml 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- WinMerge: Change generated text -->
<xsl:param name="local.l10n.xml" select="document('')"/>
<l:i18n xmlns:l="http://docbook.sourceforge.net/xmlns/l10n/1.0">
  <l:l10n language="en">
     <l:context name="xref">
       <l:template name="section" text="%t"/>
     </l:context>
    <l:context name="xref-number-and-title">
      <l:template name="section" text="%t"/>
    </l:context>
  </l:l10n>
</l:i18n>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/docbook.xsl 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- Link to the appropriate css stylesheets -->
<xsl:template name="user.head.content">
  <link rel="stylesheet" type="text/css" href="css/all.css" />
  <link rel="stylesheet" type="text/css" media="print" href="css/print.css" />
</xsl:template>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/autotoc.xsl
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- WinMerge: turn off section numbers in TOC. 
      Note: if we decide to set  section.autolabel to 0 above, this customization can be removed -->
<xsl:template name="toc.line">
  <xsl:param name="toc-context" select="."/>
  <xsl:param name="depth" select="1"/>
  <xsl:param name="depth.from.context" select="8"/>

 <span>
  <xsl:attribute name="class"><xsl:value-of select="local-name(.)"/></xsl:attribute>

  <!-- * if $autotoc.label.in.hyperlink is zero, then output the label -->
  <!-- * before the hyperlinked title (as the DSSSL stylesheet does) -->
  <xsl:if test="$autotoc.label.in.hyperlink = 0">
    <xsl:variable name="label">
      <xsl:apply-templates select="." mode="label.markup"/>
    </xsl:variable>
    <xsl:copy-of select="$label"/>
    <xsl:if test="$label != ''">
      <xsl:value-of select="$autotoc.label.separator"/>
    </xsl:if>
  </xsl:if>

  <a>
    <xsl:attribute name="href">
      <xsl:call-template name="href.target">
        <xsl:with-param name="context" select="$toc-context"/>
        <xsl:with-param name="toc-context" select="$toc-context"/>
      </xsl:call-template>
    </xsl:attribute>
    
  <!-- * if $autotoc.label.in.hyperlink is non-zero, then output the label -->
  <!-- * as part of the hyperlinked title -->
  <!-- WinMerge: add test for section -->
  <xsl:if test="not($autotoc.label.in.hyperlink = 0) and local-name(.) != 'section'" >
    <xsl:variable name="label">
      <xsl:apply-templates select="." mode="label.markup"/>
    </xsl:variable>
    <xsl:copy-of select="$label"/>
    <xsl:if test="$label != ''"> 
      <xsl:value-of select="$autotoc.label.separator"/>
    </xsl:if>
  </xsl:if>
    <xsl:apply-templates select="." mode="titleabbrev.markup"/>
  </a>
  </span>
</xsl:template>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
WinMerge custom templates 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- This template adds ads to the top of HTML pages when you use the 
     'build html withads' command line to generate commercialized 
     pages for the WinMerge web site. -->
<xsl:template name="header.ads">
  <div id="ads">
    <script type="text/javascript">&lt;!--
      google_ad_client = "pub-1411904495533584";
      google_ad_width = 728;
      google_ad_height = 90;
      google_ad_format = "728x90_as";
      google_ad_type = "text";
      google_ad_channel ="";
      google_color_border = "FFCC00";
      google_color_bg = "FFFFCC";
      google_color_link = "0000FF";
      google_color_text = "000000";
      google_color_url = "008000";
    //--&gt;</script>
    <script type="text/javascript" src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
    </script>
  </div>
</xsl:template>


</xsl:stylesheet>
