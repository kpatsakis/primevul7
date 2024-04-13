usm_read_user(const char *line)
{
    struct usmUser *user;
    size_t          len, proper_length, privtype;

    user = usm_create_user();
    if (user == NULL)
        return NULL;

    user->userStatus = atoi(line);
    line = skip_token_const(line);
    user->userStorageType = atoi(line);
    line = skip_token_const(line);
    line = read_config_read_octet_string_const(line, &user->engineID,
                                               &user->engineIDLen);

    /*
     * set the lcd entry for this engineID to the minimum boots/time
     * values so that its a known engineid and won't return a report pdu.
     * This is mostly important when receiving v3 traps so that the usm
     * will at least continue processing them. 
     */
    set_enginetime(user->engineID, user->engineIDLen, 1, 0, 0);

    line = read_config_read_octet_string(line, (u_char **) & user->name,
                                         &len);
    line = read_config_read_octet_string(line, (u_char **) & user->secName,
                                         &len);
    SNMP_FREE(user->cloneFrom);
    user->cloneFromLen = 0;

    line = read_config_read_objid_const(line, &user->cloneFrom,
                                        &user->cloneFromLen);

    SNMP_FREE(user->authProtocol);
    user->authProtocolLen = 0;

    line = read_config_read_objid_const(line, &user->authProtocol,
                                        &user->authProtocolLen);
    line = read_config_read_octet_string_const(line, &user->authKey,
                                               &user->authKeyLen);
    SNMP_FREE(user->privProtocol);
    user->privProtocolLen = 0;

    line = read_config_read_objid_const(line, &user->privProtocol,
                                        &user->privProtocolLen);
    line = read_config_read_octet_string(line, &user->privKey,
                                         &user->privKeyLen);

    privtype = sc_get_privtype(user->privProtocol, user->privProtocolLen);
    proper_length = sc_get_proper_priv_length_bytype(privtype);
    if (USM_CREATE_USER_PRIV_DES == privtype)
        proper_length *= 2; /* ?? we store salt with key */
    /* For backwards compatibility */
    if (user->privKeyLen > proper_length) {
        user->privKeyLen = proper_length;
    }

    line = read_config_read_octet_string(line, &user->userPublicString,
                                         &user->userPublicStringLen);
    return user;
}