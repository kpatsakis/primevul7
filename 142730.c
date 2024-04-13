usm_calc_offsets(size_t globalDataLen,  /* SNMPv3Message + HeaderData */
                 int secLevel, size_t secEngineIDLen, size_t secNameLen, size_t scopedPduLen,   /* An BER encoded sequence. */
                 u_long engineboots,    /* XXX (asn1.c works in long, not int.) */
                 long engine_time,      /* XXX (asn1.c works in long, not int.) */
                 size_t * theTotalLength,       /* globalDataLen + msgSecurityP. + msgData */
                 size_t * authParamsOffset,     /* Distance to auth bytes.                 */
                 size_t * privParamsOffset,     /* Distance to priv bytes.                 */
                 size_t * dataOffset,   /* Distance to scopedPdu SEQ  -or-  the
                                         *   crypted (data) portion of msgData.    */
                 size_t * datalen,      /* Size of msgData OCTET STRING encoding.  */
                 size_t * msgAuthParmLen,       /* Size of msgAuthenticationParameters.    */
                 size_t * msgPrivParmLen,       /* Size of msgPrivacyParameters.           */
                 size_t * otstlen,      /* Size of msgSecurityP. O.S. encoding.    */
                 size_t * seq_len,      /* Size of msgSecurityP. SEQ data.         */
                 size_t * msgSecParmLen)
{                               /* Size of msgSecurityP. SEQ.              */
    int             engIDlen,   /* Sizes of OCTET STRING and SEQ encodings */
                    engBtlen,   /*   for fields within                     */
                    engTmlen,   /*   msgSecurityParameters portion of      */
                    namelen,    /*   SNMPv3Message.                        */
                    authlen, privlen, ret;

    /*
     * If doing authentication, msgAuthParmLen = 12 else msgAuthParmLen = 0.
     * If doing encryption,     msgPrivParmLen = 8  else msgPrivParmLen = 0.
     */
    *msgAuthParmLen = (secLevel == SNMP_SEC_LEVEL_AUTHNOPRIV
                       || secLevel == SNMP_SEC_LEVEL_AUTHPRIV) ? 12 : 0;

    *msgPrivParmLen = (secLevel == SNMP_SEC_LEVEL_AUTHPRIV) ? 8 : 0;


    /*
     * Calculate lengths.
     */
    if ((engIDlen = asn_predict_length(ASN_OCTET_STR,
                                       NULL, secEngineIDLen)) == -1) {
        return -1;
    }

    if ((engBtlen = asn_predict_length(ASN_INTEGER,
                                       (u_char *) & engineboots,
                                       sizeof(long))) == -1) {
        return -1;
    }

    if ((engTmlen = asn_predict_length(ASN_INTEGER,
                                       (u_char *) & engine_time,
                                       sizeof(long))) == -1) {
        return -1;
    }

    if ((namelen = asn_predict_length(ASN_OCTET_STR,
                                      NULL, secNameLen)) == -1) {
        return -1;
    }

    if ((authlen = asn_predict_length(ASN_OCTET_STR,
                                      NULL, *msgAuthParmLen)) == -1) {
        return -1;
    }

    if ((privlen = asn_predict_length(ASN_OCTET_STR,
                                      NULL, *msgPrivParmLen)) == -1) {
        return -1;
    }

    *seq_len =
        engIDlen + engBtlen + engTmlen + namelen + authlen + privlen;

    if ((ret = asn_predict_length(ASN_SEQUENCE,
                                      NULL, *seq_len)) == -1) {
        return -1;
    }
    *otstlen = (size_t)ret;

    if ((ret = asn_predict_length(ASN_OCTET_STR,
                                      NULL, *otstlen)) == -1) {
        return -1;
    }
    *msgSecParmLen = (size_t)ret;

    *authParamsOffset = globalDataLen + +(*msgSecParmLen - *seq_len)
        + engIDlen + engBtlen + engTmlen + namelen
        + (authlen - *msgAuthParmLen);

    *privParamsOffset = *authParamsOffset + *msgAuthParmLen
        + (privlen - *msgPrivParmLen);


    /*
     * Compute the size of the plaintext.  Round up to account for cipher
     * block size, if necessary.
     *
     * XXX  This is hardwired for 1DES... If scopedPduLen is already
     *      a multiple of 8, then *add* 8 more; otherwise, round up
     *      to the next multiple of 8.
     *
     * FIX  Calculation of encrypted portion of msgData and consequent
     *      setting and sanity checking of theTotalLength, et al. should
     *      occur *after* encryption has taken place.
     */
    if (secLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        scopedPduLen = ROUNDUP8(scopedPduLen);

        if ((ret = asn_predict_length(ASN_OCTET_STR, NULL, scopedPduLen)) == -1) {
            return -1;
        }
        *datalen = (size_t)ret;
    } else {
        *datalen = scopedPduLen;
    }

    *dataOffset = globalDataLen + *msgSecParmLen +
        (*datalen - scopedPduLen);
    *theTotalLength = globalDataLen + *msgSecParmLen + *datalen;

    return 0;

}                               /* end usm_calc_offsets() */