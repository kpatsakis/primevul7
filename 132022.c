xmlCreatePushParserCtxt(xmlSAXHandlerPtr sax, void *user_data,
                        const char *chunk, int size, const char *filename) {
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr inputStream;
    xmlParserInputBufferPtr buf;
    xmlCharEncoding enc = XML_CHAR_ENCODING_NONE;

    /*
     * plug some encoding conversion routines
     */
    if ((chunk != NULL) && (size >= 4))
	enc = xmlDetectCharEncoding((const xmlChar *) chunk, size);

    buf = xmlAllocParserInputBuffer(enc);
    if (buf == NULL) return(NULL);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
        xmlErrMemory(NULL, "creating parser: out of memory\n");
	xmlFreeParserInputBuffer(buf);
	return(NULL);
    }
    ctxt->dictNames = 1;
    ctxt->pushTab = (void **) xmlMalloc(ctxt->nameMax * 3 * sizeof(xmlChar *));
    if (ctxt->pushTab == NULL) {
        xmlErrMemory(ctxt, NULL);
	xmlFreeParserInputBuffer(buf);
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }
    if (sax != NULL) {
#ifdef LIBXML_SAX1_ENABLED
	if (ctxt->sax != (xmlSAXHandlerPtr) &xmlDefaultSAXHandler)
#endif /* LIBXML_SAX1_ENABLED */
	    xmlFree(ctxt->sax);
	ctxt->sax = (xmlSAXHandlerPtr) xmlMalloc(sizeof(xmlSAXHandler));
	if (ctxt->sax == NULL) {
	    xmlErrMemory(ctxt, NULL);
	    xmlFreeParserInputBuffer(buf);
	    xmlFreeParserCtxt(ctxt);
	    return(NULL);
	}
	memset(ctxt->sax, 0, sizeof(xmlSAXHandler));
	if (sax->initialized == XML_SAX2_MAGIC)
	    memcpy(ctxt->sax, sax, sizeof(xmlSAXHandler));
	else
	    memcpy(ctxt->sax, sax, sizeof(xmlSAXHandlerV1));
	if (user_data != NULL)
	    ctxt->userData = user_data;
    }
    if (filename == NULL) {
	ctxt->directory = NULL;
    } else {
        ctxt->directory = xmlParserGetDirectory(filename);
    }

    inputStream = xmlNewInputStream(ctxt);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(ctxt);
	xmlFreeParserInputBuffer(buf);
	return(NULL);
    }

    if (filename == NULL)
	inputStream->filename = NULL;
    else {
	inputStream->filename = (char *)
	    xmlCanonicPath((const xmlChar *) filename);
	if (inputStream->filename == NULL) {
	    xmlFreeParserCtxt(ctxt);
	    xmlFreeParserInputBuffer(buf);
	    return(NULL);
	}
    }
    inputStream->buf = buf;
    xmlBufResetInput(inputStream->buf->buffer, inputStream);
    inputPush(ctxt, inputStream);

    /*
     * If the caller didn't provide an initial 'chunk' for determining
     * the encoding, we set the context to XML_CHAR_ENCODING_NONE so
     * that it can be automatically determined later
     */
    if ((size == 0) || (chunk == NULL)) {
	ctxt->charset = XML_CHAR_ENCODING_NONE;
    } else if ((ctxt->input != NULL) && (ctxt->input->buf != NULL)) {
	size_t base = xmlBufGetInputBase(ctxt->input->buf->buffer, ctxt->input);
	size_t cur = ctxt->input->cur - ctxt->input->base;

	xmlParserInputBufferPush(ctxt->input->buf, size, chunk);

        xmlBufSetInputBaseCur(ctxt->input->buf->buffer, ctxt->input, base, cur);
#ifdef DEBUG_PUSH
	xmlGenericError(xmlGenericErrorContext, "PP: pushed %d\n", size);
#endif
    }

    if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }

    return(ctxt);
}