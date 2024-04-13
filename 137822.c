xsltParamComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemParamPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif

    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemParamPtr)
	xsltNewStylePreComp(style, XSLT_FUNC_PARAM);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_PARAM);
#endif

    if (comp == NULL)
	return;
    inst->psvi = comp;
    comp->inst = inst;

    /*
     * Attribute "name".
     */
    xsltGetQNameProperty(style, inst, BAD_CAST "name",
	1, &(comp->has_name), &(comp->ns), &(comp->name));
    if (comp->ns)
	comp->has_ns = 1;
    /*
    * Attribute "select".
    */
    comp->select = xsltGetCNsProp(style, inst, (const xmlChar *)"select",
	                        XSLT_NAMESPACE);
    if (comp->select != NULL) {
	comp->comp = xsltXPathCompile(style, comp->select);
	if (comp->comp == NULL) {
	    xsltTransformError(NULL, style, inst,
		"XSLT-param: could not compile select expression '%s'.\n",
		comp->select);
	    style->errors++;
	}
	if (inst->children != NULL) {
	    xsltTransformError(NULL, style, inst,
		"XSLT-param: The content should be empty since the "
		"attribute 'select' is present.\n");
	    style->warnings++;
	}
    }
}