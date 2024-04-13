XML_SetUnknownEncodingHandler(XML_Parser parser,
                              XML_UnknownEncodingHandler handler,
                              void *data)
{
  unknownEncodingHandler = handler;
  unknownEncodingHandlerData = data;
}