<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html"/>

<xsl:param name="local_extension">xml</xsl:param>

<xsl:template match="/">
  <html>
    <head>
      <title>
        <xsl:value-of select="index/title"/>
      </title>
      <link href="lsapi.css" rel="stylesheet" type="text/css"/>
    </head>
    <body>
      <xsl:apply-templates select="index/section"/>
    </body>
  </html>
</xsl:template>

<!-- Section -->

<xsl:template match="section">
  <xsl:element name="{concat('h',count(ancestor::section)+1)}">
    <xsl:value-of select="@name"/>
  </xsl:element>
  <xsl:if test="count(description) > 0">
    <xsl:apply-templates select="description"/>
  </xsl:if>
  <xsl:if test="count(link) > 0">
    <xsl:element name="ul">
      <xsl:apply-templates select="link" />
    </xsl:element>
  </xsl:if>
  <xsl:apply-templates select="section"/>
</xsl:template>

<!-- Description -->

<xsl:template match="description">
  <xsl:choose>
    <xsl:when test="p">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <p>
        <xsl:apply-templates/>
      </p>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- Link -->

<xsl:template match="link">
  <li>
    <xsl:choose>
      <xsl:when test="@stub = 'yes'">
        <xsl:value-of select="."/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="@target">
            <a href="{concat(concat(@target,'.'), $local_extension)}">
              <xsl:value-of select="."/>
            </a>
          </xsl:when>
          
          <xsl:otherwise>
            <a href="{concat(concat(string(.), '.'), $local_extension)}">
              <xsl:value-of select="."/>
            </a>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </li>
</xsl:template>

<!-- Supported HTML Tags -->

<xsl:template match="br | dd | dl | dt | em | li | ol | p | strong | ul">
  <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
