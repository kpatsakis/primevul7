XML_SetReturnNSTriplet(XML_Parser parser, int do_nst)
{
  /* block after XML_Parse()/XML_ParseBuffer() has been called */
  if (ps_parsing == XML_PARSING || ps_parsing == XML_SUSPENDED)
    return;
  ns_triplets = do_nst ? XML_TRUE : XML_FALSE;
}