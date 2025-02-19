XML_GetBuffer(XML_Parser parser, int len)
{
  if (len < 0) {
    errorCode = XML_ERROR_NO_MEMORY;
    return NULL;
  }
  switch (ps_parsing) {
  case XML_SUSPENDED:
    errorCode = XML_ERROR_SUSPENDED;
    return NULL;
  case XML_FINISHED:
    errorCode = XML_ERROR_FINISHED;
    return NULL;
  default: ;
  }

  if (len > bufferLim - bufferEnd) {
    int neededSize = len + (int)(bufferEnd - bufferPtr);
    if (neededSize < 0) {
      errorCode = XML_ERROR_NO_MEMORY;
      return NULL;
    }
#ifdef XML_CONTEXT_BYTES
    int keep = (int)(bufferPtr - buffer);

    if (keep > XML_CONTEXT_BYTES)
      keep = XML_CONTEXT_BYTES;
    neededSize += keep;
#endif  /* defined XML_CONTEXT_BYTES */
    if (neededSize  <= bufferLim - buffer) {
#ifdef XML_CONTEXT_BYTES
      if (keep < bufferPtr - buffer) {
        int offset = (int)(bufferPtr - buffer) - keep;
        memmove(buffer, &buffer[offset], bufferEnd - bufferPtr + keep);
        bufferEnd -= offset;
        bufferPtr -= offset;
      }
#else
      memmove(buffer, bufferPtr, bufferEnd - bufferPtr);
      bufferEnd = buffer + (bufferEnd - bufferPtr);
      bufferPtr = buffer;
#endif  /* not defined XML_CONTEXT_BYTES */
    }
    else {
      char *newBuf;
      int bufferSize = (int)(bufferLim - bufferPtr);
      if (bufferSize == 0)
        bufferSize = INIT_BUFFER_SIZE;
      do {
        bufferSize *= 2;
      } while (bufferSize < neededSize && bufferSize > 0);
      if (bufferSize <= 0) {
        errorCode = XML_ERROR_NO_MEMORY;
        return NULL;
      }
      newBuf = (char *)MALLOC(bufferSize);
      if (newBuf == 0) {
        errorCode = XML_ERROR_NO_MEMORY;
        return NULL;
      }
      bufferLim = newBuf + bufferSize;
#ifdef XML_CONTEXT_BYTES
      if (bufferPtr) {
        int keep = (int)(bufferPtr - buffer);
        if (keep > XML_CONTEXT_BYTES)
          keep = XML_CONTEXT_BYTES;
        memcpy(newBuf, &bufferPtr[-keep], bufferEnd - bufferPtr + keep);
        FREE(buffer);
        buffer = newBuf;
        bufferEnd = buffer + (bufferEnd - bufferPtr) + keep;
        bufferPtr = buffer + keep;
      }
      else {
        bufferEnd = newBuf + (bufferEnd - bufferPtr);
        bufferPtr = buffer = newBuf;
      }
#else
      if (bufferPtr) {
        memcpy(newBuf, bufferPtr, bufferEnd - bufferPtr);
        FREE(buffer);
      }
      bufferEnd = newBuf + (bufferEnd - bufferPtr);
      bufferPtr = buffer = newBuf;
#endif  /* not defined XML_CONTEXT_BYTES */
    }
    eventPtr = eventEndPtr = NULL;
    positionPtr = NULL;
  }
  return bufferEnd;
}