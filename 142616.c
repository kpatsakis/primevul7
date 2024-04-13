usm_create_user_from_session(netsnmp_session * session)
{
    struct usmUser *user;
    int             user_just_created = 0;
    char *cp;

    /*
     * - don't create-another/copy-into user for this session by default
     * - bail now (no error) if we don't have an engineID
     */
    if (SNMP_FLAGS_USER_CREATED == (session->flags & SNMP_FLAGS_USER_CREATED) ||
        session->securityModel != SNMP_SEC_MODEL_USM ||
        session->version != SNMP_VERSION_3 ||
        session->securityNameLen == 0 ||
        session->securityEngineIDLen == 0)
        return SNMPERR_SUCCESS;

    DEBUGMSGTL(("usm", "no flag defined...  continuing\n"));
    session->flags |= SNMP_FLAGS_USER_CREATED;

    /*
     * now that we have the engineID, create an entry in the USM list
     * for this user using the information in the session 
     */
    user = usm_get_user_from_list(session->securityEngineID,
                                  session->securityEngineIDLen,
                                  session->securityName,
                                  usm_get_userList(), 0);
    if (NULL != user) 
        DEBUGMSGTL(("usm", "user exists x=%p\n", user));
    else
    if (user == NULL) {
        DEBUGMSGTL(("usm", "Building user %s...\n",
                    session->securityName));
        /*
         * user doesn't exist so we create and add it 
         */
        user = (struct usmUser *) calloc(1, sizeof(struct usmUser));
        if (user == NULL)
            return SNMPERR_GENERR;

        /*
         * copy in the securityName 
         */
        if (session->securityName) {
            user->name = strdup(session->securityName);
            user->secName = strdup(session->securityName);
            if (user->name == NULL || user->secName == NULL) {
                usm_free_user(user);
                return SNMPERR_GENERR;
            }
        }

        /*
         * copy in the engineID 
         */
        user->engineID = netsnmp_memdup(session->securityEngineID,
                                        session->securityEngineIDLen);
        if (session->securityEngineID && !user->engineID) {
            usm_free_user(user);
            return SNMPERR_GENERR;
        }
        user->engineIDLen = session->securityEngineIDLen;

        user_just_created = 1;
    }

    /*
     * copy the auth protocol 
     */
    if (user->authProtocol == NULL && session->securityAuthProto != NULL) {
        SNMP_FREE(user->authProtocol);
        user->authProtocol =
            snmp_duplicate_objid(session->securityAuthProto,
                                 session->securityAuthProtoLen);
        if (user->authProtocol == NULL) {
            usm_free_user(user);
            return SNMPERR_GENERR;
        }
        user->authProtocolLen = session->securityAuthProtoLen;
    }

    /*
     * copy the priv protocol 
     */
    if (user->privProtocol == NULL && session->securityPrivProto != NULL) {
        SNMP_FREE(user->privProtocol);
        user->privProtocol =
            snmp_duplicate_objid(session->securityPrivProto,
                                 session->securityPrivProtoLen);
        if (user->privProtocol == NULL) {
            usm_free_user(user);
            return SNMPERR_GENERR;
        }
        user->privProtocolLen = session->securityPrivProtoLen;
    }

    /*
     * copy in the authentication Key.  If not localized, localize it 
     */
    if (user->authKey == NULL) {
        if (session->securityAuthLocalKey != NULL
            && session->securityAuthLocalKeyLen != 0) {
            /* already localized key passed in.  use it */
            SNMP_FREE(user->authKey);
            user->authKey = netsnmp_memdup(session->securityAuthLocalKey,
                                           session->securityAuthLocalKeyLen);
            if (!user->authKey) {
                usm_free_user(user);
                return SNMPERR_GENERR;
            }
            user->authKeyLen = session->securityAuthLocalKeyLen;
        } else if (session->securityAuthKeyLen != 0) {
            SNMP_FREE(user->authKey);
            user->authKey = (u_char *) calloc(1, USM_LENGTH_KU_HASHBLOCK);
            user->authKeyLen = USM_LENGTH_KU_HASHBLOCK;
            if ((user->authKey == NULL) ||
                generate_kul(user->authProtocol, user->authProtocolLen,
                             user->engineID, user->engineIDLen,
                             session->securityAuthKey,
                             session->securityAuthKeyLen, user->authKey,
                             &user->authKeyLen) != SNMPERR_SUCCESS) {
                usm_free_user(user);
                return SNMPERR_GENERR;
            }
        } else if ((cp = netsnmp_ds_get_string(NETSNMP_DS_LIBRARY_ID, 
                                               NETSNMP_DS_LIB_AUTHLOCALIZEDKEY))) {
            size_t buflen = USM_AUTH_KU_LEN;
            SNMP_FREE(user->authKey);
            user->authKey = (u_char *)malloc(buflen); /* max length needed */
            user->authKeyLen = 0;
            /* it will be a hex string */
            if ((NULL == user->authKey) ||
                !snmp_hex_to_binary(&user->authKey, &buflen, &user->authKeyLen,
                                    0, cp)) {
                usm_free_user(user);
                return SNMPERR_GENERR;
            }
        }
    }

    /*
     * copy in the privacy Key.  If not localized, localize it 
     */
    if (user->privKey == NULL) {
        /** save buffer size in case we need to extend key */
        int keyBufSize = USM_PRIV_KU_LEN;

        DEBUGMSGTL(("usm", "copying privKey\n"));
        if (session->securityPrivLocalKey != NULL
            && session->securityPrivLocalKeyLen != 0) {
            /* already localized key passed in.  use it */
            SNMP_FREE(user->privKey);
            user->privKey = netsnmp_memdup(session->securityPrivLocalKey,
                                           session->securityPrivLocalKeyLen);
            if (!user->privKey) {
                usm_free_user(user);
                return SNMPERR_GENERR;
            }
            keyBufSize = user->privKeyLen = session->securityPrivLocalKeyLen;
        } else if (session->securityPrivKeyLen != 0) {
            SNMP_FREE(user->privKey);
            user->privKey = (u_char *) calloc(1, keyBufSize);
            user->privKeyLen = keyBufSize;
            if ((user->privKey == NULL) ||
                generate_kul(user->authProtocol, user->authProtocolLen,
                             user->engineID, user->engineIDLen,
                             session->securityPrivKey,
                             session->securityPrivKeyLen, user->privKey,
                             &user->privKeyLen) != SNMPERR_SUCCESS) {
                usm_free_user(user);
                return SNMPERR_GENERR;
            }
        } else if ((cp = netsnmp_ds_get_string(NETSNMP_DS_LIBRARY_ID, 
                                               NETSNMP_DS_LIB_PRIVLOCALIZEDKEY))) {
            size_t buflen = keyBufSize;
            user->privKey = (u_char *)malloc(buflen); /* max length needed */
            user->privKeyLen = 0;
            /* it will be a hex string */
            if ((NULL == user->privKey) ||
                !snmp_hex_to_binary(&user->privKey, &buflen, &user->privKeyLen,
                                    0, cp)) {
                usm_free_user(user);
                return SNMPERR_GENERR;
            }
        }
        if (usm_extend_user_kul(user, keyBufSize) != SNMPERR_SUCCESS) {
            usm_free_user(user);
            return SNMPERR_GENERR;
        }
    }

    if (user_just_created) {
        /*
         * add the user into the database 
         */
        user->userStatus = RS_ACTIVE;
        user->userStorageType = ST_READONLY;
        usm_add_user(user);
    }
    DEBUGMSGTL(("9:usm", "user created\n"));

    return SNMPERR_SUCCESS;


}