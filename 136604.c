XML_GetParsingStatus(XML_Parser parser, XML_ParsingStatus *status)
{
  assert(status != NULL);
  *status = parser->m_parsingStatus;
}