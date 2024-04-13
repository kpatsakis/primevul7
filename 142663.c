usm_cloneFrom_user(struct usmUser *from, struct usmUser *to)
{
    to->flags = from->flags;

    /*
     * copy the authProtocol oid row pointer 
     */
    SNMP_FREE(to->authProtocol);

    if ((to->authProtocol =
         snmp_duplicate_objid(from->authProtocol,
                              from->authProtocolLen)) != NULL)
        to->authProtocolLen = from->authProtocolLen;
    else
        to->authProtocolLen = 0;


    /*
     * copy the authKey 
     */
    SNMP_FREE(to->authKey);

    if (from->authKeyLen > 0 &&
        (to->authKey = (u_char *) malloc(from->authKeyLen))
        != NULL) {
        to->authKeyLen = from->authKeyLen;
        memcpy(to->authKey, from->authKey, to->authKeyLen);
    } else {
        to->authKey = NULL;
        to->authKeyLen = 0;
    }

    /*
     * copy the authKeyKu
     */
    SNMP_FREE(to->authKeyKu);

    if (from->authKeyKuLen > 0 &&
        (to->authKeyKu = (u_char *) malloc(from->authKeyKuLen)) != NULL) {
        to->authKeyKuLen = from->authKeyKuLen;
        memcpy(to->authKeyKu, from->authKeyKu, to->authKeyKuLen);
    } else {
        to->authKeyKu = NULL;
        to->authKeyKuLen = 0;
    }


    /*
     * copy the privProtocol oid row pointer 
     */
    SNMP_FREE(to->privProtocol);

    if ((to->privProtocol =
         snmp_duplicate_objid(from->privProtocol,
                              from->privProtocolLen)) != NULL)
        to->privProtocolLen = from->privProtocolLen;
    else
        to->privProtocolLen = 0;

    /*
     * copy the privKey 
     */
    SNMP_FREE(to->privKey);

    if (from->privKeyLen > 0 &&
        (to->privKey = (u_char *) malloc(from->privKeyLen))
        != NULL) {
        to->privKeyLen = from->privKeyLen;
        memcpy(to->privKey, from->privKey, to->privKeyLen);
    } else {
        to->privKey = NULL;
        to->privKeyLen = 0;
    }

    /*
     * copy the privKeyKu
     */
    SNMP_FREE(to->privKeyKu);
    if (from->privKeyKuLen > 0 &&
        (to->privKeyKu = (u_char *) malloc(from->privKeyKuLen)) != NULL) {
        to->privKeyKuLen = from->privKeyKuLen;
        memcpy(to->privKeyKu, from->privKeyKu, to->privKeyKuLen);
    } else {
        to->privKeyKu = NULL;
        to->privKeyKuLen = 0;
    }
    return to;
}