xsltTextComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemTextPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif
    const xmlChar *prop;

    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemTextPtr) xsltNewStylePreComp(style, XSLT_FUNC_TEXT);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_TEXT);
#endif
    if (comp == NULL)
	return;
    inst->psvi = comp;
    comp->inst = inst;
    comp->noescape = 0;

    prop = xsltGetCNsProp(style, inst,
	    (const xmlChar *)"disable-output-escaping",
			XSLT_NAMESPACE);
    if (prop != NULL) {
	if (xmlStrEqual(prop, (const xmlChar *)"yes")) {
	    comp->noescape = 1;
	} else if (!xmlStrEqual(prop,
	    (const xmlChar *)"no")){
	    xsltTransformError(NULL, style, inst,
		"xsl:text: disable-output-escaping allows only yes or no\n");
	    if (style != NULL) style->warnings++;
	}
    }
}