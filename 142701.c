usm_process_in_msg(int msgProcModel,    /* (UNUSED) */
                   size_t maxMsgSize,   /* IN     - Used to calc maxSizeResponse.  */
                   u_char * secParams,  /* IN     - BER encoded securityParameters. */
                   int secModel,        /* (UNUSED) */
                   int secLevel,        /* IN     - AuthNoPriv, authPriv etc.      */
                   u_char * wholeMsg,   /* IN     - Original v3 message.           */
                   size_t wholeMsgLen,  /* IN     - Msg length.                    */
                   u_char * secEngineID,        /* OUT    - Pointer snmpEngineID.          */
                   size_t * secEngineIDLen,     /* IN/OUT - Len available, len returned.   */
                   /*
                    * NOTE: Memory provided by caller.      
                    */
                   char *secName,       /* OUT    - Pointer to securityName.       */
                   size_t * secNameLen, /* IN/OUT - Len available, len returned.   */
                   u_char ** scopedPdu, /* OUT    - Pointer to plaintext scopedPdu. */
                   size_t * scopedPduLen,       /* IN/OUT - Len available, len returned.   */
                   size_t * maxSizeResponse,    /* OUT    - Max size of Response PDU.      */
                   void **secStateRf,   /* OUT    - Ref to security state.         */
                   netsnmp_session * sess,      /* IN     - session which got the message  */
                   u_char msg_flags)
{                               /* IN     - v3 Message flags.              */
    size_t          remaining = wholeMsgLen - (u_int)
        ((u_long) * secParams - (u_long) * wholeMsg);
    u_int           boots_uint;
    u_int           time_uint;
#ifdef HAVE_AES
    u_int           net_boots, net_time;
#endif
#ifndef NETSNMP_DISABLE_DES
    int             i;
#endif
    u_char          signature[USM_MAX_AUTHSIZE];
    size_t          signature_length = USM_MAX_AUTHSIZE;
    u_char          salt[BYTESIZE(USM_MAX_SALT_LENGTH)];
    size_t          salt_length = BYTESIZE(USM_MAX_SALT_LENGTH);
    u_char          iv[BYTESIZE(USM_MAX_SALT_LENGTH)];
    u_int           iv_length = BYTESIZE(USM_MAX_SALT_LENGTH);
    u_char         *data_ptr;
    u_char         *value_ptr;
    u_char          type_value;
    u_char         *end_of_overhead = NULL;
    int             error;
    int             rc = 0;
    struct usmStateReference **secStateRef =
        (struct usmStateReference **) secStateRf;

    struct usmUser *user;


    DEBUGMSGTL(("usm", "USM processing begun...\n"));

    netsnmp_assert(secStateRef);

    usm_free_usmStateReference(*secStateRef);
    *secStateRef = usm_malloc_usmStateReference();
    if (*secStateRef == NULL) {
        DEBUGMSGTL(("usm", "Out of memory.\n"));
        return SNMPERR_USM_GENERICERROR;
    }

    /*
     * Make sure the *secParms is an OCTET STRING.
     * Extract the user name, engine ID, and security level.
     */
    if ((rc = usm_parse_security_parameters(secParams, remaining,
                                            secEngineID, secEngineIDLen,
                                            &boots_uint, &time_uint,
                                            secName, secNameLen,
                                            signature, &signature_length,
                                            salt, &salt_length,
                                            &data_ptr)) < 0) {
        DEBUGMSGTL(("usm", "Parsing failed (rc %d).\n", rc));
        if (rc == -2) {
            /*
             * This indicates a decryptionError.  
             */
            snmp_increment_statistic(STAT_USMSTATSDECRYPTIONERRORS);
            error = SNMPERR_USM_DECRYPTIONERROR;
        } else {
            snmp_increment_statistic(STAT_SNMPINASNPARSEERRS);
            error = SNMPERR_USM_PARSEERROR;
        }
        goto err;
    }

    /*
     * RFC 2574 section 8.3.2
     * 1)  If the privParameters field is not an 8-octet OCTET STRING,
     * then an error indication (decryptionError) is returned to the
     * calling module.
     */
    if ((secLevel == SNMP_SEC_LEVEL_AUTHPRIV) && (salt_length != 8)) {
        snmp_increment_statistic(STAT_USMSTATSDECRYPTIONERRORS);
        error = SNMPERR_USM_DECRYPTIONERROR;
        goto err;
    }

    if (secLevel != SNMP_SEC_LEVEL_AUTHPRIV) {
        /*
         * pull these out now so reports can use them 
         */
        *scopedPdu = data_ptr;
        *scopedPduLen = wholeMsgLen - (data_ptr - wholeMsg);
        end_of_overhead = data_ptr;
    }

    /*
     * Cache the name, engine ID, and security level,
     * * per step 2 (section 3.2)
     */
    if (usm_set_usmStateReference_name
        (*secStateRef, secName, *secNameLen) == -1) {
        DEBUGMSGTL(("usm", "%s\n", "Couldn't cache name."));
        error = SNMPERR_USM_GENERICERROR;
        goto err;
    }

    if (usm_set_usmStateReference_engine_id
        (*secStateRef, secEngineID, *secEngineIDLen) == -1) {
        DEBUGMSGTL(("usm", "%s\n", "Couldn't cache engine id."));
        error = SNMPERR_USM_GENERICERROR;
        goto err;
    }

    if (usm_set_usmStateReference_sec_level(*secStateRef, secLevel) ==
        -1) {
        DEBUGMSGTL(("usm", "%s\n", "Couldn't cache security level."));
        error = SNMPERR_USM_GENERICERROR;
        goto err;
    }

    /*
     * Locate the engine ID record.
     * If it is unknown, then either create one or note this as an error.
     */
    if ((sess && (sess->isAuthoritative == SNMP_SESS_AUTHORITATIVE ||
                  (sess->isAuthoritative == SNMP_SESS_UNKNOWNAUTH &&
                   (msg_flags & SNMP_MSG_FLAG_RPRT_BIT)))) ||
        (!sess && (msg_flags & SNMP_MSG_FLAG_RPRT_BIT))) {
        if (ISENGINEKNOWN(secEngineID, *secEngineIDLen) == FALSE) {
            DEBUGMSGTL(("usm", "Unknown Engine ID.\n"));
            snmp_increment_statistic(STAT_USMSTATSUNKNOWNENGINEIDS);
            error = SNMPERR_USM_UNKNOWNENGINEID;
            goto err;
        }
    } else {
        if (ENSURE_ENGINE_RECORD(secEngineID, *secEngineIDLen)
            != SNMPERR_SUCCESS) {
            DEBUGMSGTL(("usm", "%s\n", "Couldn't ensure engine record."));
            error = SNMPERR_USM_GENERICERROR;
            goto err;
        }

    }


    /*
     * Locate the User record.
     * If the user/engine ID is unknown, report this as an error.
     */
    if ((user = usm_get_user_from_list(secEngineID, *secEngineIDLen,
                                       secName, userList,
                                       (((sess && sess->isAuthoritative ==
                                          SNMP_SESS_AUTHORITATIVE) ||
                                         (!sess)) ? 0 : 1)))
        == NULL) {
        DEBUGMSGTL(("usm", "Unknown User(%s)\n", secName));
        snmp_increment_statistic(STAT_USMSTATSUNKNOWNUSERNAMES);
        error = SNMPERR_USM_UNKNOWNSECURITYNAME;
        goto err;
    }

    /* ensure the user is active */
    if (user->userStatus != RS_ACTIVE) {
        DEBUGMSGTL(("usm", "Attempt to use an inactive user.\n"));
        error = SNMPERR_USM_UNKNOWNSECURITYNAME;
        goto err;
    }

    /*
     * Make sure the security level is appropriate.
     */

    rc = usm_check_secLevel(secLevel, user);
    if (1 == rc) {
        DEBUGMSGTL(("usm", "Unsupported Security Level (%d).\n",
                    secLevel));
        snmp_increment_statistic(STAT_USMSTATSUNSUPPORTEDSECLEVELS);
        error = SNMPERR_USM_UNSUPPORTEDSECURITYLEVEL;
        goto err;
    } else if (rc != 0) {
        DEBUGMSGTL(("usm", "Unknown issue.\n"));
        error = SNMPERR_USM_GENERICERROR;
        goto err;
    }

    /*
     * Check the authentication credentials of the message.
     */
    if (secLevel == SNMP_SEC_LEVEL_AUTHNOPRIV
        || secLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        if (sc_check_keyed_hash(user->authProtocol, user->authProtocolLen,
                                user->authKey, user->authKeyLen,
                                wholeMsg, wholeMsgLen,
                                signature, signature_length)
            != SNMP_ERR_NOERROR) {
            DEBUGMSGTL(("usm", "Verification failed.\n"));
            snmp_increment_statistic(STAT_USMSTATSWRONGDIGESTS);
	    snmp_log(LOG_WARNING, "Authentication failed for %s\n",
				user->name);
            error = SNMPERR_USM_AUTHENTICATIONFAILURE;
            goto err;
        }

        DEBUGMSGTL(("usm", "Verification succeeded.\n"));
    }


    /*
     * Steps 10-11  user is already set - relocated before timeliness 
     * check in case it fails - still save user data for response.
     *
     * Cache the keys and protocol oids, per step 11 (s3.2).
     */
    if (usm_set_usmStateReference_auth_protocol(*secStateRef,
                                                user->authProtocol,
                                                user->
                                                authProtocolLen) == -1) {
        DEBUGMSGTL(("usm", "%s\n",
                    "Couldn't cache authentication protocol."));
        error = SNMPERR_USM_GENERICERROR;
        goto err;
    }

    if (usm_set_usmStateReference_auth_key(*secStateRef,
                                           user->authKey,
                                           user->authKeyLen) == -1) {
        DEBUGMSGTL(("usm", "%s\n",
                    "Couldn't cache authentication key."));
        error = SNMPERR_USM_GENERICERROR;
        goto err;
    }

    if (usm_set_usmStateReference_priv_protocol(*secStateRef,
                                                user->privProtocol,
                                                user->
                                                privProtocolLen) == -1) {
        DEBUGMSGTL(("usm", "%s\n",
                    "Couldn't cache privacy protocol."));
        error = SNMPERR_USM_GENERICERROR;
        goto err;
    }

    if (usm_set_usmStateReference_priv_key(*secStateRef,
                                           user->privKey,
                                           user->privKeyLen) == -1) {
        DEBUGMSGTL(("usm", "%s\n", "Couldn't cache privacy key."));
        error = SNMPERR_USM_GENERICERROR;
        goto err;
    }


    /*
     * Perform the timeliness/time manager functions.
     */
    if (secLevel == SNMP_SEC_LEVEL_AUTHNOPRIV
        || secLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        if (usm_check_and_update_timeliness(secEngineID, *secEngineIDLen,
                                            boots_uint, time_uint,
                                            &error) == -1) {
            goto err;
        }
    }
#ifdef							LCD_TIME_SYNC_OPT
    /*
     * Cache the unauthenticated time to use in case we don't have
     * anything better - this guess will be no worse than (0,0)
     * that we normally use.
     */
    else {
        set_enginetime(secEngineID, *secEngineIDLen,
                       boots_uint, time_uint, FALSE);
    }
#endif                          /* LCD_TIME_SYNC_OPT */


    /*
     * If needed, decrypt the scoped PDU.
     */
    if (secLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
        int priv_type = sc_get_privtype(user->privProtocol,
                                        user->privProtocolLen);
        remaining = wholeMsgLen - (data_ptr - wholeMsg);

        if ((value_ptr = asn_parse_sequence(data_ptr, &remaining,
                                            &type_value,
                                            (ASN_UNIVERSAL | ASN_PRIMITIVE
                                             | ASN_OCTET_STR),
                                            "encrypted sPDU")) == NULL) {
            DEBUGMSGTL(("usm", "%s\n",
                        "Failed while parsing encrypted sPDU."));
            snmp_increment_statistic(STAT_SNMPINASNPARSEERRS);
            usm_free_usmStateReference(*secStateRef);
            *secStateRef = NULL;
            error = SNMPERR_USM_PARSEERROR;
            goto err;
        }

#ifndef NETSNMP_DISABLE_DES
        if (USM_CREATE_USER_PRIV_DES == (priv_type & USM_PRIV_MASK_ALG)) {
            /*
             * From RFC2574:
             * 
             * "Before decryption, the encrypted data length is verified.
             * If the length of the OCTET STRING to be decrypted is not
             * an integral multiple of 8 octets, the decryption process
             * is halted and an appropriate exception noted."  
             */

            if (remaining % 8 != 0) {
                DEBUGMSGTL(("usm",
                            "Ciphertext is %lu bytes, not an integer multiple of 8 (rem %lu)\n",
                            (unsigned long)remaining, (unsigned long)remaining % 8));
                snmp_increment_statistic(STAT_USMSTATSDECRYPTIONERRORS);
                usm_free_usmStateReference(*secStateRef);
                *secStateRef = NULL;
                error = SNMPERR_USM_DECRYPTIONERROR;
                goto err;
            }

            end_of_overhead = value_ptr;

            if ( !user->privKey ) {
                DEBUGMSGTL(("usm", "No privacy pass phrase for %s\n", user->secName));
                snmp_increment_statistic(STAT_USMSTATSDECRYPTIONERRORS);
                usm_free_usmStateReference(*secStateRef);
                *secStateRef = NULL;
                error = SNMPERR_USM_DECRYPTIONERROR;
                goto err;
            }

            /*
             * XOR the salt with the last (iv_length) bytes
             * of the priv_key to obtain the IV.
             */
            iv_length = BYTESIZE(USM_DES_SALT_LENGTH);
            for (i = 0; i < (int) iv_length; i++)
                iv[i] = salt[i] ^ user->privKey[iv_length + i];
        }
#endif
#ifdef HAVE_AES
        if (USM_CREATE_USER_PRIV_AES == (priv_type & USM_PRIV_MASK_ALG)) {
            iv_length = BYTESIZE(USM_AES_SALT_LENGTH);
            net_boots = ntohl(boots_uint);
            net_time = ntohl(time_uint);
            memcpy(iv, &net_boots, 4);
            memcpy(iv+4, &net_time, 4);
            memcpy(iv+8, salt, salt_length);
        }
#endif

#ifdef NETSNMP_ENABLE_TESTING_CODE
        if (debug_is_token_registered("usm/dump") == SNMPERR_SUCCESS) {
            dump_chunk("usm/dump", "Cypher Text", value_ptr, remaining);
            dump_chunk("usm/dump", "salt + Encrypted form:",
                       salt, salt_length);
            dump_chunk("usm/dump", "IV + Encrypted form:", iv, iv_length);
        }
#endif
        if (sc_decrypt(user->privProtocol, user->privProtocolLen,
                       user->privKey, user->privKeyLen,
                       iv, iv_length,
                       value_ptr, remaining, *scopedPdu, scopedPduLen)
            != SNMP_ERR_NOERROR) {
            DEBUGMSGTL(("usm", "%s\n", "Failed decryption."));
            snmp_increment_statistic(STAT_USMSTATSDECRYPTIONERRORS);
            error = SNMPERR_USM_DECRYPTIONERROR;
            goto err;
        }
#ifdef NETSNMP_ENABLE_TESTING_CODE
        if (debug_is_token_registered("usm/dump") == SNMPERR_SUCCESS) {
            dump_chunk("usm/dump", "Decrypted chunk:",
                       *scopedPdu, *scopedPduLen);
        }
#endif
    }
    /*
     * sPDU is plaintext.
     */
    else {
        *scopedPdu = data_ptr;
        *scopedPduLen = wholeMsgLen - (data_ptr - wholeMsg);
        end_of_overhead = data_ptr;

    }                           /* endif -- PDU decryption */


    /*
     * Calculate the biggest sPDU for the response (i.e., whole - ovrhd).
     *
     * FIX  Correct? 
     */
    *maxSizeResponse = maxMsgSize - (end_of_overhead - wholeMsg);


    DEBUGMSGTL(("usm", "USM processing completed.\n"));

    return SNMPERR_SUCCESS;

err:
    usm_free_usmStateReference(*secStateRef);
    *secStateRef = NULL;
    netsnmp_assert(error != SNMPERR_SUCCESS);
    return error;
}                               /* end usm_process_in_msg() */