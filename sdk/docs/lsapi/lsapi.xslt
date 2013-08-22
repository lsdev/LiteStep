<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:preserve-space elements="code blockcode"/>
<xsl:output method="html"/>

<xsl:param name="local_extension">xml</xsl:param>

<xsl:template match="/">
  <html>
    <head>
      <title>
        <xsl:value-of select="//name"/>
      </title>
      <link href="lsapi.css" rel="stylesheet" type="text/css"/>
    </head>
    <body>
      <div id="indexlink"><a href="index.xml">Return To Index</a></div>
      <xsl:apply-templates/>
    </body>
  </html>
</xsl:template>

<!-- Function -->

<xsl:template match="function">
  <h1>
    <xsl:value-of select="name"/>
  </h1>
  <div class="brief">
    <xsl:apply-templates select="description"/>
  </div>
  <h2>Syntax</h2>
  <div class="syntax">
    <pre class="blockcode">
      <xsl:choose>
        <xsl:when test="return">
          <xsl:value-of select="return/type"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>VOID</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text> </xsl:text>
      <xsl:if test="calling-convention">
        <xsl:value-of select="calling-convention"/>
        <xsl:text> </xsl:text>
      </xsl:if>
      <xsl:value-of select="name"/>
      <xsl:text>(</xsl:text>
      <xsl:for-each select="parameters/parameter">
        <xsl:if test="position() = 1">
          <xsl:text>&#10;</xsl:text>
        </xsl:if>
        <xsl:text>&#32;&#32;&#32;&#32;</xsl:text>
        <xsl:if test="count(type) > 0"> <!-- typeless "..." parameter list -->
          <xsl:value-of select="type"/>
          <xsl:text> </xsl:text>
        </xsl:if>
        <xsl:value-of select="name"/>
        <xsl:if test="position() != last()">
          <xsl:text>,</xsl:text>
        </xsl:if>
        <xsl:text>&#10;</xsl:text>
      </xsl:for-each>
      <xsl:text>);</xsl:text>
    </pre>
  </div>
  <xsl:apply-templates select="parameters"/>
  <xsl:apply-templates select="return"/>
  <xsl:apply-templates select="remarks"/>
  <xsl:apply-templates select="example"/>
  <xsl:apply-templates select="see-also"/>
</xsl:template>

<xsl:template match="parameters">
  <h2>Parameters</h2>
  <div class="parameters">
    <dl>
      <xsl:apply-templates select="parameter"/>
    </dl>
  </div>
</xsl:template>

<xsl:template match="parameter">
  <dt class="parameter-name">
    <xsl:value-of select="name"/>
  </dt>
  <dd class="parameter-description">
    <xsl:apply-templates select="description"/>
  </dd>
</xsl:template>

<xsl:template match="return">
  <h2>Return Value</h2>
  <div class="return">
    <xsl:apply-templates select="description"/>
  </div>
</xsl:template>

<!-- Message -->

<xsl:template match="message">
  <h1>
    <xsl:value-of select="name"/>
  </h1>
  <div class="brief">
    <xsl:apply-templates select="description"/>
  </div>
  <h2>Syntax</h2>
  <div class="syntax">
    <pre class="blockcode">
      <xsl:value-of select="name"/>
      <xsl:for-each select="parameters/parameter">
        <xsl:text>&#10;&#32;&#32;&#32;&#32;</xsl:text>
        <xsl:value-of select="type"/>
        <xsl:text> </xsl:text>
        <xsl:value-of select="name"/>
      </xsl:for-each>
    </pre>
  </div>
  <xsl:apply-templates select="parameters"/>
  <xsl:apply-templates select="return"/>
  <xsl:apply-templates select="remarks"/>
  <xsl:apply-templates select="example"/>
  <xsl:apply-templates select="see-also"/>
</xsl:template>

<!-- Structure -->

