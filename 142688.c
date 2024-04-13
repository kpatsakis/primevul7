usm_set_user_password(struct usmUser *user, const char *token, char *line)
{
    char           *cp = line;
    u_char         *engineID = user->engineID;
    size_t          engineIDLen = user->engineIDLen;

    u_char        **key;
    size_t         *keyLen;
    u_char          userKey[SNMP_MAXBUF_SMALL];
    size_t          userKeyLen = SNMP_MAXBUF_SMALL;
    u_char         *userKeyP = userKey;
    int             type, ret;

    /*
     * Retrieve the "old" key and set the key type.
     */
    if (!token) {
        return;
    } else if (strcmp(token, "userSetAuthPass") == 0) {
        key = &user->authKey;
        keyLen = &user->authKeyLen;
        type = 0;
    } else if (strcmp(token, "userSetPrivPass") == 0) {
        key = &user->privKey;
        keyLen = &user->privKeyLen;
        type = 0;
    } else if (strcmp(token, "userSetAuthKey") == 0) {
        key = &user->authKey;
        keyLen = &user->authKeyLen;
        type = 1;
    } else if (strcmp(token, "userSetPrivKey") == 0) {
        key = &user->privKey;
        keyLen = &user->privKeyLen;
        type = 1;
    } else if (strcmp(token, "userSetAuthLocalKey") == 0) {
        key = &user->authKey;
        keyLen = &user->authKeyLen;
        type = 2;
    } else if (strcmp(token, "userSetPrivLocalKey") == 0) {
        key = &user->privKey;
        keyLen = &user->privKeyLen;
        type = 2;
    } else {
        /*
         * no old key, or token was not recognized 
         */
        return;
    }

    if (*key) {
        /*
         * (destroy and) free the old key 
         */
        memset(*key, 0, *keyLen);
        SNMP_FREE(*key);
    }

    if (type == 0) {
        /*
         * convert the password into a key 
         */
        if (cp == NULL) {
            config_perror("missing user password");
            return;
        }
        ret = generate_Ku(user->authProtocol, user->authProtocolLen,
                          (u_char *) cp, strlen(cp), userKey, &userKeyLen);

        if (ret != SNMPERR_SUCCESS) {
            config_perror("setting key failed (in sc_genKu())");
            return;
        }
        /* save master key */
        if (user->flags & USMUSER_FLAG_KEEP_MASTER_KEY) {
            if (userKey == user->privKey) {
                user->privKeyKu = netsnmp_memdup(userKey, userKeyLen);
                user->privKeyKuLen = userKeyLen;
            } else if (userKey == user->authKey) {
                user->authKeyKu = netsnmp_memdup(userKey, userKeyLen);
                user->authKeyKuLen = userKeyLen;
            }
        }
    } else if (type == 1) {
        cp = read_config_read_octet_string(cp, &userKeyP, &userKeyLen);

        if (cp == NULL) {
            config_perror("invalid user key");
            return;
        }
    }

    if (type < 2) {
        *key = (u_char *) malloc(SNMP_MAXBUF_SMALL);
        *keyLen = SNMP_MAXBUF_SMALL;
        ret = generate_kul(user->authProtocol, user->authProtocolLen,
                           engineID, engineIDLen,
                           userKey, userKeyLen, *key, keyLen);
        if (ret != SNMPERR_SUCCESS) {
            config_perror("setting key failed (in generate_kul())");
            return;
        }

        /*
         * (destroy and) free the old key 
         */
        memset(userKey, 0, sizeof(userKey));

    } else {
        /*
         * the key is given, copy it in 
         */
        cp = read_config_read_octet_string(cp, key, keyLen);

        if (cp == NULL) {
            config_perror("invalid localized user key");
            return;
        }
    }

    if (key == &user->privKey) {
        ret = usm_extend_user_kul(user, *keyLen);
        if (SNMPERR_SUCCESS != ret) {
            config_perror("error extending localized user key");
            return;
        }
    }
}                               /* end usm_set_password() */