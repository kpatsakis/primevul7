DU_ncreateStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    switch (statusCode) {
      case STATUS_Success:
          s = "Success";
          break;
      case STATUS_N_ClassInstanceConflict:
          s = "Failure: ClassInstanceConflict";
          break;
      case STATUS_N_DuplicateInvocation:
          s = "Failure: DuplicateInvocation";
          break;
      case STATUS_N_DuplicateSOPInstance:
          s = "Failure: DuplicateSOPInstance";
          break;
      case STATUS_N_InvalidAttributeValue:
          s = "Failure: InvalidAttributeValue";
          break;
      case STATUS_N_InvalidObjectInstance:
          s = "Failure: InvalidObjectInstance";
          break;
      case STATUS_N_MissingAttribute:
          s = "Failure: MissingAttribute";
          break;
      case STATUS_N_MissingAttributeValue:
          s = "Failure: MissingAttributeValue";
          break;
      case STATUS_N_MistypedArgument:
          s = "Failure: MistypedArgument";
          break;
      case STATUS_N_NoSuchAttribute:
          s = "Failure: NoSuchAttribute";
          break;
      case STATUS_N_NoSuchSOPClass:
          s = "Failure: NoSuchSOPClass";
          break;
      case STATUS_N_NoSuchObjectInstance:
          s = "Failure: NoSuchObjectInstance";
          break;
      case STATUS_N_ProcessingFailure:
          s = "Failure: ProcessingFailure";
          break;
      case STATUS_N_ResourceLimitation:
          s = "Failure: ResourceLimitation";
          break;
      case STATUS_N_UnrecognizedOperation:
          s = "Failure: UnrecognizedOperation";
          break;
      case STATUS_N_AttributeValueOutOfRange:
          s = "Warning: AttributeValueOutOfRange";
          break;
    }
    if (s)
          return s;

    switch (statusCode & 0xf000) {        /* high nibble significant */
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