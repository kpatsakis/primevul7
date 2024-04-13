usm_free_user(struct usmUser *user)
{
    if (user == NULL)
        return NULL;

    SNMP_FREE(user->engineID);
    SNMP_FREE(user->name);
    SNMP_FREE(user->secName);
    SNMP_FREE(user->cloneFrom);
    SNMP_FREE(user->userPublicString);
    SNMP_FREE(user->authProtocol);
    SNMP_FREE(user->privProtocol);

    if (user->authKey != NULL) {
        SNMP_ZERO(user->authKey, user->authKeyLen);
        SNMP_FREE(user->authKey);
    }

    if (user->privKey != NULL) {
        SNMP_ZERO(user->privKey, user->privKeyLen);
        SNMP_FREE(user->privKey);
    }

    if (user->authKeyKu != NULL) {
        SNMP_ZERO(user->authKeyKu, user->authKeyKuLen);
        SNMP_FREE(user->authKeyKu);
    }

    if (user->privKeyKu != NULL) {
        SNMP_ZERO(user->privKeyKu, user->privKeyKuLen);
        SNMP_FREE(user->privKeyKu);
    }


    /*
     * FIX  Why not put this check *first?*
     */
    if (user->prev != NULL) {   /* ack, this shouldn't happen */
        user->prev->next = user->next;
    }
    if (user->next != NULL) {
        user->next->prev = user->prev;
        if (user->prev != NULL) /* ack this is really bad, because it means
                                 * * we'll loose the head of some structure tree */
            DEBUGMSGTL(("usm",
                        "Severe: Asked to free the head of a usmUser tree somewhere."));
    }


    SNMP_ZERO(user, sizeof(*user));
    SNMP_FREE(user);

    return NULL;                /* for convenience to returns from calling functions */

}                               /* end usm_free_user() */