usm_create_initial_user(const char *name,
                        const oid * authProtocol, size_t authProtocolLen,
                        const oid * privProtocol, size_t privProtocolLen)
{
    struct usmUser *newUser = usm_create_user();
    if (newUser == NULL)
        return NULL;

    if ((newUser->name = strdup(name)) == NULL)
        return usm_free_user(newUser);

    if ((newUser->secName = strdup(name)) == NULL)
        return usm_free_user(newUser);

    if ((newUser->engineID =
         snmpv3_generate_engineID(&newUser->engineIDLen)) == NULL)
        return usm_free_user(newUser);

    if ((newUser->cloneFrom = (oid *) malloc(sizeof(oid) * 2)) == NULL)
        return usm_free_user(newUser);
    newUser->cloneFrom[0] = 0;
    newUser->cloneFrom[1] = 0;
    newUser->cloneFromLen = 2;

    SNMP_FREE(newUser->privProtocol);
    if ((newUser->privProtocol = snmp_duplicate_objid(privProtocol,
                                                      privProtocolLen)) ==
        NULL) {
        return usm_free_user(newUser);
    }
    newUser->privProtocolLen = privProtocolLen;

    SNMP_FREE(newUser->authProtocol);
    if ((newUser->authProtocol = snmp_duplicate_objid(authProtocol,
                                                      authProtocolLen)) ==
        NULL) {
        return usm_free_user(newUser);
    }
    newUser->authProtocolLen = authProtocolLen;

    newUser->userStatus = RS_ACTIVE;
    newUser->userStorageType = ST_READONLY;

    return newUser;
}