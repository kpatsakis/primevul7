usm_secmod_process_in_msg(struct snmp_secmod_incoming_params *parms)
{
    if (!parms)
        return SNMPERR_GENERR;

    return usm_process_in_msg(parms->msgProcModel,
                              parms->maxMsgSize,
                              parms->secParams,
                              parms->secModel,
                              parms->secLevel,
                              parms->wholeMsg,
                              parms->wholeMsgLen,
                              parms->secEngineID,
                              parms->secEngineIDLen,
                              parms->secName,
                              parms->secNameLen,
                              parms->scopedPdu,
                              parms->scopedPduLen,
                              parms->maxSizeResponse,
                              parms->secStateRef,
                              parms->sess, parms->msg_flags);
}