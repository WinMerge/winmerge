<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		version="1.0">

<!-- ********************************************************************
     build_html_withads.xsl
     WinMerge customization of the XSL DocBook stylesheet distribution.
     Contains customized templates and parameters for the Web site version 
     of help; all other styles are inherited from the WinMerge 
     build_common.xsl and the stock DocBook stylesheets in ../build/xsl. 
 
     See build_common.xsl for editing instructions.
     ******************************************************************** 
     Changes
     Created July 2008
     ******************************************************************** -->

<xsl:import href="build_html.xsl"/>

<!-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
html/docbook.xsl 		
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->  
<!-- WinMerge: commercialize the header -->
<xsl:template name="user.header.content">
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
