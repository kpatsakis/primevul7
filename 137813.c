xsltVariableComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemVariablePtr comp;
#else
    xsltStylePreCompPtr comp;
#endif

    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemVariablePtr)
	xsltNewStylePreComp(style, XSLT_FUNC_VARIABLE);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_VARIABLE);
#endif

    if (comp == NULL)
	return;

    inst->psvi = comp;
    comp->inst = inst;
    /*
     * The full template resolution can be done statically
     */

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
#ifndef XSLT_REFACTORED
        xmlNodePtr cur;
#endif
	comp->comp = xsltXPathCompile(style, comp->select);
	if (comp->comp == NULL) {
	    xsltTransformError(NULL, style, inst,
		"XSLT-variable: Failed to compile the XPath expression '%s'.\n",
		comp->select);
	    style->errors++;
	}
#ifdef XSLT_REFACTORED
	if (inst->children != NULL) {
	    xsltTransformError(NULL, style, inst,
		"XSLT-variable: There must be no child nodes, since the "
		"attribute 'select' was specified.\n");
	    style->errors++;
	}
#else
        for (cur = inst->children; cur != NULL; cur = cur->next) {
            if (cur->type != XML_COMMENT_NODE &&
                (cur->type != XML_TEXT_NODE || !xsltIsBlank(cur->content)))
            {
                xsltTransformError(NULL, style, inst,
                    "XSLT-variable: There must be no child nodes, since the "
                    "attribute 'select' was specified.\n");
                style->errors++;
            }
        }
#endif
    }
}