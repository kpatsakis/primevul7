xsltCheckTopLevelElement(xsltStylesheetPtr style, xmlNodePtr inst, int err) {
    xmlNodePtr parent;
    if ((style == NULL) || (inst == NULL) || (inst->ns == NULL))
        return(-1);

    parent = inst->parent;
    if (parent == NULL) {
        if (err) {
	    xsltTransformError(NULL, style, inst,
		    "internal problem: element has no parent\n");
	    style->errors++;
	}
	return(0);
    }
    if ((parent->ns == NULL) || (parent->type != XML_ELEMENT_NODE) ||
        ((parent->ns != inst->ns) &&
	 (!xmlStrEqual(parent->ns->href, inst->ns->href))) ||
	((!xmlStrEqual(parent->name, BAD_CAST "stylesheet")) &&
	 (!xmlStrEqual(parent->name, BAD_CAST "transform")))) {
	if (err) {
	    xsltTransformError(NULL, style, inst,
		    "element %s only allowed as child of stylesheet\n",
			       inst->name);
	    style->errors++;
	}
	return(0);
    }
    return(1);
}