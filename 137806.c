xsltChooseComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemChoosePtr comp;
#else
    xsltStylePreCompPtr comp;
#endif

    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemChoosePtr)
	xsltNewStylePreComp(style, XSLT_FUNC_CHOOSE);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_CHOOSE);
#endif

    if (comp == NULL)
	return;
    inst->psvi = comp;
    comp->inst = inst;
}