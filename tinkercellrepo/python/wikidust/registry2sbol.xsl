<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0"
    xmlns="http://sbol.bhi.washington.edu/rdf/sbol.owl#"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
    xmlns:rdfs="http://www.w3.org/2000/01/rdf-schema#">

<xsl:template match="/">
    <rdf:RDF>
        <xsl:apply-templates select="rsbpml/part_list/part"/>
    </rdf:RDF>
</xsl:template>

<xsl:template match="part|subpart">
    <rdf:Description rdf:about="http://sbol.bhi.washington.edu/rdf/sbol.owl#">
        <rdf:type rdf:resource="http://sbol.bhi.washington.edu/rdf/sbol.owl#Part"/>
        <name><xsl:value-of select="part_name"/></name>
        <author><xsl:value-of select="part_author"/></author>
        <date><xsl:value-of select="part_entered"/></date>
        <shortDescription><xsl:value-of select="part_short_desc"/></shortDescription>
        <description><xsl:value-of select="part_desc"/></description>
        type:<xsl:value-of select="part_type"/>
        status:<xsl:value-of select="part_status"/>
        results:<xsl:value-of select="part_results"/>
        nick:<xsl:value-of select="part_nickname"/>
        rating:<xsl:value-of select="part_rating"/>
        url:<xsl:value-of select="part_url"/>
        <xsl:apply-templates select="sequences"/>
        <xsl:apply-templates select="features/feature"/>
        <xsl:apply-templates select="categories/category"/>
        <xsl:apply-templates select="specified_subparts/subpart"/>
        <!-- <xsl:apply-templates select="deep_subparts"/> -->
    </rdf:Description>
</xsl:template>

<xsl:template match="sequences">
    <!-- combines all seq_data nodes into one dnaSequence -->
    <!-- and for some reason, leaves spaces between them? -->
    <dnaSequence>
        <xsl:for-each select="seq_data">
            <xsl:value-of select="text()"/>
        </xsl:for-each>
    </dnaSequence>
</xsl:template>

<xsl:template match="feature">
    id:<xsl:value-of select="id"/>
    title:<xsl:value-of select="title"/>
    type:<xsl:value-of select="type"/>
    dir:<xsl:value-of select="direction"/>
    start:<xsl:value-of select="startpos"/>
    end:<xsl:value-of select="endpos"/>
</xsl:template>

<xsl:template match="category">
    <!-- need some string processing here to make nodes? -->
    cat:<xsl:value-of select="text()"/>
</xsl:template>

</xsl:stylesheet>

