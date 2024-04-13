xmlParseNCName(xmlParserCtxtPtr ctxt) {
    const xmlChar *in, *e;
    const xmlChar *ret;
    int count = 0;

#ifdef DEBUG
    nbParseNCName++;
#endif

    /*
     * Accelerator for simple ASCII names
     */
    in = ctxt->input->cur;
    e = ctxt->input->end;
    if ((((*in >= 0x61) && (*in <= 0x7A)) ||
	 ((*in >= 0x41) && (*in <= 0x5A)) ||
	 (*in == '_')) && (in < e)) {
	in++;
	while ((((*in >= 0x61) && (*in <= 0x7A)) ||
	        ((*in >= 0x41) && (*in <= 0x5A)) ||
	        ((*in >= 0x30) && (*in <= 0x39)) ||
	        (*in == '_') || (*in == '-') ||
	        (*in == '.')) && (in < e))
	    in++;
	if (in >= e)
	    goto complex;
	if ((*in > 0) && (*in < 0x80)) {
	    count = in - ctxt->input->cur;
            if ((count > XML_MAX_NAME_LENGTH) &&
                ((ctxt->options & XML_PARSE_HUGE) == 0)) {
                xmlFatalErr(ctxt, XML_ERR_NAME_TOO_LONG, "NCName");
                return(NULL);
            }
	    ret = xmlDictLookup(ctxt->dict, ctxt->input->cur, count);
	    ctxt->input->cur = in;
	    ctxt->nbChars += count;
	    ctxt->input->col += count;
	    if (ret == NULL) {
	        xmlErrMemory(ctxt, NULL);
	    }
	    return(ret);
	}
    }
complex:
    return(xmlParseNCNameComplex(ctxt));
}