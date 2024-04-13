XML_SetExternalEntityRefHandlerArg(XML_Parser parser, void *arg)
{
  if (arg)
    externalEntityRefHandlerArg = (XML_Parser)arg;
  else
    externalEntityRefHandlerArg = parser;
}