xmlCtxtResetPush(xmlParserCtxtPtr ctxt, const char *chunk,
                 int size, const char *filename, const char *encoding)
{
    xmlParserInputPtr inputStream;
    xmlParserInputBufferPtr buf;
    xmlCharEncoding enc = XML_CHAR_ENCODING_NONE;

    if (ctxt == NULL)
        return(1);

    if ((encoding == NULL) && (chunk != NULL) && (size >= 4))
        enc = xmlDetectCharEncoding((const xmlChar *) chunk, size);

    buf = xmlAllocParserInputBuffer(enc);
    if (buf == NULL)
        return(1);

    if (ctxt == NULL) {
        xmlFreeParserInputBuffer(buf);
        return(1);
    }

    xmlCtxtReset(ctxt);

    if (ctxt->pushTab == NULL) {
        ctxt->pushTab = (void **) xmlMalloc(ctxt->nameMax * 3 *
	                                    sizeof(xmlChar *));
        if (ctxt->pushTab == NULL) {
	    xmlErrMemory(ctxt, NULL);
            xmlFreeParserInputBuffer(buf);
            return(1);
        }
    }

    if (filename == NULL) {
        ctxt->directory = NULL;
    } else {
        ctxt->directory = xmlParserGetDirectory(filename);
    }

    inputStream = xmlNewInputStream(ctxt);
    if (inputStream == NULL) {
        xmlFreeParserInputBuffer(buf);
        return(1);
    }

    if (filename == NULL)
        inputStream->filename = NULL;
    else
        inputStream->filename = (char *)
            xmlCanonicPath((const xmlChar *) filename);
    inputStream->buf = buf;
    xmlBufResetInput(buf->buffer, inputStream);

    inputPush(ctxt, inputStream);

    if ((size > 0) && (chunk != NULL) && (ctxt->input != NULL) &&
        (ctxt->input->buf != NULL)) {
	size_t base = xmlBufGetInputBase(ctxt->input->buf->buffer, ctxt->input);
        size_t cur = ctxt->input->cur - ctxt->input->base;

        xmlParserInputBufferPush(ctxt->input->buf, size, chunk);

        xmlBufSetInputBaseCur(ctxt->input->buf->buffer, ctxt->input, base, cur);
#ifdef DEBUG_PUSH
        xmlGenericError(xmlGenericErrorContext, "PP: pushed %d\n", size);
#endif
    }

    if (encoding != NULL) {
        xmlCharEncodingHandlerPtr hdlr;

        if (ctxt->encoding != NULL)
	    xmlFree((xmlChar *) ctxt->encoding);
        ctxt->encoding = xmlStrdup((const xmlChar *) encoding);

        hdlr = xmlFindCharEncodingHandler(encoding);
        if (hdlr != NULL) {
            xmlSwitchToEncoding(ctxt, hdlr);
	} else {
	    xmlFatalErrMsgStr(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			      "Unsupported encoding %s\n", BAD_CAST encoding);
        }
    } else if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }

    return(0);
}