<xsl:template match="structure">
  <h1>
    <xsl:value-of select="name"/>
  </h1>
  <div class="brief">
    <xsl:apply-templates select="description"/>
  </div>
  <h2>Syntax</h2>
  <div class="syntax">
    <pre class="blockcode">
      <xsl:text>struct </xsl:text>
      <xsl:value-of select="name"/>
      <xsl:text>&#10;{&#10;</xsl:text>
      <xsl:for-each select="members/member">
        <xsl:text>&#32;&#32;&#32;&#32;</xsl:text>
        <xsl:value-of select="type"/>
        <xsl:if test="not(contains(type, '*'))">
          <xsl:text> </xsl:text>
        </xsl:if>
        <xsl:value-of select="name"/>
        <!-- Optionally insert array size following the name -->
        <xsl:if test="type/@array-size">
           <xsl:text>[</xsl:text>
           <xsl:value-of select="type/@array-size"/>
           <xsl:text>]</xsl:text>
        </xsl:if>
        <xsl:text>;&#10;</xsl:text>
      </xsl:for-each>
      <xsl:text>};</xsl:text>
    </pre>
  </div>
  <xsl:apply-templates select="members"/>
  <xsl:apply-templates select="remarks"/>
  <xsl:apply-templates select="example"/>
  <xsl:apply-templates select="see-also"/>
</xsl:template>

<xsl:template match="members">
  <h2>Members</h2>
  <div class="members">
    <dl>
      <xsl:apply-templates select="member"/>
    </dl>
  </div>
</xsl:template>

<xsl:template match="member">
  <dt class="member-name">
    <xsl:value-of select="name"/>
  </dt>
  <dd class="member-description">
    <xsl:apply-templates select="description"/>
  </dd>
</xsl:template>

<!-- Common Sections -->

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

<xsl:template match="remarks">
  <h2>Remarks</h2>
  <div class="remarks">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="example">
  <h2>Example</h2>
  <div class="example">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="see-also">
  <h2>See Also</h2>
  <div class="see-also">
    <p>
      <xsl:for-each select="fn | msg | struct">
        <a href="{concat(concat(string(.), '.'),$local_extension)}">
          <span class="{local-name(.)}">
            <xsl:value-of select="."/>
          </span>
        </a>
        <xsl:if test="position() != last()">
          <xsl:text>, </xsl:text>
        </xsl:if>
      </xsl:for-each>
    </p>
  </div>
</xsl:template>

<!-- Description and Remarks Markup -->

<xsl:template match="code | const | extfn | extmsg | extstruct | param | type">
  <span class="{local-name(.)}">
    <xsl:apply-templates/>
  </span>
</xsl:template>

<xsl:template match="blockcode">
  <pre class="blockcode">
    <xsl:apply-templates/>
  </pre>
</xsl:template>

<xsl:template match="constant-list">
  <p>
    <dl class="constant-list">
      <xsl:apply-templates select="constant"/>
    </dl>
  </p>
</xsl:template>

<xsl:template match="constant">
  <dt class="constant-name">
    <xsl:apply-templates select="name"/>
  </dt>
  <dd class="constant-description">
    <xsl:apply-templates select="description"/>
  </dd>
</xsl:template>

<xsl:template match="fn">
  <xsl:variable name="name" select="string(.)"/>
  <xsl:choose>
    <xsl:when test="string(//name)=$name or ancestor::fn[string(.)=$name] or preceding::fn[string(.)=$name]">
      <span class="fn">
        <xsl:value-of select="."/>
      </span>
    </xsl:when>
    <xsl:otherwise>
      <a href="{concat(concat(string(.), '.'),$local_extension)}">
        <span class="fn">
          <xsl:value-of select="."/>
        </span>
      </a>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="msg">
  <xsl:variable name="name" select="string(.)"/>
  <xsl:choose>
    <xsl:when test="string(//name)=$name or ancestor::msg[string(.)=$name] or preceding::msg[string(.)=$name]">
      <span class="msg">
        <xsl:value-of select="."/>
      </span>
    </xsl:when>
    <xsl:otherwise>
      <a href="{concat(concat(string(.), '.'),$local_extension)}">
        <span class="msg">
          <xsl:value-of select="."/>
        </span>
      </a>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="struct">
  <xsl:variable name="name" select="string(.)"/>
  <xsl:choose>
    <xsl:when test="string(//name)=$name or ancestor::struct[string(.)=$name] or preceding::struct[string(.)=$name]">
      <span class="struct">
        <xsl:value-of select="."/>
      </span>
    </xsl:when>
    <xsl:otherwise>
      <a href="{concat(concat(string(.), '.'),$local_extension)}">
        <span class="struct">
          <xsl:value-of select="."/>
        </span>
      </a>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- Supported HTML Tags -->

<xsl:template match="br | dd | dl | dt | em | li | ol | p | strong | ul">
  <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
