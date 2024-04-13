XML_GetCurrentByteCount(XML_Parser parser)
{
  if (eventEndPtr && eventPtr)
    return (int)(eventEndPtr - eventPtr);
  return 0;
}