XML_SetBase(XML_Parser parser, const XML_Char *p)
{
  if (p) {
    p = poolCopyString(&_dtd->pool, p);
    if (!p)
      return XML_STATUS_ERROR;
    curBase = p;
  }
  else
    curBase = NULL;
  return XML_STATUS_OK;
}