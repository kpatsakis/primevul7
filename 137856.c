exsltSaxonExpressionFunction (xmlXPathParserContextPtr ctxt, int nargs) {
    xmlChar *arg;
    xmlXPathCompExprPtr ret;
    xmlHashTablePtr hash;
    xsltTransformContextPtr tctxt = xsltXPathGetTransformContext(ctxt);

    if (nargs != 1) {
	xmlXPathSetArityError(ctxt);
	return;
    }

    arg = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (arg == NULL)) {
	xmlXPathSetTypeError(ctxt);
	return;
    }

    hash = (xmlHashTablePtr) xsltGetExtData(tctxt,
					    ctxt->context->functionURI);

    ret = xmlHashLookup(hash, arg);

    if (ret == NULL) {
	 ret = xmlXPathCompile(arg);
	 if (ret == NULL) {
	      xmlFree(arg);
              xmlXPathSetError(ctxt, XPATH_EXPR_ERROR);
	      return;
	 }
	 xmlHashAddEntry(hash, arg, (void *) ret);
    }

    xmlFree(arg);

    xmlXPathReturnExternal(ctxt, ret);
}