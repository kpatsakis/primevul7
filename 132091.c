xmlSAXParseFileWithData(xmlSAXHandlerPtr sax, const char *filename,
                        int recovery, void *data) {
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt;

    xmlInitParser();

    ctxt = xmlCreateFileParserCtxt(filename);
    if (ctxt == NULL) {
	return(NULL);
    }
    if (sax != NULL) {
	if (ctxt->sax != NULL)
	    xmlFree(ctxt->sax);
        ctxt->sax = sax;
    }
    xmlDetectSAX2(ctxt);
    if (data!=NULL) {
	ctxt->_private = data;
    }

    if (ctxt->directory == NULL)
        ctxt->directory = xmlParserGetDirectory(filename);

    ctxt->recovery = recovery;

    xmlParseDocument(ctxt);

    if ((ctxt->wellFormed) || recovery) {
        ret = ctxt->myDoc;
	if (ret != NULL) {
	    if (ctxt->input->buf->compressed > 0)
		ret->compression = 9;
	    else
		ret->compression = ctxt->input->buf->compressed;
	}
    }
    else {
       ret = NULL;
       xmlFreeDoc(ctxt->myDoc);
       ctxt->myDoc = NULL;
    }
    if (sax != NULL)
        ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);

    return(ret);
}