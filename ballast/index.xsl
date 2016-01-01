<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:param name="url" />
<xsl:output omit-xml-declaration="yes" />
  <xsl:template match="/">
    <xsl:apply-templates select="post" />
  </xsl:template>

  <xsl:template match="post">
    <xsl:copy><xsl:attribute name="url"><xsl:value-of select="translate($url,':','_')" /></xsl:attribute><xsl:apply-templates select="@*|tag" /></xsl:copy>
  </xsl:template>

  <xsl:template match="tag|@*">
    <xsl:copy><xsl:apply-templates select="@*" /></xsl:copy>
  </xsl:template>
</xsl:stylesheet>
