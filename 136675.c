ignoreSectionProcessor(XML_Parser parser,
                       const char *start,
                       const char *end,
                       const char **endPtr)
{
  enum XML_Error result = doIgnoreSection(parser, encoding, &start, end,
                                          endPtr, (XML_Bool)!ps_finalBuffer);
  if (result != XML_ERROR_NONE)
    return result;
  if (start) {
    processor = prologProcessor;
    return prologProcessor(parser, start, end, endPtr);
  }
  return result;
}