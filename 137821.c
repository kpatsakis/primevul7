xsltNumberComp(xsltStylesheetPtr style, xmlNodePtr cur) {
#ifdef XSLT_REFACTORED
    xsltStyleItemNumberPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif
    const xmlChar *prop;

    if ((style == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemNumberPtr) xsltNewStylePreComp(style, XSLT_FUNC_NUMBER);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_NUMBER);
#endif

    if (comp == NULL)
	return;
    cur->psvi = comp;

    if ((style == NULL) || (cur == NULL))
	return;

    comp->numdata.doc = cur->doc;
    comp->numdata.node = cur;
    comp->numdata.value = xsltGetCNsProp(style, cur, (const xmlChar *)"value",
	                                XSLT_NAMESPACE);

    prop = xsltEvalStaticAttrValueTemplate(style, cur,
			 (const xmlChar *)"format",
			 XSLT_NAMESPACE, &comp->numdata.has_format);
    if (comp->numdata.has_format == 0) {
	comp->numdata.format = xmlDictLookup(style->dict, BAD_CAST "" , 0);
    } else {
	comp->numdata.format = prop;
    }

    comp->numdata.count = xsltGetCNsProp(style, cur, (const xmlChar *)"count",
                                         XSLT_NAMESPACE);
    comp->numdata.from = xsltGetCNsProp(style, cur, (const xmlChar *)"from",
                                        XSLT_NAMESPACE);

    prop = xsltGetCNsProp(style, cur, (const xmlChar *)"count", XSLT_NAMESPACE);
    if (prop != NULL) {
	comp->numdata.countPat = xsltCompilePattern(prop, cur->doc, cur, style,
                                                    NULL);
    }

    prop = xsltGetCNsProp(style, cur, (const xmlChar *)"from", XSLT_NAMESPACE);
    if (prop != NULL) {
	comp->numdata.fromPat = xsltCompilePattern(prop, cur->doc, cur, style,
                                                   NULL);
    }

    prop = xsltGetCNsProp(style, cur, (const xmlChar *)"level", XSLT_NAMESPACE);
    if (prop != NULL) {
	if (xmlStrEqual(prop, BAD_CAST("single")) ||
	    xmlStrEqual(prop, BAD_CAST("multiple")) ||
	    xmlStrEqual(prop, BAD_CAST("any"))) {
	    comp->numdata.level = prop;
	} else {
	    xsltTransformError(NULL, style, cur,
			 "xsl:number : invalid value %s for level\n", prop);
	    if (style != NULL) style->warnings++;
	}
    }

    prop = xsltGetCNsProp(style, cur, (const xmlChar *)"lang", XSLT_NAMESPACE);
    if (prop != NULL) {
	    xsltTransformError(NULL, style, cur,
		 "xsl:number : lang attribute not implemented\n");
	XSLT_TODO; /* xsl:number lang attribute */
    }

    prop = xsltGetCNsProp(style, cur, (const xmlChar *)"letter-value", XSLT_NAMESPACE);
    if (prop != NULL) {
	if (xmlStrEqual(prop, BAD_CAST("alphabetic"))) {
	    xsltTransformError(NULL, style, cur,
		 "xsl:number : letter-value 'alphabetic' not implemented\n");
	    if (style != NULL) style->warnings++;
	    XSLT_TODO; /* xsl:number letter-value attribute alphabetic */
	} else if (xmlStrEqual(prop, BAD_CAST("traditional"))) {
	    xsltTransformError(NULL, style, cur,
		 "xsl:number : letter-value 'traditional' not implemented\n");
	    if (style != NULL) style->warnings++;
	    XSLT_TODO; /* xsl:number letter-value attribute traditional */
	} else {
	    xsltTransformError(NULL, style, cur,
		     "xsl:number : invalid value %s for letter-value\n", prop);
	    if (style != NULL) style->warnings++;
	}
    }

    prop = xsltGetCNsProp(style, cur, (const xmlChar *)"grouping-separator",
	                XSLT_NAMESPACE);
    if (prop != NULL) {
        comp->numdata.groupingCharacterLen = xmlStrlen(prop);
	comp->numdata.groupingCharacter =
	    xsltGetUTF8Char(prop, &(comp->numdata.groupingCharacterLen));
    }

    prop = xsltGetCNsProp(style, cur, (const xmlChar *)"grouping-size", XSLT_NAMESPACE);
    if (prop != NULL) {
	sscanf((char *)prop, "%d", &comp->numdata.digitsPerGroup);
    } else {
	comp->numdata.groupingCharacter = 0;
    }

    /* Set default values */
    if (comp->numdata.value == NULL) {
	if (comp->numdata.level == NULL) {
	    comp->numdata.level = xmlDictLookup(style->dict,
	                                        BAD_CAST"single", 6);
	}
    }

}