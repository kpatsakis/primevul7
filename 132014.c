xmlNewBlanksWrapperInputStream(xmlParserCtxtPtr ctxt, xmlEntityPtr entity) {
    xmlParserInputPtr input;
    xmlChar *buffer;
    size_t length;
    if (entity == NULL) {
	xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR,
	            "xmlNewBlanksWrapperInputStream entity\n");
	return(NULL);
    }
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"new blanks wrapper for entity: %s\n", entity->name);
    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
	return(NULL);
    }
    length = xmlStrlen(entity->name) + 5;
    buffer = xmlMallocAtomic(length);
    if (buffer == NULL) {
	xmlErrMemory(ctxt, NULL);
        xmlFree(input);
	return(NULL);
    }
    buffer [0] = ' ';
    buffer [1] = '%';
    buffer [length-3] = ';';
    buffer [length-2] = ' ';
    buffer [length-1] = 0;
    memcpy(buffer + 2, entity->name, length - 5);
    input->free = deallocblankswrapper;
    input->base = buffer;
    input->cur = buffer;
    input->length = length;
    input->end = &buffer[length];
    return(input);
}