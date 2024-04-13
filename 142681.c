usm_generate_out_msg(int msgProcModel,  /* (UNUSED) */
                     u_char * globalData,       /* IN */
                     /*
                      * Pointer to msg header data will point to the beginning
                      * * of the entire packet buffer to be transmitted on wire,
                      * * memory will be contiguous with secParams, typically
                      * * this pointer will be passed back as beginning of
                      * * wholeMsg below.  asn seq. length is updated w/ new length.
                      * *
                      * * While this points to a buffer that should be big enough
                      * * for the whole message, only the first two parts
                      * * of the message are completed, namely SNMPv3Message and
                      * * HeaderData.  globalDataLen (next parameter) represents
                      * * the length of these two completed parts.
                      */
                     size_t globalDataLen,      /* IN - Length of msg header data.      */
                     int maxMsgSize,    /* (UNUSED) */
                     int secModel,      /* (UNUSED) */
                     u_char * secEngineID,      /* IN - Pointer snmpEngineID.           */
                     size_t secEngineIDLen,     /* IN - SnmpEngineID length.            */
                     char *secName,     /* IN - Pointer to securityName.        */
                     size_t secNameLen, /* IN - SecurityName length.            */
                     int secLevel,      /* IN - AuthNoPriv, authPriv etc.       */
                     u_char * scopedPdu,        /* IN */
                     /*
                      * Pointer to scopedPdu will be encrypted by USM if needed
                      * * and written to packet buffer immediately following
                      * * securityParameters, entire msg will be authenticated by
                      * * USM if needed.
                      */
                     size_t scopedPduLen,       /* IN - scopedPdu length. */
                     void *secStateRef, /* IN */
                     /*
                      * secStateRef, pointer to cached info provided only for
                      * * Response, otherwise NULL.
                      */
                     u_char * secParams,        /* OUT */
                     /*
                      * BER encoded securityParameters pointer to offset within
                      * * packet buffer where secParams should be written, the
                      * * entire BER encoded OCTET STRING (including header) is
                      * * written here by USM secParams = globalData +
                      * * globalDataLen.
                      */
                     size_t * secParamsLen,     /* IN/OUT - Len available, len returned. */
                     u_char ** wholeMsg,        /* OUT */
                     /*
                      * Complete authenticated/encrypted message - typically
                      * * the pointer to start of packet buffer provided in
                      * * globalData is returned here, could also be a separate
                      * * buffer.
                      */
                     size_t * wholeMsgLen)
{                               /* IN/OUT - Len available, len returned. */
    size_t          otstlen;
    size_t          seq_len;
    size_t          msgAuthParmLen;
    size_t          msgPrivParmLen;
    size_t          msgSecParmLen;
    size_t          authParamsOffset;
    size_t          privParamsOffset;
    size_t          datalen;
    size_t          dataOffset;
    size_t          theTotalLength;

    u_char         *ptr;
    size_t          ptr_len;
    size_t          remaining;
    size_t          offSet;
    u_int           boots_uint;
    u_int           time_uint;
    long            boots_long;
    long            time_long;

    /*
     * Indirection because secStateRef values override parameters.
     * 
     * None of these are to be free'd - they are either pointing to
     * what's in the secStateRef or to something either in the
     * actual prarmeter list or the user list.
     */

    char           *theName = NULL;
    u_int           theNameLength = 0;
    u_char         *theEngineID = NULL;
    u_int           theEngineIDLength = 0;
    u_char         *theAuthKey = NULL;
    u_int           theAuthKeyLength = 0;
    const oid      *theAuthProtocol = NULL;
    u_int           theAuthProtocolLength = 0;
    u_char         *thePrivKey = NULL;
    u_int           thePrivKeyLength = 0;
    const oid      *thePrivProtocol = NULL;
    u_int           thePrivProtocolLength = 0;
    int             theSecLevel = 0;    /* No defined const for bad
                                         * value (other then err).
                                         */

    DEBUGMSGTL(("usm", "USM processing has begun.\n"));

    if (secStateRef != NULL) {
        /*
         * To hush the compiler for now.  XXX 
         */
        struct usmStateReference *ref
            = (struct usmStateReference *) secStateRef;

        theName = ref->usr_name;
        theNameLength = ref->usr_name_length;
        theEngineID = ref->usr_engine_id;
        theEngineIDLength = ref->usr_engine_id_length;

        if (!theEngineIDLength) {
            theEngineID = secEngineID;
            theEngineIDLength = secEngineIDLen;
        }

        theAuthProtocol = ref->usr_auth_protocol;
        theAuthProtocolLength = ref->usr_auth_protocol_length;
        theAuthKey = ref->usr_auth_key;
        theAuthKeyLength = ref->usr_auth_key_length;
        thePrivProtocol = ref->usr_priv_protocol;
        thePrivProtocolLength = ref->usr_priv_protocol_length;
        thePrivKey = ref->usr_priv_key;
        thePrivKeyLength = ref->usr_priv_key_length;
        theSecLevel = ref->usr_sec_level;
    }

    /*
     * Identify the user record.
     */
    else {
        struct usmUser *user;

        /*
         * we do allow an unknown user name for
         * unauthenticated requests. 
         */
        if ((user = usm_get_user(secEngineID, secEngineIDLen, secName))
            == NULL && secLevel != SNMP_SEC_LEVEL_NOAUTH) {
            DEBUGMSGTL(("usm", "Unknown User(%s)\n", secName));
            return SNMPERR_USM_UNKNOWNSECURITYNAME;
        }

        theName = secName;
        theNameLength = secNameLen;
        theEngineID = secEngineID;
        theSecLevel = secLevel;
        theEngineIDLength = secEngineIDLen;
        if (user) {
            theAuthProtocol = user->authProtocol;
            theAuthProtocolLength = user->authProtocolLen;
            theAuthKey = user->authKey;
            theAuthKeyLength = user->authKeyLen;
            thePrivProtocol = user->privProtocol;
            thePrivProtocolLength = user->privProtocolLen;
            thePrivKey = user->privKey;
            thePrivKeyLength = user->privKeyLen;
        } else {
            /*
             * unknown users can not do authentication (obviously) 
             */
            theAuthProtocol = usmNoAuthProtocol;
            theAuthProtocolLength =
                sizeof(usmNoAuthProtocol) / sizeof(oid);
            theAuthKey = NULL;
            theAuthKeyLength = 0;
            thePrivProtocol = usmNoPrivProtocol;
            thePrivProtocolLength =
                sizeof(usmNoPrivProtocol) / sizeof(oid);
            thePrivKey = NULL;
            thePrivKeyLength = 0;
        }
    }                           /* endif -- secStateRef==NULL */


    /*
     * From here to the end of the function, avoid reference to
     * secName, secEngineID, secLevel, and associated lengths.
     */


    /*
     * Check to see if the user can use the requested sec services.
     */
    if (usm_check_secLevel_vs_protocols(theSecLevel,
                                        theAuthProtocol,
                                        theAuthProtocolLength,
                                        thePrivProtocol,
                                        thePrivProtocolLength) == 1) {
        DEBUGMSGTL(("usm", "Unsupported Security Level (%d)\n",
                    theSecLevel));
        return SNMPERR_USM_UNSUPPORTEDSECURITYLEVEL;
    }


    /*
     * Retrieve the engine information.
     *
     * XXX  No error is declared in the EoP when sending messages to
     *      unknown engines, processing continues w/ boots/time == (0,0).
     */
    if (get_enginetime(theEngineID, theEngineIDLength,
                       &boots_uint, &time_uint, FALSE) == -1) {
        DEBUGMSGTL(("usm", "%s\n", "Failed to find engine data."));
    }

    boots_long = boots_uint;
    time_long = time_uint;


    /*
     * Set up the Offsets.
     */
    if (usm_calc_offsets(globalDataLen, theSecLevel, theEngineIDLength,
                         theNameLength, scopedPduLen, boots_long,
                         time_long, &theTotalLength, &authParamsOffset,
                         &privParamsOffset, &dataOffset, &datalen,
                         &msgAuthParmLen, &msgPrivParmLen, &otstlen,
                         &seq_len, &msgSecParmLen) == -1) {
        DEBUGMSGTL(("usm", "Failed calculating offsets.\n"));
        return SNMPERR_USM_GENERICERROR;
    }

    /*
     * So, we have the offsets for the three parts that need to be
     * determined, and an overall length.  Now we need to make
     * sure all of this would fit in the outgoing buffer, and
     * whether or not we need to make a new buffer, etc.
     */


    /*
     * Set wholeMsg as a pointer to globalData.  Sanity check for
     * the proper size.
     * 
     * Mark workspace in the message with bytes of all 1's to make it
     * easier to find mistakes in raw message dumps.
     */
    ptr = *wholeMsg = globalData;
    if (theTotalLength > *wholeMsgLen) {
        DEBUGMSGTL(("usm", "Message won't fit in buffer.\n"));
        return SNMPERR_USM_GENERICERROR;
    }

    ptr_len = *wholeMsgLen = theTotalLength;

#ifdef NETSNMP_ENABLE_TESTING_CODE
    memset(&ptr[globalDataLen], 0xFF, theTotalLength - globalDataLen);
#endif                          /* NETSNMP_ENABLE_TESTING_CODE */

    /*
     * Do the encryption.
     */
    if (theSecLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        size_t          encrypted_length = theTotalLength - dataOffset;
        size_t          salt_length = BYTESIZE(USM_MAX_SALT_LENGTH);
        u_char          salt[BYTESIZE(USM_MAX_SALT_LENGTH)];
        int             priv_type = sc_get_privtype(thePrivProtocol,
                                                    thePrivProtocolLength);
#ifdef HAVE_AES
        if (USM_CREATE_USER_PRIV_AES == (priv_type & USM_PRIV_MASK_ALG)) {
            if (!thePrivKey ||
                usm_set_aes_iv(salt, &salt_length,
                               htonl(boots_uint), htonl(time_uint),
                               &ptr[privParamsOffset]) == -1) {
                DEBUGMSGTL(("usm", "Can't set AES iv.\n"));
                return SNMPERR_USM_GENERICERROR;
            }
        }
#endif
#ifndef NETSNMP_DISABLE_DES
        /*
         * XXX  Hardwired to seek into a 1DES private key!
         */
        if (USM_CREATE_USER_PRIV_DES == (priv_type & USM_PRIV_MASK_ALG)) {
            if (!thePrivKey ||
                (usm_set_salt(salt, &salt_length,
                              thePrivKey + 8, thePrivKeyLength - 8,
                              &ptr[privParamsOffset])
                 == -1)) {
                DEBUGMSGTL(("usm", "Can't set DES-CBC salt.\n"));
                return SNMPERR_USM_GENERICERROR;
            }
        }
#endif

        if (sc_encrypt(thePrivProtocol, thePrivProtocolLength,
                       thePrivKey, thePrivKeyLength,
                       salt, salt_length,
                       scopedPdu, scopedPduLen,
                       &ptr[dataOffset], &encrypted_length)
            != SNMP_ERR_NOERROR) {
            DEBUGMSGTL(("usm", "encryption error.\n"));
            return SNMPERR_USM_ENCRYPTIONERROR;
        }
#ifdef NETSNMP_ENABLE_TESTING_CODE
        if (debug_is_token_registered("usm/dump") == SNMPERR_SUCCESS) {
            dump_chunk("usm/dump", "This data was encrypted:",
                       scopedPdu, scopedPduLen);
            dump_chunk("usm/dump", "salt + Encrypted form:",
                       salt, salt_length);
            dump_chunk("usm/dump", NULL,
                       &ptr[dataOffset], encrypted_length);
            dump_chunk("usm/dump", "*wholeMsg:",
                       *wholeMsg, theTotalLength);
        }
#endif


        ptr = *wholeMsg;
        ptr_len = *wholeMsgLen = theTotalLength;


        /*
         * XXX  Sanity check for salt length should be moved up
         *      under usm_calc_offsets() or tossed.
         */
        if ((encrypted_length != (theTotalLength - dataOffset))
            || (salt_length != msgPrivParmLen)) {
            DEBUGMSGTL(("usm", "encryption length error.\n"));
            return SNMPERR_USM_ENCRYPTIONERROR;
        }

        DEBUGMSGTL(("usm", "Encryption successful.\n"));
    }

    /*
     * No encryption for you!
     */
    else {
        memcpy(&ptr[dataOffset], scopedPdu, scopedPduLen);
    }



    /*
     * Start filling in the other fields (in prep for authentication).
     * 
     * offSet is an octet string header, which is different from all
     * the other headers.
     */
    remaining = ptr_len - globalDataLen;

    offSet = ptr_len - remaining;
    asn_build_header(&ptr[offSet], &remaining,
                     (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE |
                               ASN_OCTET_STR), otstlen);

    offSet = ptr_len - remaining;
    asn_build_sequence(&ptr[offSet], &remaining,
                       (u_char) (ASN_SEQUENCE | ASN_CONSTRUCTOR), seq_len);

    offSet = ptr_len - remaining;
    DEBUGDUMPHEADER("send", "msgAuthoritativeEngineID");
    asn_build_string(&ptr[offSet], &remaining,
                     (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE |
                               ASN_OCTET_STR), theEngineID,
                     theEngineIDLength);
    DEBUGINDENTLESS();

    offSet = ptr_len - remaining;
    DEBUGDUMPHEADER("send", "msgAuthoritativeEngineBoots");
    asn_build_int(&ptr[offSet], &remaining,
                  (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
                  &boots_long, sizeof(long));
    DEBUGINDENTLESS();

    offSet = ptr_len - remaining;
    DEBUGDUMPHEADER("send", "msgAuthoritativeEngineTime");
    asn_build_int(&ptr[offSet], &remaining,
                  (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
                  &time_long, sizeof(long));
    DEBUGINDENTLESS();

    offSet = ptr_len - remaining;
    DEBUGDUMPHEADER("send", "msgUserName");
    asn_build_string(&ptr[offSet], &remaining,
                     (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE |
                               ASN_OCTET_STR), (u_char *) theName,
                     theNameLength);
    DEBUGINDENTLESS();


    /*
     * Note: if there is no authentication being done,
     * msgAuthParmLen is 0, and there is no effect (other than
     * inserting a zero-length header) of the following
     * statements.
     */

    offSet = ptr_len - remaining;
    asn_build_header(&ptr[offSet],
                     &remaining,
                     (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE |
                               ASN_OCTET_STR), msgAuthParmLen);

    if (theSecLevel == SNMP_SEC_LEVEL_AUTHNOPRIV
        || theSecLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        offSet = ptr_len - remaining;
        memset(&ptr[offSet], 0, msgAuthParmLen);
    }

    remaining -= msgAuthParmLen;


    /*
     * Note: if there is no encryption being done, msgPrivParmLen
     * is 0, and there is no effect (other than inserting a
     * zero-length header) of the following statements.
     */

    offSet = ptr_len - remaining;
    asn_build_header(&ptr[offSet],
                     &remaining,
                     (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE |
                               ASN_OCTET_STR), msgPrivParmLen);

    remaining -= msgPrivParmLen;        /* Skipping the IV already there. */


    /*
     * For privacy, need to add the octet string header for it.
     */
    if (theSecLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        offSet = ptr_len - remaining;
        asn_build_header(&ptr[offSet],
                         &remaining,
                         (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE |
                                   ASN_OCTET_STR),
                         theTotalLength - dataOffset);
    }


    /*
     * Adjust overall length and store it as the first SEQ length
     * of the SNMPv3Message.
     *
     * FIX  4 is a magic number!
     */
    remaining = theTotalLength;
    asn_build_sequence(ptr, &remaining,
                       (u_char) (ASN_SEQUENCE | ASN_CONSTRUCTOR),
                       theTotalLength - 4);


    /*
     * Now, time to consider / do authentication.
     */
    if (theSecLevel == SNMP_SEC_LEVEL_AUTHNOPRIV
        || theSecLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        size_t          temp_sig_len = msgAuthParmLen;
        u_char         *temp_sig = (u_char *) malloc(temp_sig_len);

        if (temp_sig == NULL) {
            DEBUGMSGTL(("usm", "Out of memory.\n"));
            return SNMPERR_USM_GENERICERROR;
        }

        if (sc_generate_keyed_hash(theAuthProtocol, theAuthProtocolLength,
                                   theAuthKey, theAuthKeyLength,
                                   ptr, ptr_len, temp_sig, &temp_sig_len)
            != SNMP_ERR_NOERROR) {
            /*
             * FIX temp_sig_len defined?!
             */
            SNMP_ZERO(temp_sig, temp_sig_len);
            SNMP_FREE(temp_sig);
            DEBUGMSGTL(("usm", "Signing failed.\n"));
            return SNMPERR_USM_AUTHENTICATIONFAILURE;
        }

        if (temp_sig_len != msgAuthParmLen) {
            SNMP_ZERO(temp_sig, temp_sig_len);
            SNMP_FREE(temp_sig);
            DEBUGMSGTL(("usm", "Signing lengths failed.\n"));
            return SNMPERR_USM_AUTHENTICATIONFAILURE;
        }

        memcpy(&ptr[authParamsOffset], temp_sig, msgAuthParmLen);

        SNMP_ZERO(temp_sig, temp_sig_len);
        SNMP_FREE(temp_sig);

    }

    /*
     * endif -- create keyed hash 
     */

    DEBUGMSGTL(("usm", "USM processing completed.\n"));

    return SNMPERR_SUCCESS;

}                               /* end usm_generate_out_msg() */