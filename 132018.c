xmlIsNameChar(xmlParserCtxtPtr ctxt, int c) {
    if ((ctxt->options & XML_PARSE_OLD10) == 0) {
        /*
	 * Use the new checks of production [4] [4a] amd [5] of the
	 * Update 5 of XML-1.0
	 */
	if ((c != ' ') && (c != '>') && (c != '/') && /* accelerators */
	    (((c >= 'a') && (c <= 'z')) ||
	     ((c >= 'A') && (c <= 'Z')) ||
	     ((c >= '0') && (c <= '9')) || /* !start */
	     (c == '_') || (c == ':') ||
	     (c == '-') || (c == '.') || (c == 0xB7) || /* !start */
	     ((c >= 0xC0) && (c <= 0xD6)) ||
	     ((c >= 0xD8) && (c <= 0xF6)) ||
	     ((c >= 0xF8) && (c <= 0x2FF)) ||
	     ((c >= 0x300) && (c <= 0x36F)) || /* !start */
	     ((c >= 0x370) && (c <= 0x37D)) ||
	     ((c >= 0x37F) && (c <= 0x1FFF)) ||
	     ((c >= 0x200C) && (c <= 0x200D)) ||
	     ((c >= 0x203F) && (c <= 0x2040)) || /* !start */
	     ((c >= 0x2070) && (c <= 0x218F)) ||
	     ((c >= 0x2C00) && (c <= 0x2FEF)) ||
	     ((c >= 0x3001) && (c <= 0xD7FF)) ||
	     ((c >= 0xF900) && (c <= 0xFDCF)) ||
	     ((c >= 0xFDF0) && (c <= 0xFFFD)) ||
	     ((c >= 0x10000) && (c <= 0xEFFFF))))
	     return(1);
    } else {
        if ((IS_LETTER(c)) || (IS_DIGIT(c)) ||
            (c == '.') || (c == '-') ||
	    (c == '_') || (c == ':') ||
	    (IS_COMBINING(c)) ||
	    (IS_EXTENDER(c)))
	    return(1);
    }
    return(0);
}