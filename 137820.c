xsltCheckParentElement(xsltStylesheetPtr style, xmlNodePtr inst,
                       const xmlChar *allow1, const xmlChar *allow2) {
    xmlNodePtr parent;

    if ((style == NULL) || (inst == NULL) || (inst->ns == NULL) ||
        (style->literal_result))
        return;

    parent = inst->parent;
    if (parent == NULL) {
	xsltTransformError(NULL, style, inst,
		"internal problem: element has no parent\n");
	style->errors++;
	return;
    }
    if (((parent->ns == inst->ns) ||
	 ((parent->ns != NULL) &&
	  (xmlStrEqual(parent->ns->href, inst->ns->href)))) &&
	((xmlStrEqual(parent->name, allow1)) ||
	 (xmlStrEqual(parent->name, allow2)))) {
	return;
    }

    if (style->extInfos != NULL) {
	while ((parent != NULL) && (parent->type != XML_DOCUMENT_NODE)) {
	    /*
	     * if we are within an extension element all bets are off
	     * about the semantic there e.g. xsl:param within func:function
	     */
	    if ((parent->ns != NULL) &&
		(xmlHashLookup(style->extInfos, parent->ns->href) != NULL))
		return;

	    parent = parent->parent;
	}
    }
    xsltTransformError(NULL, style, inst,
		       "element %s is not allowed within that context\n",
		       inst->name);
    style->errors++;
}