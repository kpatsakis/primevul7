xsltCopyComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemCopyPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif

    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;
#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemCopyPtr) xsltNewStylePreComp(style, XSLT_FUNC_COPY);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_COPY);
#endif

    if (comp == NULL)
	return;
    inst->psvi = comp;
    comp->inst = inst;


    comp->use = xsltGetCNsProp(style, inst, (const xmlChar *)"use-attribute-sets",
				    XSLT_NAMESPACE);
    if (comp->use == NULL)
	comp->has_use = 0;
    else
	comp->has_use = 1;
}