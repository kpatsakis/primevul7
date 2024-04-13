xsltXPathFunctionLookup (xmlXPathContextPtr ctxt,
			 const xmlChar *name, const xmlChar *ns_uri) {
    xmlXPathFunction ret;

    if ((ctxt == NULL) || (name == NULL) || (ns_uri == NULL))
	return (NULL);

#ifdef WITH_XSLT_DEBUG_FUNCTION
    xsltGenericDebug(xsltGenericDebugContext,
            "Lookup function {%s}%s\n", ns_uri, name);
#endif

    /* give priority to context-level functions */
    /*
    ret = (xmlXPathFunction) xmlHashLookup2(ctxt->funcHash, name, ns_uri);
    */
    XML_CAST_FPTR(ret) = xmlHashLookup2(ctxt->funcHash, name, ns_uri);

    if (ret == NULL)
	ret = xsltExtModuleFunctionLookup(name, ns_uri);

#ifdef WITH_XSLT_DEBUG_FUNCTION
    if (ret != NULL)
        xsltGenericDebug(xsltGenericDebugContext,
            "found function %s\n", name);
#endif
    return(ret);
}