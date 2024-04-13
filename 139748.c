DU_cfindStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    switch (statusCode) {
      case STATUS_Success:
          s = "Success";
          break;
      case STATUS_Pending:
          s = "Pending";
          break;
      case STATUS_FIND_Refused_OutOfResources:
          s = "Refused: OutOfResources";
          break;
      case STATUS_FIND_Refused_SOPClassNotSupported:
          s = "Refused: SOPClassNotSupported";
          break;
      case STATUS_FIND_Failed_IdentifierDoesNotMatchSOPClass:
          s = "Failed: IdentifierDoesNotMatchSOPClass";
          break;
      case STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest:
          s = "Cancel: MatchingTerminatedDueToCancelRequest";
          break;
      case STATUS_FIND_Pending_WarningUnsupportedOptionalKeys:
          s = "Pending: WarningUnsupportedOptionalKeys";
          break;
    }
    if (s)
        return s;

    switch (statusCode & 0xf000) {      /* high nibble significant */
    case STATUS_FIND_Failed_UnableToProcess:    /* high nibble */
        s = "Failed: UnableToProcess";
        break;
    }

    if (s == NULL) {
        sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
        s = staticBuf;
    }
    return s;
}