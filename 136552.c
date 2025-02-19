XML_Parse(XML_Parser parser, const char *s, int len, int isFinal)
{
  switch (ps_parsing) {
  case XML_SUSPENDED:
    errorCode = XML_ERROR_SUSPENDED;
    return XML_STATUS_ERROR;
  case XML_FINISHED:
    errorCode = XML_ERROR_FINISHED;
    return XML_STATUS_ERROR;
  case XML_INITIALIZED:
    if (parentParser == NULL && !startParsing(parser)) {
      errorCode = XML_ERROR_NO_MEMORY;
      return XML_STATUS_ERROR;
    }
  default:
    ps_parsing = XML_PARSING;
  }

  if (len == 0) {
    ps_finalBuffer = (XML_Bool)isFinal;
    if (!isFinal)
      return XML_STATUS_OK;
    positionPtr = bufferPtr;
    parseEndPtr = bufferEnd;

    /* If data are left over from last buffer, and we now know that these
       data are the final chunk of input, then we have to check them again
       to detect errors based on that fact.
    */
    errorCode = processor(parser, bufferPtr, parseEndPtr, &bufferPtr);

    if (errorCode == XML_ERROR_NONE) {
      switch (ps_parsing) {
      case XML_SUSPENDED:
        XmlUpdatePosition(encoding, positionPtr, bufferPtr, &position);
        positionPtr = bufferPtr;
        return XML_STATUS_SUSPENDED;
      case XML_INITIALIZED:
      case XML_PARSING:
        ps_parsing = XML_FINISHED;
        /* fall through */
      default:
        return XML_STATUS_OK;
      }
    }
    eventEndPtr = eventPtr;
    processor = errorProcessor;
    return XML_STATUS_ERROR;
  }
#ifndef XML_CONTEXT_BYTES
  else if (bufferPtr == bufferEnd) {
    const char *end;
    int nLeftOver;
    enum XML_Error result;
    parseEndByteIndex += len;
    positionPtr = s;
    ps_finalBuffer = (XML_Bool)isFinal;

    errorCode = processor(parser, s, parseEndPtr = s + len, &end);

    if (errorCode != XML_ERROR_NONE) {
      eventEndPtr = eventPtr;
      processor = errorProcessor;
      return XML_STATUS_ERROR;
    }
    else {
      switch (ps_parsing) {
      case XML_SUSPENDED:
        result = XML_STATUS_SUSPENDED;
        break;
      case XML_INITIALIZED:
      case XML_PARSING:
        if (isFinal) {
          ps_parsing = XML_FINISHED;
          return XML_STATUS_OK;
        }
      /* fall through */
      default:
        result = XML_STATUS_OK;
      }
    }

    XmlUpdatePosition(encoding, positionPtr, end, &position);
    nLeftOver = s + len - end;
    if (nLeftOver) {
      if (buffer == NULL || nLeftOver > bufferLim - buffer) {
        /* FIXME avoid integer overflow */
        char *temp;
        temp = (buffer == NULL
                ? (char *)MALLOC(len * 2)
                : (char *)REALLOC(buffer, len * 2));
        if (temp == NULL) {
          errorCode = XML_ERROR_NO_MEMORY;
          eventPtr = eventEndPtr = NULL;
          processor = errorProcessor;
          return XML_STATUS_ERROR;
        }
        buffer = temp;
        bufferLim = buffer + len * 2;
      }
      memcpy(buffer, end, nLeftOver);
    }
    bufferPtr = buffer;
    bufferEnd = buffer + nLeftOver;
    positionPtr = bufferPtr;
    parseEndPtr = bufferEnd;
    eventPtr = bufferPtr;
    eventEndPtr = bufferPtr;
    return result;
  }
#endif  /* not defined XML_CONTEXT_BYTES */
  else {
    void *buff = XML_GetBuffer(parser, len);
    if (buff == NULL)
      return XML_STATUS_ERROR;
    else {
      memcpy(buff, s, len);
      return XML_ParseBuffer(parser, len, isFinal);
    }
  }
}