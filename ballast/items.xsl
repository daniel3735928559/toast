<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:key name="tags" match="tag" use="@name"/>
  <xsl:template match="footnote">
    <xsl:variable name="pos" select="count(preceding::footnote)+1" />
    <sup><a href="#footnote{$pos}">[<xsl:value-of select="$pos" />]</a></sup>
  </xsl:template>

  <xsl:template name="link">
    <a href="{@url}"><xsl:value-of select="@name" /></a>
  </xsl:template>

  <xsl:template match="post">
    
    <div class="post">
      <xsl:variable name="title" select="@title" />
      <xsl:apply-templates select="text/*|text()"/>
      <xsl:if test="count(text//footnote) > 0">
	<br /><b>Footnotes:</b>
      </xsl:if>
      <xsl:for-each select="text//footnote">
	<br />
	<sup>
	  <a name="footnote{position()}">[<xsl:value-of select="position()" />]</a>
	</sup> 
	<xsl:value-of select="." />
      </xsl:for-each>
      <br />
      <div class="index">
	<xsl:for-each select="/post/tag">
          <h2>Other posts about <xsl:value-of select="@name" />: </h2>
          <xsl:for-each select="document('index.xml')/index/post/tag[name=@name]/..">
	    <xsl:if test="@title!=$title"><xsl:call-template name="link" /></xsl:if>
          </xsl:for-each>
	</xsl:for-each>
        <h2>All other posts: </h2>
        <xsl:for-each select="document('index.xml')/index/post">
	  <xsl:apply-templates select="."><xsl:with-param name="link" select="'yes'" /></xsl:apply-templates>
	</xsl:for-each>
        <br />
      </div>
    </div>
  </xsl:template>

  <xsl:template match="index">
    <div class="mainindex">
      <div align="right"><b>Occasional aubergine</b></div>

      <h1>By Date:</h1>
      <xsl:for-each select="post">
	<xsl:sort select="date" order="descending" />
	<xsl:call-template name="link" />
      </xsl:for-each>

      <h1>By Tag:</h1>
      <xsl:for-each select="//tag[generate-id() = generate-id(key('tags',@name)[1])">
	<xsl:call-template name="link" />
      </xsl:for-each>
    
    </div>
  </xsl:template>
  
  <xsl:template match="/">
    <html><head><title><xsl:value-of select="post/@title" /></title><link rel="stylesheet" type="text/css" href="style.css" /></head><body>
    <xsl:apply-templates select="post"/>
    <xsl:apply-templates select="index"/>
    </body></html>
  </xsl:template>

  <xsl:template match="@*|node()">
    <xsl:copy><xsl:apply-templates select="@*|node()" /></xsl:copy>
  </xsl:template>
</xsl:stylesheet>
