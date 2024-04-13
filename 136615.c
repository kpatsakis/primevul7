XML_SetDefaultHandler(XML_Parser parser,
                      XML_DefaultHandler handler)
{
  defaultHandler = handler;
  defaultExpandInternalEntities = XML_FALSE;
}