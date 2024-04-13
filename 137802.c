xsltCheckInstructionElement(xsltStylesheetPtr style, xmlNodePtr inst) {
    xmlNodePtr parent;
    int has_ext;

    if ((style == NULL) || (inst == NULL) || (inst->ns == NULL) ||
        (style->literal_result))
        return;

    has_ext = (style->extInfos != NULL);

    parent = inst->parent;
    if (parent == NULL) {
	xsltTransformError(NULL, style, inst,
		"internal problem: element has no parent\n");
	style->errors++;
	return;
    }
    while ((parent != NULL) && (parent->type != XML_DOCUMENT_NODE)) {
        if (((parent->ns == inst->ns) ||
	     ((parent->ns != NULL) &&
	      (xmlStrEqual(parent->ns->href, inst->ns->href)))) &&
	    ((xmlStrEqual(parent->name, BAD_CAST "template")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "param")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "attribute")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "variable")))) {
	    return;
	}

	/*
	 * if we are within an extension element all bets are off
	 * about the semantic there e.g. xsl:param within func:function
	 */
	if ((has_ext) && (parent->ns != NULL) &&
	    (xmlHashLookup(style->extInfos, parent->ns->href) != NULL))
	    return;

        parent = parent->parent;
    }
    xsltTransformError(NULL, style, inst,
	    "element %s only allowed within a template, variable or param\n",
		           inst->name);
    style->errors++;
}