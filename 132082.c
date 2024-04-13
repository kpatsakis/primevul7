xmlHaltParser(xmlParserCtxtPtr ctxt) {
    if (ctxt == NULL)
        return;
    ctxt->instate = XML_PARSER_EOF;
    ctxt->disableSAX = 1;
    if (ctxt->input != NULL) {
        /*
	 * in case there was a specific allocation deallocate before
	 * overriding base
	 */
        if (ctxt->input->free != NULL) {
	    ctxt->input->free((xmlChar *) ctxt->input->base);
	    ctxt->input->free = NULL;
	}
	ctxt->input->cur = BAD_CAST"";
	ctxt->input->base = ctxt->input->cur;
    }
}