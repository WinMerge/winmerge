<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0">

<!-- ********************************************************************
     WinMerge customization of the XSL DocBook stylesheet distribution.
     Imported by other WinMerge *.xsl files, so they use the parameters 
     and templates here unless they have their own versions. 
     ******************************************************************** -->

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/param.xsl  parameters
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<xsl:param name="suppress.navigation" select="0"/> 
<xsl:param name="header.rule" select="0"/>
<xsl:param name="footer.rule" select="0"/>
<xsl:param name="use.extensions" select="0"/>
<xsl:param name="tablecolumns.extensions" select="0"/>
<xsl:param name="admon.graphics" select="0"/>
<xsl:param name="callout.graphics" select="0"/>
<xsl:param name="navig.graphics" select="0"/>
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
<!-- WinMerge: Remove "Section X.Y" from links -->
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
    <script type="text/javascript" src="https://pagead2.googlesyndication.com/pagead/show_ads.js">
    </script>
  </div>
</xsl:template>


</xsl:stylesheet>
