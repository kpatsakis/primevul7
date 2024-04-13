XML_SetEndElementHandler(XML_Parser parser,
                         XML_EndElementHandler end) {
  endElementHandler = end;
}