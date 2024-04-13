xmlStopParser(xmlParserCtxtPtr ctxt) {
    if (ctxt == NULL)
        return;
    xmlHaltParser(ctxt);
    ctxt->errNo = XML_ERR_USER_STOP;
}