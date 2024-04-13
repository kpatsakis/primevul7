xsltForEachComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemForEachPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif

    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemForEachPtr)
	xsltNewStylePreComp(style, XSLT_FUNC_FOREACH);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_FOREACH);
#endif

    if (comp == NULL)
	return;
    inst->psvi = comp;
    comp->inst = inst;

    comp->select = xsltGetCNsProp(style, inst, (const xmlChar *)"select",
	                        XSLT_NAMESPACE);
    if (comp->select == NULL) {
	xsltTransformError(NULL, style, inst,
		"xsl:for-each : select is missing\n");
	if (style != NULL) style->errors++;
    } else {
	comp->comp = xsltXPathCompile(style, comp->select);
	if (comp->comp == NULL) {
	    xsltTransformError(NULL, style, inst,
     "xsl:for-each : could not compile select expression '%s'\n",
			     comp->select);
	    if (style != NULL) style->errors++;
	}
    }
    /* TODO: handle and skip the xsl:sort */
}