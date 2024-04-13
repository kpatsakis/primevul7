xsltSortComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemSortPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif
    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemSortPtr) xsltNewStylePreComp(style, XSLT_FUNC_SORT);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_SORT);
#endif

    if (comp == NULL)
	return;
    inst->psvi = comp;
    comp->inst = inst;

    comp->stype = xsltEvalStaticAttrValueTemplate(style, inst,
			 (const xmlChar *)"data-type",
			 NULL, &comp->has_stype);
    if (comp->stype != NULL) {
	if (xmlStrEqual(comp->stype, (const xmlChar *) "text"))
	    comp->number = 0;
	else if (xmlStrEqual(comp->stype, (const xmlChar *) "number"))
	    comp->number = 1;
	else {
	    xsltTransformError(NULL, style, inst,
		 "xsltSortComp: no support for data-type = %s\n", comp->stype);
	    comp->number = 0; /* use default */
	    if (style != NULL) style->warnings++;
	}
    }
    comp->order = xsltEvalStaticAttrValueTemplate(style, inst,
			      (const xmlChar *)"order",
			      NULL, &comp->has_order);
    if (comp->order != NULL) {
	if (xmlStrEqual(comp->order, (const xmlChar *) "ascending"))
	    comp->descending = 0;
	else if (xmlStrEqual(comp->order, (const xmlChar *) "descending"))
	    comp->descending = 1;
	else {
	    xsltTransformError(NULL, style, inst,
		 "xsltSortComp: invalid value %s for order\n", comp->order);
	    comp->descending = 0; /* use default */
	    if (style != NULL) style->warnings++;
	}
    }
    comp->case_order = xsltEvalStaticAttrValueTemplate(style, inst,
			      (const xmlChar *)"case-order",
			      NULL, &comp->has_use);
    if (comp->case_order != NULL) {
	if (xmlStrEqual(comp->case_order, (const xmlChar *) "upper-first"))
	    comp->lower_first = 0;
	else if (xmlStrEqual(comp->case_order, (const xmlChar *) "lower-first"))
	    comp->lower_first = 1;
	else {
	    xsltTransformError(NULL, style, inst,
		 "xsltSortComp: invalid value %s for order\n", comp->order);
	    comp->lower_first = 0; /* use default */
	    if (style != NULL) style->warnings++;
	}
    }

    comp->lang = xsltEvalStaticAttrValueTemplate(style, inst,
				 (const xmlChar *)"lang",
				 NULL, &comp->has_lang);
    if (comp->lang != NULL) {
	comp->locale = xsltNewLocale(comp->lang);
    }
    else {
        comp->locale = (xsltLocale)0;
    }

    comp->select = xsltGetCNsProp(style, inst,(const xmlChar *)"select", XSLT_NAMESPACE);
    if (comp->select == NULL) {
	/*
	 * The default value of the select attribute is ., which will
	 * cause the string-value of the current node to be used as
	 * the sort key.
	 */
	comp->select = xmlDictLookup(style->dict, BAD_CAST ".", 1);
    }
    comp->comp = xsltXPathCompile(style, comp->select);
    if (comp->comp == NULL) {
	xsltTransformError(NULL, style, inst,
	     "xsltSortComp: could not compile select expression '%s'\n",
	                 comp->select);
	if (style != NULL) style->errors++;
    }
    if (inst->children != NULL) {
	xsltTransformError(NULL, style, inst,
	"xsl:sort : is not empty\n");
	if (style != NULL) style->errors++;
    }
}