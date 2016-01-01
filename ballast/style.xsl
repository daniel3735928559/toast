<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:key name="tags" match="tag" use="@name"/>
  <xsl:template match="footnote">
    <xsl:variable name="pos" select="count(preceding::footnote)+1" />
    <sup><a href="#footnote{$pos}">[<xsl:value-of select="$pos" />]</a></sup>
  </xsl:template>

  <xsl:template name="tags">
    <span class="smalltext"> (Tags: <xsl:for-each
    select="./tag"><xsl:value-of select="@name" /><xsl:if
    test="position()!=last()">, </xsl:if></xsl:for-each>)</span>
  </xsl:template>

  <xsl:template name="taglinks">
    [Tags: <xsl:for-each select="./tag">
      <a href="#{@name}"><xsl:value-of select="@name" /></a>
      <xsl:if test="position()!=last()"><xsl:text>, </xsl:text></xsl:if>
    </xsl:for-each>]<br />
  </xsl:template>

  <xsl:template name="link">
    <xsl:param name="withtags" select="'yes'" />
    <xsl:param name="withdate" select="'yes'" />
    <xsl:text>- </xsl:text><div class="shareditem"><a href="{@url}"><xsl:if test="$withdate='yes'"><xsl:value-of select="@date" /> -- </xsl:if><xsl:value-of
    select="@title" /></a>
    <xsl:if test="$withtags='yes'"><xsl:call-template name="tags" /></xsl:if></div><br />
  </xsl:template>

  <xsl:template match="post">
    
    <xsl:variable name="title" select="@title" />
    <div class="super">
      <div class="post">
	<div align="right"><a href="index.xml"><b>Ballast</b></a></div>
	<h2><xsl:value-of select="@date" />:<xsl:value-of select="@title" /></h2>
	<xsl:call-template name="taglinks" />
	<xsl:apply-templates select="text/*|text/text()"/>
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
      </div>
      <div class="index">
	<hr />
	<h2>Posts with shared tags:</h2>
	<table width="90%" cellspacing="10px">
	<xsl:for-each select="/post/tag">
	    <xsl:variable name="cpos" select="(position()-1)*3+1"/>
	    <tr>
	      <xsl:for-each select="../tag[position() >= $cpos and $cpos+3 > position()]">
		<xsl:sort order="descending" data-type="number" select="count(document('index.xml')/index/post/tag[@name=current()/@name])" />
		<xsl:variable name="name" select="@name" />
		<xsl:if test="count(document('index.xml')/index/post/tag[@name=$name]) > 1">
		  <td class="shared" border="5">
		    <a name="{@name}" />
		    <h3><xsl:value-of select="@name" />: </h3>
		    <xsl:for-each select="document('index.xml')/index/post/tag[@name=$name]/..">
		      <xsl:if test="@title!=$title"><xsl:call-template name="link">
		      <xsl:with-param name="withdate" select="'no'" /><xsl:with-param name="withtags" select="'no'" /></xsl:call-template></xsl:if>
		    </xsl:for-each>
		  </td>
		</xsl:if>
	      </xsl:for-each>
	    </tr>
	</xsl:for-each>
	</table>
	<hr />
	<h2>All other posts: </h2>
	<xsl:for-each select="document('index.xml')/index/post">
	  <xsl:sort select="@date" order="descending" />
	  <xsl:call-template name="link" />
	</xsl:for-each>
	<br />
      </div>
    </div>
  </xsl:template>

  <xsl:template match="index">
    <div class="mainindex super">
      <div align="right"><b>Ballast</b></div>
      <div class="intro">
	<h1>Intro: </h1>
	<xsl:apply-templates select="intro/*|intro/text()" />
      </div>

      <h1>Recent Posts:</h1>
      <xsl:for-each select="post">
	<xsl:sort select="@date" order="descending" />
	<xsl:if test="10 > position()"><xsl:call-template name="link" /></xsl:if>
      </xsl:for-each>

      <h1>By Tag (Click to expand):</h1>
      <xsl:for-each select="//tag[generate-id() =
			    generate-id(key('tags',@name)[1])]">
	<xsl:sort order="descending" data-type="number" select="count(//post/tag[@name=current()/@name])" />
	<xsl:variable name="name" select="@name" />
	<div style="display:inline-block;margin-right:4px;">
	  <div class="taglink" onclick="var d=document.getElementById('{@name}'); console.log(d.style.display); if(d.style.display=='' || d.style.display=='none'){{ d.style.display='block'; this.style.color='#f00';}} else{{ d.style.display='none'; this.style.color='#00f';}}">
	    <xsl:value-of select="$name" />
	  </div>
	  <span class="smalltext"> (<xsl:value-of select="count(//post/tag[@name=current()/@name])"/>)</span>
	  <xsl:if test="position()!=last()">,</xsl:if>
	</div>
      </xsl:for-each>

      <xsl:for-each select="//tag[generate-id() =
			    generate-id(key('tags',@name)[1])]">
	<xsl:sort order="descending" data-type="number"
		  select="count(//tag[@name=current()/@name])" />
	<xsl:variable name="name" select="@name" />
	<div id="{$name}" class="tagged">
	  <h2><xsl:value-of select="$name" />:</h2>
	  <xsl:for-each select="//post/tag[@name=$name]/..">
	    <xsl:call-template name="link">
	      <xsl:with-param name="withtags" select="'no'" />
	    </xsl:call-template>
	  </xsl:for-each>
	</div>
      </xsl:for-each>

      <h1>All Posts (Recent first):</h1>
      <xsl:for-each select="post">
	<xsl:sort select="@date" order="descending" />
	<xsl:call-template name="link" />
      </xsl:for-each>
    
    </div>
  </xsl:template>

  <xsl:template match="code">
    <div class="codebox">
      <pre><xsl:apply-templates select="./text()|./*" /></pre>
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
