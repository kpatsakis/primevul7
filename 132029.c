nsPush(xmlParserCtxtPtr ctxt, const xmlChar *prefix, const xmlChar *URL)
{
    if (ctxt->options & XML_PARSE_NSCLEAN) {
        int i;
	for (i = ctxt->nsNr - 2;i >= 0;i -= 2) {
	    if (ctxt->nsTab[i] == prefix) {
		/* in scope */
	        if (ctxt->nsTab[i + 1] == URL)
		    return(-2);
		/* out of scope keep it */
		break;
	    }
	}
    }
    if ((ctxt->nsMax == 0) || (ctxt->nsTab == NULL)) {
	ctxt->nsMax = 10;
	ctxt->nsNr = 0;
	ctxt->nsTab = (const xmlChar **)
	              xmlMalloc(ctxt->nsMax * sizeof(xmlChar *));
	if (ctxt->nsTab == NULL) {
	    xmlErrMemory(ctxt, NULL);
	    ctxt->nsMax = 0;
            return (-1);
	}
    } else if (ctxt->nsNr >= ctxt->nsMax) {
        const xmlChar ** tmp;
        ctxt->nsMax *= 2;
        tmp = (const xmlChar **) xmlRealloc((char *) ctxt->nsTab,
				    ctxt->nsMax * sizeof(ctxt->nsTab[0]));
        if (tmp == NULL) {
            xmlErrMemory(ctxt, NULL);
	    ctxt->nsMax /= 2;
            return (-1);
        }
	ctxt->nsTab = tmp;
    }
    ctxt->nsTab[ctxt->nsNr++] = prefix;
    ctxt->nsTab[ctxt->nsNr++] = URL;
    return (ctxt->nsNr);
}