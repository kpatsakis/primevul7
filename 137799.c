xsltWithParamComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemWithParamPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif

    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemWithParamPtr) xsltNewStylePreComp(style, XSLT_FUNC_WITHPARAM);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_WITHPARAM);
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
		 "XSLT-with-param: Failed to compile select "
		 "expression '%s'\n", comp->select);
	    style->errors++;
	}
	if (inst->children != NULL) {
	    xsltTransformError(NULL, style, inst,
		"XSLT-with-param: The content should be empty since "
		"the attribute select is present.\n");
	    style->warnings++;
	}
    }
}