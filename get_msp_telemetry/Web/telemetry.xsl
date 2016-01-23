<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >


<xsl:template match="/TelemetryRecord">
<tr>

<td><xsl:value-of select="substring(Time, 18,2)"/></td>

<td>Batt</td>
<td><xsl:value-of select="BatteryV"/>V</td>

<td>Roll</td>
<td><xsl:value-of select="AngleX"/><xsl:text disable-output-escaping="yes"><![CDATA[&deg;]]></xsl:text></td>

<td>Pitch</td>
<td><xsl:value-of select="AngleY"/><xsl:text disable-output-escaping="yes"><![CDATA[&deg;]]></xsl:text></td>

<td>Throttle</td>
<td><xsl:value-of select="format-number(RcThrottlePct, '#')"/>%</td>

<td>Gps?</td>
<td>
<xsl:if test="GpsFix = 'true'">&#9745;</xsl:if>
<xsl:if test="not(GpsFix = 'true')">&#9744;</xsl:if>
</td>

<td>Head</td>
<td><xsl:value-of select="GpsHeading"/><xsl:text disable-output-escaping="yes"><![CDATA[&deg;]]></xsl:text></td>

<td>Alt</td>
<td><xsl:value-of select="GpsAltitudeM"/>m</td>

<td>Spd</td>
<td><xsl:value-of select="GpsSpeedMpS * 3.6"/>km/h</td>



</tr>
</xsl:template>

</xsl:stylesheet>
