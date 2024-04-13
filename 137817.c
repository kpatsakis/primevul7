xsltApplyTemplatesComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemApplyTemplatesPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif

    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemApplyTemplatesPtr)
	xsltNewStylePreComp(style, XSLT_FUNC_APPLYTEMPLATES);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_APPLYTEMPLATES);
#endif

    if (comp == NULL)
	return;
    inst->psvi = comp;
    comp->inst = inst;

    /*
     * Attribute "mode".
     */
    xsltGetQNameProperty(style, inst, BAD_CAST "mode",
	0, NULL, &(comp->modeURI), &(comp->mode));
    /*
    * Attribute "select".
    */
    comp->select = xsltGetCNsProp(style, inst, BAD_CAST "select",
	XSLT_NAMESPACE);
    if (comp->select != NULL) {
	comp->comp = xsltXPathCompile(style, comp->select);
	if (comp->comp == NULL) {
	    xsltTransformError(NULL, style, inst,
		"XSLT-apply-templates: could not compile select "
		"expression '%s'\n", comp->select);
	     style->errors++;
	}
    }
    /* TODO: handle (or skip) the xsl:sort and xsl:with-param */
}