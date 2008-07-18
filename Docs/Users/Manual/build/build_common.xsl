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
<xsl:param name="use.id.as.filename" select="1"/>
<xsl:param name="chunker.output.indent" select="'yes'"></xsl:param>
<xsl:param name="chunk.section.depth" select="0" />
<xsl:param name="generate.toc">book toc,title article nop</xsl:param>
<xsl:param name="toc.section.depth">1</xsl:param>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
common/local.l10n.xml 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- WinMerge: Change generated text -->
<xsl:param name="local.l10n.xml" select="document('')"/>
<l:i18n xmlns:l="http://docbook.sourceforge.net/xmlns/l10n/1.0">
  <l:l10n language="en">
    <l:context name="xref-number-and-title">
      <l:template name="section" text="%t"/>
    </l:context>
  </l:l10n>
</l:i18n>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/docbook.xsl 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<xsl:template name="user.head.content">
  <link rel="stylesheet" type="text/css" href="css/all.css" />
  <link rel="stylesheet" type="text/css" media="print" href="css/print.css" />
</xsl:template>

</xsl:stylesheet>
