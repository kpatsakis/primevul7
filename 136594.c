cdataSectionProcessor(XML_Parser parser,
                      const char *start,
                      const char *end,
                      const char **endPtr)
{
  enum XML_Error result = doCdataSection(parser, encoding, &start, end,
                                         endPtr, (XML_Bool)!ps_finalBuffer);
  if (result != XML_ERROR_NONE)
    return result;
  if (start) {
    if (parentParser) {  /* we are parsing an external entity */
      processor = externalEntityContentProcessor;
      return externalEntityContentProcessor(parser, start, end, endPtr);
    }
    else {
      processor = contentProcessor;
      return contentProcessor(parser, start, end, endPtr);
    }
  }
  return result;
}