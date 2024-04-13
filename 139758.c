DU_cstoreStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    switch (statusCode) {
      case STATUS_Success:
          s = "Success";
          break;
      case STATUS_STORE_Refused_SOPClassNotSupported:
          s = "Refused: SOPClassNotSupported";
          break;
      case STATUS_STORE_Warning_CoercionOfDataElements:
          s = "Warning: CoercionOfDataElements";
          break;
      case STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass:
          s = "Warning: DataSetDoesNotMatchSOPClass";
          break;
      case STATUS_STORE_Warning_ElementsDiscarded:
          s = "Warning: ElementsDiscarded";
          break;
    }
    if (s)
        return s;

    switch (statusCode & 0xff00) {      /* high byte significant */
      case STATUS_STORE_Refused_OutOfResources:   /* high byte */
          s = "Refused: OutOfResources";
          break;
      case STATUS_STORE_Error_DataSetDoesNotMatchSOPClass:        /* high byte */
          s = "Error: DataSetDoesNotMatchSOPClass";
          break;
    }
    if (s)
        return s;

    switch (statusCode & 0xf000) {      /* high nibble significant */
      case STATUS_STORE_Error_CannotUnderstand:   /* high nibble */
          s = "Error: CannotUnderstand";
          break;
    }

    if (s == NULL) {
        sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
        s = staticBuf;
    }
    return s;
}