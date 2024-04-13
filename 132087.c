xmlSetupParserForBuffer(xmlParserCtxtPtr ctxt, const xmlChar* buffer,
                             const char* filename)
{
    xmlParserInputPtr input;

    if ((ctxt == NULL) || (buffer == NULL))
        return;

    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
        xmlErrMemory(NULL, "parsing new buffer: out of memory\n");
        xmlClearParserCtxt(ctxt);
        return;
    }

    xmlClearParserCtxt(ctxt);
    if (filename != NULL)
        input->filename = (char *) xmlCanonicPath((const xmlChar *)filename);
    input->base = buffer;
    input->cur = buffer;
    input->end = &buffer[xmlStrlen(buffer)];
    inputPush(ctxt, input);
}