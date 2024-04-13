xsltDocumentComp(xsltStylesheetPtr style, xmlNodePtr inst,
		 xsltTransformFunction function ATTRIBUTE_UNUSED) {
#ifdef XSLT_REFACTORED
    xsltStyleItemDocumentPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif
    const xmlChar *filename = NULL;

    /*
    * As of 2006-03-30, this function is currently defined in Libxslt
    * to be used for:
    * (in libxslt/extra.c)
    * "output" in XSLT_SAXON_NAMESPACE
    * "write" XSLT_XALAN_NAMESPACE
    * "document" XSLT_XT_NAMESPACE
    * "document" XSLT_NAMESPACE (from the abandoned old working
    *                            draft of XSLT 1.1)
    * (in libexslt/common.c)
    * "document" in EXSLT_COMMON_NAMESPACE
    */
#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemDocumentPtr)
	xsltNewStylePreComp(style, XSLT_FUNC_DOCUMENT);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_DOCUMENT);
#endif

    if (comp == NULL)
	return (NULL);
    comp->inst = inst;
    comp->ver11 = 0;

    if (xmlStrEqual(inst->name, (const xmlChar *) "output")) {
#ifdef WITH_XSLT_DEBUG_EXTRA
	xsltGenericDebug(xsltGenericDebugContext,
	    "Found saxon:output extension\n");
#endif
	/*
	* The element "output" is in the namespace XSLT_SAXON_NAMESPACE
	*   (http://icl.com/saxon)
	* The @file is in no namespace; it is an AVT.
	*   (http://www.computerwizards.com/saxon/doc/extensions.html#saxon:output)
	*
	* TODO: Do we need not to check the namespace here?
	*/
	filename = xsltEvalStaticAttrValueTemplate(style, inst,
			 (const xmlChar *)"file",
			 NULL, &comp->has_filename);
    } else if (xmlStrEqual(inst->name, (const xmlChar *) "write")) {
#ifdef WITH_XSLT_DEBUG_EXTRA
	xsltGenericDebug(xsltGenericDebugContext,
	    "Found xalan:write extension\n");
#endif
	/* the filename need to be interpreted */
	/*
	* TODO: Is "filename need to be interpreted" meant to be a todo?
	*   Where will be the filename of xalan:write be processed?
	*
	* TODO: Do we need not to check the namespace here?
	*   The extension ns is "http://xml.apache.org/xalan/redirect".
	*   See http://xml.apache.org/xalan-j/extensionslib.html.
	*/
    } else if (xmlStrEqual(inst->name, (const xmlChar *) "document")) {
	if (inst->ns != NULL) {
	    if (xmlStrEqual(inst->ns->href, XSLT_NAMESPACE)) {
		/*
		* Mark the instruction as being of
		* XSLT version 1.1 (abandoned).
		*/
		comp->ver11 = 1;
#ifdef WITH_XSLT_DEBUG_EXTRA
		xsltGenericDebug(xsltGenericDebugContext,
		    "Found xslt11:document construct\n");
#endif
	    } else {
		if (xmlStrEqual(inst->ns->href,
		    (const xmlChar *)"http://exslt.org/common")) {
		    /* EXSLT. */
#ifdef WITH_XSLT_DEBUG_EXTRA
		    xsltGenericDebug(xsltGenericDebugContext,
			"Found exslt:document extension\n");
#endif
		} else if (xmlStrEqual(inst->ns->href, XSLT_XT_NAMESPACE)) {
		    /* James Clark's XT. */
#ifdef WITH_XSLT_DEBUG_EXTRA
		    xsltGenericDebug(xsltGenericDebugContext,
			"Found xt:document extension\n");
#endif
		}
	    }
	}
	/*
	* The element "document" is used in conjunction with the
	* following namespaces:
	*
	* 1) XSLT_NAMESPACE (http://www.w3.org/1999/XSL/Transform version 1.1)
	*    <!ELEMENT xsl:document %template;>
	*    <!ATTLIST xsl:document
	*       href %avt; #REQUIRED
	*    @href is an AVT
	*    IMPORTANT: xsl:document was in the abandoned XSLT 1.1 draft,
	*    it was removed and isn't available in XSLT 1.1 anymore.
	*    In XSLT 2.0 it was renamed to xsl:result-document.
	*
	*   All other attributes are identical to the attributes
	*   on xsl:output
	*
	* 2) EXSLT_COMMON_NAMESPACE (http://exslt.org/common)
	*    <exsl:document
	*       href = { uri-reference }
	*    TODO: is @href is an AVT?
	*
	* 3) XSLT_XT_NAMESPACE (http://www.jclark.com/xt)
	*     Example: <xt:document method="xml" href="myFile.xml">
	*    TODO: is @href is an AVT?
	*
	* In all cases @href is in no namespace.
	*/
	filename = xsltEvalStaticAttrValueTemplate(style, inst,
	    (const xmlChar *)"href", NULL, &comp->has_filename);
    }
    if (!comp->has_filename) {
	goto error;
    }
    comp->filename = filename;

error:
    return ((xsltElemPreCompPtr) comp);
}