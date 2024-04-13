xsltElementAvailableFunction(xmlXPathParserContextPtr ctxt, int nargs){
    xmlXPathObjectPtr obj;
    xmlChar *prefix, *name;
    const xmlChar *nsURI = NULL;
    xsltTransformContextPtr tctxt;

    if (nargs != 1) {
	xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
		"element-available() : expects one string arg\n");
	ctxt->error = XPATH_INVALID_ARITY;
	return;
    }
    xmlXPathStringFunction(ctxt, 1);
    if ((ctxt->value == NULL) || (ctxt->value->type != XPATH_STRING)) {
	xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
	    "element-available() : invalid arg expecting a string\n");
	ctxt->error = XPATH_INVALID_TYPE;
	return;
    }
    obj = valuePop(ctxt);
    tctxt = xsltXPathGetTransformContext(ctxt);
    if (tctxt == NULL) {
	xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
		"element-available() : internal error tctxt == NULL\n");
	xmlXPathFreeObject(obj);
	valuePush(ctxt, xmlXPathNewBoolean(0));
	return;
    }


    name = xmlSplitQName2(obj->stringval, &prefix);
    if (name == NULL) {
	xmlNsPtr ns;

	name = xmlStrdup(obj->stringval);
	ns = xmlSearchNs(tctxt->inst->doc, tctxt->inst, NULL);
	if (ns != NULL) nsURI = xmlStrdup(ns->href);
    } else {
	nsURI = xmlXPathNsLookup(ctxt->context, prefix);
	if (nsURI == NULL) {
	    xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
		"element-available() : prefix %s is not bound\n", prefix);
	}
    }

    if (xsltExtElementLookup(tctxt, name, nsURI) != NULL) {
	valuePush(ctxt, xmlXPathNewBoolean(1));
    } else {
	valuePush(ctxt, xmlXPathNewBoolean(0));
    }

    xmlXPathFreeObject(obj);
    if (name != NULL)
	xmlFree(name);
    if (prefix != NULL)
	xmlFree(prefix);
}