usm_secmod_rgenerate_out_msg(struct snmp_secmod_outgoing_params *parms)
{
    if (!parms)
        return SNMPERR_GENERR;

    return usm_rgenerate_out_msg(parms->msgProcModel,
                                 parms->globalData, parms->globalDataLen,
                                 parms->maxMsgSize, parms->secModel,
                                 parms->secEngineID, parms->secEngineIDLen,
                                 parms->secName, parms->secNameLen,
                                 parms->secLevel,
                                 parms->scopedPdu, parms->scopedPduLen,
                                 parms->secStateRef,
                                 parms->wholeMsg, parms->wholeMsgLen,
                                 parms->wholeMsgOffset);
}