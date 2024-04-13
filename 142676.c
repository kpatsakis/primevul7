usm_rgenerate_out_msg(int msgProcModel, /* (UNUSED) */
                      u_char * globalData,      /* IN */
                      /*
                       * points at the msgGlobalData, which is of length given by next 
                       * parameter.  
                       */
                      size_t globalDataLen,     /* IN - Length of msg header data.      */
                      int maxMsgSize,   /* (UNUSED) */
                      int secModel,     /* (UNUSED) */
                      u_char * secEngineID,     /* IN - Pointer snmpEngineID.           */
                      size_t secEngineIDLen,    /* IN - SnmpEngineID length.            */
                      char *secName,    /* IN - Pointer to securityName.        */
                      size_t secNameLen,        /* IN - SecurityName length.            */
                      int secLevel,     /* IN - AuthNoPriv, authPriv etc.       */
                      u_char * scopedPdu,       /* IN */
                      /*
                       * Pointer to scopedPdu will be encrypted by USM if needed
                       * * and written to packet buffer immediately following
                       * * securityParameters, entire msg will be authenticated by
                       * * USM if needed.
                       */
                      size_t scopedPduLen,      /* IN - scopedPdu length. */
                      void *secStateRef,        /* IN */
                      /*
                       * secStateRef, pointer to cached info provided only for
                       * * Response, otherwise NULL.
                       */
                      u_char ** wholeMsg,       /*  IN/OUT  */
                      /*
                       * Points at the pointer to the packet buffer, which might get extended
                       * if necessary via realloc().  
                       */
                      size_t * wholeMsgLen,     /*  IN/OUT  */
                      /*
                       * Length of the entire packet buffer, **not** the length of the
                       * packet.  
                       */
                      size_t * offset   /*  IN/OUT  */
                      /*
                       * Offset from the end of the packet buffer to the start of the packet,
                       * also known as the packet length.  
                       */
    )
{
    size_t          msgAuthParmLen = 0;
    u_int           boots_uint;
    u_int           time_uint;
    long            boots_long;
    long            time_long;

    /*
     * Indirection because secStateRef values override parameters.
     * 
     * None of these are to be free'd - they are either pointing to
     * what's in the secStateRef or to something either in the
     * actual parameter list or the user list.
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
                                         * value (other then err). */
    size_t          salt_length = 0, save_salt_length = 0;
    u_char          salt[BYTESIZE(USM_MAX_SALT_LENGTH)];
    u_char          authParams[USM_MAX_AUTHSIZE];
    u_char          iv[BYTESIZE(USM_MAX_SALT_LENGTH)];
    size_t          sp_offset = 0, mac_offset = 0;
    int             rc = 0;

    DEBUGMSGTL(("usm", "USM processing has begun (offset %d)\n", (int)*offset));

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
     * * Identify the user record.
     */
    else {
        struct usmUser *user;

        /*
         * we do allow an unknown user name for
         * unauthenticated requests. 
         */
        if ((user = usm_get_user(secEngineID, secEngineIDLen, secName))
            == NULL && secLevel != SNMP_SEC_LEVEL_NOAUTH) {
            DEBUGMSGTL(("usm", "Unknown User\n"));
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
        DEBUGMSGTL(("usm", "Unsupported Security Level or type (%d)\n",
                    theSecLevel));

        return SNMPERR_USM_UNSUPPORTEDSECURITYLEVEL;
    }


    /*
     * * Retrieve the engine information.
     * *
     * * XXX    No error is declared in the EoP when sending messages to
     * *        unknown engines, processing continues w/ boots/time == (0,0).
     */
    if (get_enginetime(theEngineID, theEngineIDLength,
                       &boots_uint, &time_uint, FALSE) == -1) {
        DEBUGMSGTL(("usm", "%s\n", "Failed to find engine data."));
    }

    boots_long = boots_uint;
    time_long = time_uint;

    if (theSecLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        /*
         * Initially assume that the ciphertext will end up the same size as
         * the plaintext plus some padding.  Really sc_encrypt ought to be able
         * to grow this for us, a la asn_realloc_rbuild_<type> functions, but
         * this will do for now.  
         */
        u_char         *ciphertext = NULL;
        size_t          ciphertextlen = scopedPduLen + 64;
        int             priv_type = sc_get_privtype(thePrivProtocol,
                                                    thePrivProtocolLength);

        if ((ciphertext = (u_char *) malloc(ciphertextlen)) == NULL) {
            DEBUGMSGTL(("usm",
                        "couldn't malloc %d bytes for encrypted PDU\n",
                        (int)ciphertextlen));
            return SNMPERR_MALLOC;
        }

        /*
         * XXX Hardwired to seek into a 1DES private key!  
         */
#ifdef HAVE_AES
        if (USM_CREATE_USER_PRIV_AES == (priv_type & USM_PRIV_MASK_ALG)) {
            salt_length = BYTESIZE(USM_AES_SALT_LENGTH);
            save_salt_length = BYTESIZE(USM_AES_SALT_LENGTH)/2;
            if (!thePrivKey ||
                usm_set_aes_iv(salt, &salt_length,
                               htonl(boots_uint), htonl(time_uint),
                               iv) == -1) {
                DEBUGMSGTL(("usm", "Can't set AES iv.\n"));
                SNMP_FREE(ciphertext);
                return SNMPERR_USM_GENERICERROR;
            }
        } 
#endif
#ifndef NETSNMP_DISABLE_DES
        if (USM_CREATE_USER_PRIV_DES == (priv_type & USM_PRIV_MASK_ALG)) {
            salt_length = BYTESIZE(USM_DES_SALT_LENGTH);
            save_salt_length = BYTESIZE(USM_DES_SALT_LENGTH);
            if (!thePrivKey || (usm_set_salt(salt, &salt_length,
                                             thePrivKey + 8,
                                             thePrivKeyLength - 8,
                                             iv) == -1)) {
                DEBUGMSGTL(("usm", "Can't set DES-CBC salt.\n"));
                SNMP_FREE(ciphertext);
                return SNMPERR_USM_GENERICERROR;
            }
        }
#endif
#ifdef NETSNMP_ENABLE_TESTING_CODE
        if (debug_is_token_registered("usm/dump") == SNMPERR_SUCCESS) {
            dump_chunk("usm/dump", "This data was encrypted:",
                       scopedPdu, scopedPduLen);
        }
#endif

        if (sc_encrypt(thePrivProtocol, thePrivProtocolLength,
                       thePrivKey, thePrivKeyLength,
                       salt, salt_length,
                       scopedPdu, scopedPduLen,
                       ciphertext, &ciphertextlen) != SNMP_ERR_NOERROR) {
            DEBUGMSGTL(("usm", "encryption error.\n"));
            SNMP_FREE(ciphertext);
            return SNMPERR_USM_ENCRYPTIONERROR;
        }

        /*
         * Write the encrypted scopedPdu back into the packet buffer.  
         */

        *offset = 0;
        rc = asn_realloc_rbuild_string(wholeMsg, wholeMsgLen, offset, 1,
                                       (u_char) (ASN_UNIVERSAL |
                                                 ASN_PRIMITIVE |
                                                 ASN_OCTET_STR),
                                       ciphertext, ciphertextlen);
        if (rc == 0) {
            DEBUGMSGTL(("usm", "Encryption failed.\n"));
            SNMP_FREE(ciphertext);
            return SNMPERR_USM_ENCRYPTIONERROR;
        }

#ifdef NETSNMP_ENABLE_TESTING_CODE
        if (debug_is_token_registered("usm/dump") == SNMPERR_SUCCESS) {
            dump_chunk("usm/dump", "salt + Encrypted form: ", salt,
                       salt_length);
            dump_chunk("usm/dump", "wholeMsg:",
                       (*wholeMsg + *wholeMsgLen - *offset), *offset);
        }
#endif

        DEBUGMSGTL(("usm", "Encryption successful.\n"));
        SNMP_FREE(ciphertext);
    } else {
        /*
         * theSecLevel != SNMP_SEC_LEVEL_AUTHPRIV  
         */
    }

    /*
     * Start encoding the msgSecurityParameters.  
     */

    sp_offset = *offset;

    DEBUGDUMPHEADER("send", "msgPrivacyParameters");
    /*
     * msgPrivacyParameters (warning: assumes DES salt).  
     */
    rc = asn_realloc_rbuild_string(wholeMsg, wholeMsgLen, offset, 1,
                                   (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE
                                             | ASN_OCTET_STR),
                                   iv,
                                   save_salt_length);
    DEBUGINDENTLESS();
    if (rc == 0) {
        DEBUGMSGTL(("usm", "building privParams failed.\n"));
        return SNMPERR_TOO_LONG;
    }

    DEBUGDUMPHEADER("send", "msgAuthenticationParameters");
    /*
     * msgAuthenticationParameters.
     */
    if (theSecLevel == SNMP_SEC_LEVEL_AUTHNOPRIV
        || theSecLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        memset(authParams, 0, sizeof(authParams));
        msgAuthParmLen =
            sc_get_auth_maclen(sc_get_authtype(theAuthProtocol,
                                               theAuthProtocolLength));
    }

    rc = asn_realloc_rbuild_string(wholeMsg, wholeMsgLen, offset, 1,
                                   (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE
                                             | ASN_OCTET_STR), authParams,
                                   msgAuthParmLen);
    DEBUGINDENTLESS();
    if (rc == 0) {
        DEBUGMSGTL(("usm", "building authParams failed.\n"));
        return SNMPERR_TOO_LONG;
    }

    /*
     * Remember where to put the actual HMAC we calculate later on.  An
     * encoded OCTET STRING of length USM_MD5_AND_SHA_AUTH_LEN has an ASN.1
     * header of length 2, hence the fudge factor.  This works as long as
     * auth lengths stay < 127.
     */
    mac_offset = *offset - 2;

    /*
     * msgUserName.  
     */
    DEBUGDUMPHEADER("send", "msgUserName");
    rc = asn_realloc_rbuild_string(wholeMsg, wholeMsgLen, offset, 1,
                                   (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE
                                             | ASN_OCTET_STR),
                                   (u_char *) theName, theNameLength);
    DEBUGINDENTLESS();
    if (rc == 0) {
        DEBUGMSGTL(("usm", "building authParams failed.\n"));
        return SNMPERR_TOO_LONG;
    }

    /*
     * msgAuthoritativeEngineTime.  
     */
    DEBUGDUMPHEADER("send", "msgAuthoritativeEngineTime");
    rc = asn_realloc_rbuild_int(wholeMsg, wholeMsgLen, offset, 1,
                                (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE |
                                          ASN_INTEGER), &time_long,
                                sizeof(long));
    DEBUGINDENTLESS();
    if (rc == 0) {
        DEBUGMSGTL(("usm",
                    "building msgAuthoritativeEngineTime failed.\n"));
        return SNMPERR_TOO_LONG;
    }

    /*
     * msgAuthoritativeEngineBoots.  
     */
    DEBUGDUMPHEADER("send", "msgAuthoritativeEngineBoots");
    rc = asn_realloc_rbuild_int(wholeMsg, wholeMsgLen, offset, 1,
                                (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE |
                                          ASN_INTEGER), &boots_long,
                                sizeof(long));
    DEBUGINDENTLESS();
    if (rc == 0) {
        DEBUGMSGTL(("usm",
                    "building msgAuthoritativeEngineBoots failed.\n"));
        return SNMPERR_TOO_LONG;
    }

    DEBUGDUMPHEADER("send", "msgAuthoritativeEngineID");
    rc = asn_realloc_rbuild_string(wholeMsg, wholeMsgLen, offset, 1,
                                   (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE
                                             | ASN_OCTET_STR), theEngineID,
                                   theEngineIDLength);
    DEBUGINDENTLESS();
    if (rc == 0) {
        DEBUGMSGTL(("usm", "building msgAuthoritativeEngineID failed.\n"));
        return SNMPERR_TOO_LONG;
    }

    /*
     * USM msgSecurityParameters sequence header  
     */
    rc = asn_realloc_rbuild_sequence(wholeMsg, wholeMsgLen, offset, 1,
                                     (u_char) (ASN_SEQUENCE |
                                               ASN_CONSTRUCTOR),
                                     *offset - sp_offset);
    if (rc == 0) {
        DEBUGMSGTL(("usm", "building usm security parameters failed.\n"));
        return SNMPERR_TOO_LONG;
    }

    /*
     * msgSecurityParameters OCTET STRING wrapper.  
     */
    rc = asn_realloc_rbuild_header(wholeMsg, wholeMsgLen, offset, 1,
                                   (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE
                                             | ASN_OCTET_STR),
                                   *offset - sp_offset);

    if (rc == 0) {
        DEBUGMSGTL(("usm", "building msgSecurityParameters failed.\n"));
        return SNMPERR_TOO_LONG;
    }

    /*
     * Copy in the msgGlobalData and msgVersion.  
     */
    while ((*wholeMsgLen - *offset) < globalDataLen) {
        if (!asn_realloc(wholeMsg, wholeMsgLen)) {
            DEBUGMSGTL(("usm", "building global data failed.\n"));
            return SNMPERR_TOO_LONG;
        }
    }

    *offset += globalDataLen;
    memcpy(*wholeMsg + *wholeMsgLen - *offset, globalData, globalDataLen);

    /*
     * Total packet sequence.  
     */
    rc = asn_realloc_rbuild_sequence(wholeMsg, wholeMsgLen, offset, 1,
                                     (u_char) (ASN_SEQUENCE |
                                               ASN_CONSTRUCTOR), *offset);
    if (rc == 0) {
        DEBUGMSGTL(("usm", "building master packet sequence failed.\n"));
        return SNMPERR_TOO_LONG;
    }

    /*
     * Now consider / do authentication.  
     */

    if (theSecLevel == SNMP_SEC_LEVEL_AUTHNOPRIV ||
        theSecLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        size_t          temp_sig_len = msgAuthParmLen;
        u_char         *temp_sig = (u_char *) malloc(temp_sig_len);
        u_char         *proto_msg = *wholeMsg + *wholeMsgLen - *offset;
        size_t          proto_msg_len = *offset;


        if (temp_sig == NULL) {
            DEBUGMSGTL(("usm", "Out of memory.\n"));
            return SNMPERR_USM_GENERICERROR;
        }

        if (sc_generate_keyed_hash(theAuthProtocol, theAuthProtocolLength,
                                   theAuthKey, theAuthKeyLength,
                                   proto_msg, proto_msg_len,
                                   temp_sig, &temp_sig_len)
            != SNMP_ERR_NOERROR) {
            SNMP_FREE(temp_sig);
            DEBUGMSGTL(("usm", "Signing failed.\n"));
            return SNMPERR_USM_AUTHENTICATIONFAILURE;
        }

        if (temp_sig_len != msgAuthParmLen) {
            SNMP_FREE(temp_sig);
            DEBUGMSGTL(("usm", "Signing lengths failed.\n"));
            return SNMPERR_USM_AUTHENTICATIONFAILURE;
        }

        memcpy(*wholeMsg + *wholeMsgLen - mac_offset, temp_sig,
               msgAuthParmLen);
        SNMP_FREE(temp_sig);
    }
    /*
     * endif -- create keyed hash 
     */
    DEBUGMSGTL(("usm", "USM processing completed.\n"));
    return SNMPERR_SUCCESS;
}                               /* end usm_rgenerate_out_msg() */