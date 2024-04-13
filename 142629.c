snmp_errstring(int errstat)
{
    const char * const error_string[19] = {
        "(noError) No Error",
        "(tooBig) Response message would have been too large.",
        "(noSuchName) There is no such variable name in this MIB.",
        "(badValue) The value given has the wrong type or length.",
        "(readOnly) The two parties used do not have access to use the specified SNMP PDU.",
        "(genError) A general failure occured",
        "noAccess",
        "wrongType (The set datatype does not match the data type the agent expects)",
        "wrongLength (The set value has an illegal length from what the agent expects)",
        "wrongEncoding",
        "wrongValue (The set value is illegal or unsupported in some way)",
        "noCreation (That table does not support row creation or that object can not ever be created)",
        "inconsistentValue (The set value is illegal or unsupported in some way)",
        "resourceUnavailable (This is likely a out-of-memory failure within the agent)",
        "commitFailed",
        "undoFailed",
        "authorizationError (access denied to that object)",
        "notWritable (That object does not support modification)",
        "inconsistentName (That object can not currently be created)"
    };

    if (errstat <= MAX_SNMP_ERR && errstat >= SNMP_ERR_NOERROR) {
        return error_string[errstat];
    } else {
        return "Unknown Error";
    }
}