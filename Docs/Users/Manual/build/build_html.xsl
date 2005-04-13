<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0">
  <xsl:import href="file:/C:/docbook/xsl/html/chunk.xsl"/>
  <xsl:param name="suppress.navigation" select="0"/>
  <xsl:param name="use.extensions" select="0"/>
  <xsl:param name="tablecolumns.extensions" select="0"/>
  <xsl:param name="admon.graphics" select="1"/>
  <xsl:param name="admon.graphics.path">images/</xsl:param>
  <xsl:param name="section.autolabel" select="1"/>
  <xsl:param name="use.id.as.filename" select="1"/>
  <xsl:param name="chunker.output.indent" select="'yes'"></xsl:param>
  <xsl:template name="user.head.content">
    <link rel="stylesheet" type="text/css" href="css/all.css" />
    <link rel="stylesheet" type="text/css" media="print" href="css/print.css" />
  </xsl:template>
</xsl:stylesheet>
