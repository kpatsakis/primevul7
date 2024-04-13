XML_SetStartElementHandler(XML_Parser parser,
                           XML_StartElementHandler start) {
  startElementHandler = start;
}