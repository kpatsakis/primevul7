DU_cmoveStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    switch (statusCode) {
      case STATUS_Success:
          s = "Success";
          break;
      case STATUS_Pending:
          s = "Pending";
          break;
      case STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches:
          s = "Refused: OutOfResourcesNumberOfMatches";
          break;
      case STATUS_MOVE_Refused_OutOfResourcesSubOperations:
          s = "Refused: OutOfResourcesSubOperations";
          break;
      case STATUS_MOVE_Failed_SOPClassNotSupported:
          s = "Failed: SOPClassNotSupported";
          break;
      case STATUS_MOVE_Failed_MoveDestinationUnknown:
          s = "Failed: MoveDestinationUnknown";
          break;
      case STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass:
          s = "Failed: IdentifierDoesNotMatchSOPClass";
          break;
      case STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication:
          s = "Cancel: SubOperationsTerminatedDueToCancelIndication";
          break;
      case STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures:
          s = "Warning: SubOperationsCompleteOneOrMoreFailures";
          break;

    }
    if (s)
        return s;

    switch (statusCode & 0xf000) {      /* high nibble significant */
      case STATUS_MOVE_Failed_UnableToProcess:    /* high nibble */
          s = "Failed: UnableToProcess";
          break;
    }

    if (s == NULL) {
        sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
        s = staticBuf;
    }
    return s;
}