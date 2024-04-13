xmlParseLookupSequence(xmlParserCtxtPtr ctxt, xmlChar first,
                       xmlChar next, xmlChar third) {
    int base, len;
    xmlParserInputPtr in;
    const xmlChar *buf;

    in = ctxt->input;
    if (in == NULL) return(-1);
    base = in->cur - in->base;
    if (base < 0) return(-1);
    if (ctxt->checkIndex > base)
        base = ctxt->checkIndex;
    if (in->buf == NULL) {
	buf = in->base;
	len = in->length;
    } else {
	buf = xmlBufContent(in->buf->buffer);
	len = xmlBufUse(in->buf->buffer);
    }
    /* take into account the sequence length */
    if (third) len -= 2;
    else if (next) len --;
    for (;base < len;base++) {
        if (buf[base] == first) {
	    if (third != 0) {
		if ((buf[base + 1] != next) ||
		    (buf[base + 2] != third)) continue;
	    } else if (next != 0) {
		if (buf[base + 1] != next) continue;
	    }
	    ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
	    if (next == 0)
		xmlGenericError(xmlGenericErrorContext,
			"PP: lookup '%c' found at %d\n",
			first, base);
	    else if (third == 0)
		xmlGenericError(xmlGenericErrorContext,
			"PP: lookup '%c%c' found at %d\n",
			first, next, base);
	    else
		xmlGenericError(xmlGenericErrorContext,
			"PP: lookup '%c%c%c' found at %d\n",
			first, next, third, base);
#endif
	    return(base - (in->cur - in->base));
	}
    }
    ctxt->checkIndex = base;
#ifdef DEBUG_PUSH
    if (next == 0)
	xmlGenericError(xmlGenericErrorContext,
		"PP: lookup '%c' failed\n", first);
    else if (third == 0)
	xmlGenericError(xmlGenericErrorContext,
		"PP: lookup '%c%c' failed\n", first, next);
    else
	xmlGenericError(xmlGenericErrorContext,
		"PP: lookup '%c%c%c' failed\n", first, next, third);
#endif
    return(-1);
}