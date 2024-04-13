usm_create_usmUser_from_string(char *line, const char **errorMsg)
{
    char           *cp;
    const char     *dummy;
    char            buf[SNMP_MAXBUF_MEDIUM];
    struct usmUser *newuser;
    u_char          userKey[SNMP_MAXBUF_SMALL], *tmpp;
    size_t          userKeyLen = SNMP_MAXBUF_SMALL;
    size_t          privKeySize;
    size_t          ret;
    int             ret2, properLen, properPrivKeyLen;
    const oid      *def_auth_prot, *def_priv_prot;
    size_t          def_auth_prot_len, def_priv_prot_len;
    netsnmp_priv_alg_info *pai;

    def_auth_prot = get_default_authtype(&def_auth_prot_len);
    def_priv_prot = get_default_privtype(&def_priv_prot_len);

    if (NULL == line)
        return NULL;

#ifdef NETSNMP_ENABLE_TESTING_CODE
    DEBUGMSGTL(("usmUser", "new user %s\n", line)); /* logs passphrases */
#endif

    if (NULL == errorMsg)
        errorMsg = &dummy;
    *errorMsg = NULL; /* no errors yet */

    newuser = usm_create_user();
    if (newuser == NULL) {
        *errorMsg = "malloc failure creating new user";
        goto fail;
    }

    /*
     * READ: Security Name 
     */
    cp = copy_nword(line, buf, sizeof(buf));

    /*
     * check for (undocumented) 'keep master key' flag. so far, this is
     * just used for users for informs (who need non-localized keys).
     */
    if (strcmp(buf, "-M") == 0) {
        newuser->flags |= USMUSER_FLAG_KEEP_MASTER_KEY;
        cp = copy_nword(cp, buf, sizeof(buf));
    }

    /*
     * might be a -e ENGINEID argument 
     */
    if (strcmp(buf, "-e") == 0) {
        size_t          ebuf_len = 32, eout_len = 0;
        u_char         *ebuf = (u_char *) malloc(ebuf_len);

        if (ebuf == NULL) {
            *errorMsg = "malloc failure processing -e flag";
            goto fail;
        }

        /*
         * Get the specified engineid from the line.  
         */
        cp = copy_nword(cp, buf, sizeof(buf));
        if (!snmp_hex_to_binary(&ebuf, &ebuf_len, &eout_len, 1, buf)) {
            *errorMsg = "invalid EngineID argument to -e";
            SNMP_FREE(ebuf);
            goto fail;
        }

        newuser->engineID = ebuf;
        newuser->engineIDLen = eout_len;
        cp = copy_nword(cp, buf, sizeof(buf));
    } else {
        newuser->engineID = snmpv3_generate_engineID(&ret);
        if (ret == 0) {
            goto fail;
        }
        newuser->engineIDLen = ret;
    }

    newuser->secName = strdup(buf);
    newuser->name = strdup(buf);

    if (!cp) {
#ifdef NETSNMP_FORCE_SYSTEM_V3_AUTHPRIV
        /** no passwords ok iff defaults are noauth/nopriv */
        if (snmp_oid_compare(usmNoAuthProtocol, OID_LENGTH(usmNoAuthProtocol),
                             def_auth_prot, def_auth_prot_len) != 0) {
            *errorMsg = "no authentication pass phrase";
            goto fail;
        }
        if (snmp_oid_compare(usmNoPrivProtocol, OID_LENGTH(usmNoPrivProtocol),
                             def_priv_prot, def_priv_prot_len) != 0) {
            *errorMsg = "no privacy pass phrase";
            goto fail;
        }
#endif /* NETSNMP_FORCE_SYSTEM_V3_AUTHPRIV */
        goto add;               /* no authentication or privacy type */
    }

    /*
     * READ: Authentication Type 
     */
    newuser->authProtocol[0] = 0;
    cp = copy_nword(cp, buf, sizeof(buf));
    if ((strncmp(cp, "default", 7) == 0) && (NULL != def_auth_prot)) {
        SNMP_FREE(newuser->authProtocol);
        newuser->authProtocol = snmp_duplicate_objid(def_auth_prot,
                                                     def_auth_prot_len);
        if (newuser->authProtocol == NULL) {
            *errorMsg = "malloc failed";
            goto fail;
        }
        newuser->authProtocolLen = def_auth_prot_len;
    } else {
        const oid *auth_prot;
        int auth_type = usm_lookup_auth_type(buf);
        if (auth_type < 0) {
            *errorMsg = "unknown authProtocol";
            goto fail;
        }
        auth_prot = sc_get_auth_oid(auth_type, &newuser->authProtocolLen);
        if (auth_prot) {
            SNMP_FREE(newuser->authProtocol);
            newuser->authProtocol =
                snmp_duplicate_objid(auth_prot, newuser->authProtocolLen);
        }
        if (newuser->authProtocol == NULL) {
            *errorMsg = "malloc failed";
            goto fail;
        }
    }
    if (0 == newuser->authProtocol[0]) {
        *errorMsg = "Unknown authentication protocol";
        goto fail;
    }
#ifdef NETSNMP_FORCE_SYSTEM_V3_AUTHPRIV
    if (snmp_oid_compare(newuser->authProtocol, newuser->authProtocolLen,
                         def_auth_prot, def_auth_prot_len) != 0) {
        *errorMsg = "auth protocol does not match system policy";
        goto fail;
    }
#endif /* NETSNMP_FORCE_SYSTEM_V3_AUTHPRIV */

    /*
     * READ: Authentication Pass Phrase or key
     */
    cp = copy_nword(cp, buf, sizeof(buf));
    if (strcmp(buf,"-m") == 0) {
        /* a master key is specified */
        cp = copy_nword(cp, buf, sizeof(buf));
        ret = sizeof(userKey);
        tmpp = userKey;
        userKeyLen = 0;
        if (!snmp_hex_to_binary(&tmpp, &ret, &userKeyLen, 0, buf)) {
            *errorMsg = "invalid key value argument to -m";
            goto fail;
        }
        /* save master key */
        if (newuser->flags & USMUSER_FLAG_KEEP_MASTER_KEY) {
            newuser->authKeyKu = netsnmp_memdup(userKey, userKeyLen);
            newuser->authKeyKuLen = userKeyLen;
        }
    } else if (strcmp(buf,"-l") != 0) {
        /* a password is specified */
        userKeyLen = sizeof(userKey);
        ret2 = generate_Ku(newuser->authProtocol, newuser->authProtocolLen,
                          (u_char *) buf, strlen(buf), userKey, &userKeyLen);
        if (ret2 != SNMPERR_SUCCESS) {
            *errorMsg = "could not generate the authentication key from the supplied pass phrase.";
            goto fail;
        }
        /* save master key */
        if (newuser->flags & USMUSER_FLAG_KEEP_MASTER_KEY) {
            newuser->authKeyKu = netsnmp_memdup(userKey, userKeyLen);
            newuser->authKeyKuLen = userKeyLen;
        }
    }        
        
    /*
     * And turn it into a localized key 
     */
    properLen = sc_get_proper_auth_length_bytype(
        sc_get_authtype(newuser->authProtocol, newuser->authProtocolLen));
    if (properLen <= 0) {
        *errorMsg = "Could not get proper authentication protocol key length";
        goto fail;
    }
    newuser->authKey = (u_char *) malloc(properLen);
    newuser->authKeyLen = properLen;

    if (strcmp(buf,"-l") == 0) {
        /* a local key is directly specified */
        cp = copy_nword(cp, buf, sizeof(buf));
        ret = newuser->authKeyLen;
        newuser->authKeyLen = 0;
        if (!snmp_hex_to_binary(&newuser->authKey, &ret,
                                &newuser->authKeyLen, 0, buf)) {
            *errorMsg = "invalid key value argument to -l";
            goto fail;
        }
        if (properLen != newuser->authKeyLen) {
            *errorMsg = "improper key length to -l";
            goto fail;
        }
    } else {
        ret2 = generate_kul(newuser->authProtocol, newuser->authProtocolLen,
                           newuser->engineID, newuser->engineIDLen,
                           userKey, userKeyLen,
                           newuser->authKey, &newuser->authKeyLen);
        if (ret2 != SNMPERR_SUCCESS) {
            *errorMsg = "could not generate localized authentication key (Kul) from the master key (Ku).";
            goto fail;
        }
    }

    if (!cp) {
#ifndef NETSNMP_FORCE_SYSTEM_V3_AUTHPRIV
        goto add;               /* no privacy type (which is legal) */
#else
        if (snmp_oid_compare(usmNoPrivProtocol, OID_LENGTH(usmNoPrivProtocol),
                             def_priv_prot, def_priv_prot_len) == 0)
            goto add;
        else {
            *errorMsg = "priv protocol does not match system policy";
            goto fail;
        }
#endif /* NETSNMP_FORCE_SYSTEM_V3_AUTHPRIV */
    }

    /*
     * READ: Privacy Type 
     */
    newuser->privProtocol[0] = 0;
    cp = copy_nword(cp, buf, sizeof(buf));
    if ((strncmp(buf, "default", 7) == 0) && (NULL != def_priv_prot)) {
        SNMP_FREE(newuser->privProtocol);
        newuser->privProtocol =
            snmp_duplicate_objid(def_priv_prot, def_priv_prot_len);
        if (newuser->privProtocol == NULL) {
            *errorMsg = "malloc failed";
            goto fail;
        }
        newuser->privProtocolLen = def_priv_prot_len;
        pai = sc_get_priv_alg_byoid(newuser->privProtocol,
                                    newuser->privProtocolLen);
    } else {
        int priv_type = usm_lookup_priv_type(buf);
        if (priv_type < 0) {
            *errorMsg = "unknown privProtocol";
            DEBUGMSGTL(("usmUser", "%s %s\n", *errorMsg, buf));
            goto fail;
        }
        DEBUGMSGTL(("9:usmUser", "privProtocol %s\n", buf));
        pai = sc_get_priv_alg_bytype(priv_type);
        if (pai) {
            SNMP_FREE(newuser->privProtocol);
            newuser->privProtocolLen = pai->oid_len;
            newuser->privProtocol =
                snmp_duplicate_objid(pai->alg_oid, newuser->privProtocolLen);
            DEBUGMSGTL(("9:usmUser", "pai %s\n", pai->name));
            if (newuser->privProtocol == NULL) {
                *errorMsg = "malloc failed";
                goto fail;
            }
        }
    }
    if (NULL == pai) {
        *errorMsg = "priv protocol lookup failed";
        goto fail;
    }

    if (0 == newuser->privProtocol[0] && NULL == *errorMsg)
        *errorMsg = "Unknown privacy protocol";
    if (NULL != *errorMsg)
        goto fail;
#ifdef NETSNMP_FORCE_SYSTEM_V3_AUTHPRIV
    if (snmp_oid_compare(newuser->privProtocol, newuser->privProtocolLen,
                         def_priv_prot, def_priv_prot_len) != 0) {
        *errorMsg = "priv protocol does not match system policy";
        goto fail;
    }
#endif /* NETSNMP_FORCE_SYSTEM_V3_AUTHPRIV */

    properPrivKeyLen = pai->proper_length;
    if (USM_CREATE_USER_PRIV_DES == pai->type)
        properPrivKeyLen *= 2; /* ?? we store salt with key */

    /*
     * READ: Encryption Pass Phrase or key
     */
    if (!cp) {
        /*
         * assume the same as the authentication key 
         */
        newuser->privKey = netsnmp_memdup(newuser->authKey,
                                          newuser->authKeyLen);
        privKeySize = newuser->privKeyLen = newuser->authKeyLen;
        if (newuser->flags & USMUSER_FLAG_KEEP_MASTER_KEY) {
            newuser->privKeyKu = netsnmp_memdup(newuser->authKeyKu,
                                                newuser->authKeyKuLen);
            newuser->privKeyKuLen = newuser->authKeyKuLen;
        }
    } else {
        cp = copy_nword(cp, buf, sizeof(buf));
        
        if (strcmp(buf,"-m") == 0) {
            /* a master key is specified */
            cp = copy_nword(cp, buf, sizeof(buf));
            ret = sizeof(userKey);
            tmpp = userKey;
            userKeyLen = 0;
            if (!snmp_hex_to_binary(&tmpp, &ret, &userKeyLen, 0, buf)) {
                *errorMsg = "invalid key value argument to -m";
                goto fail;
            }
            /* save master key */
            if (newuser->flags & USMUSER_FLAG_KEEP_MASTER_KEY) {
                newuser->privKeyKu = netsnmp_memdup(userKey, userKeyLen);
                newuser->privKeyKuLen = userKeyLen;
            }
        } else if (strcmp(buf,"-l") != 0) {
            /* a password is specified */
            userKeyLen = sizeof(userKey);
            ret2 = generate_Ku(newuser->authProtocol, newuser->authProtocolLen,
                              (u_char*)buf, strlen(buf), userKey, &userKeyLen);
            if (ret2 != SNMPERR_SUCCESS) {
                *errorMsg = "could not generate the privacy key from the supplied pass phrase.";
                goto fail;
            }
            /* save master key */
            if (newuser->flags & USMUSER_FLAG_KEEP_MASTER_KEY) {
                newuser->privKeyKu = netsnmp_memdup(userKey, userKeyLen);
                newuser->privKeyKuLen = userKeyLen;
            }
        }

        /*
         * And turn it into a localized key
         * Allocate enough space for greater of auth mac and privKey len.
         */
        privKeySize = SNMP_MAX(properPrivKeyLen, properLen);
        newuser->privKey = (u_char *) malloc(privKeySize);
        newuser->privKeyLen = privKeySize;

        if (strcmp(buf,"-l") == 0) {
            /* a local key is directly specified */
            cp = copy_nword(cp, buf, sizeof(buf));
            ret = newuser->privKeyLen;
            newuser->privKeyLen = 0;
            if (!snmp_hex_to_binary(&newuser->privKey, &ret,
                                    &newuser->privKeyLen, 0, buf)) {
                *errorMsg = "invalid key value argument to -l";
                goto fail;
            }
        } else {
            ret2 = generate_kul(newuser->authProtocol, newuser->authProtocolLen,
                               newuser->engineID, newuser->engineIDLen,
                               userKey, userKeyLen,
                               newuser->privKey, &newuser->privKeyLen);
            if (ret2 != SNMPERR_SUCCESS) {
                *errorMsg = "could not generate localized privacy key (Kul) from the master key (Ku).";
                goto fail;
            }
        }

        if (newuser->privKeyLen < properPrivKeyLen) {
            ret = usm_extend_user_kul(newuser, properPrivKeyLen);
            if (ret != SNMPERR_SUCCESS) {
                *errorMsg = "could not extend localized privacy key to required length.";
                goto fail;
            }
        }
    }

    if ((newuser->privKeyLen >= properPrivKeyLen) || (properPrivKeyLen == 0)){
        DEBUGMSGTL(("9:usmUser", "truncating privKeyLen from %" NETSNMP_PRIz "d to %d\n",
                    newuser->privKeyLen, properPrivKeyLen));
        newuser->privKeyLen = properPrivKeyLen;
    }
    else {
        DEBUGMSGTL(("usmUser",
                    "privKey length %" NETSNMP_PRIz "d < %d required by privProtocol\n",
                    newuser->privKeyLen, properPrivKeyLen));
      *errorMsg = "privKey length is less than required by privProtocol";
      goto fail;
    }

  add:
    usm_add_user(newuser);
    DEBUGMSGTL(("usmUser", "created a new user %s at ", newuser->secName));
    DEBUGMSGHEX(("usmUser", newuser->engineID, newuser->engineIDLen));
    DEBUGMSG(("usmUser", "\n"));

    return newuser;

  fail:
    usm_free_user(newuser);
    return NULL;
}