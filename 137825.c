xsltApplyImportsComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemApplyImportsPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif

    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemApplyImportsPtr) xsltNewStylePreComp(style, XSLT_FUNC_APPLYIMPORTS);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_APPLYIMPORTS);
#endif

    if (comp == NULL)
	return;
    inst->psvi = comp;
    comp->inst = inst;
}