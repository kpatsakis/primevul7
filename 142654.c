usm_create_user(void)
{
    struct usmUser *newUser;

    /*
     * create the new user 
     */
    newUser = (struct usmUser *) calloc(1, sizeof(struct usmUser));
    if (newUser == NULL)
        return NULL;

    /*
     * fill the auth/priv protocols 
     */
    if ((newUser->authProtocol =
         snmp_duplicate_objid(usmNoAuthProtocol,
                              sizeof(usmNoAuthProtocol) / sizeof(oid))) ==
        NULL)
        return usm_free_user(newUser);
    newUser->authProtocolLen = sizeof(usmNoAuthProtocol) / sizeof(oid);

    if ((newUser->privProtocol =
         snmp_duplicate_objid(usmNoPrivProtocol,
                              sizeof(usmNoPrivProtocol) / sizeof(oid))) ==
        NULL)
        return usm_free_user(newUser);
    newUser->privProtocolLen = sizeof(usmNoPrivProtocol) / sizeof(oid);

    /*
     * set the storage type to nonvolatile, and the status to ACTIVE 
     */
    newUser->userStorageType = ST_NONVOLATILE;
    newUser->userStatus = RS_ACTIVE;
    return newUser;

}                               /* end usm_clone_user() */