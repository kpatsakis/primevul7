usm_extend_user_kul(struct usmUser *user, u_int privKeyBufSize)
{
    netsnmp_priv_alg_info *pai;

    DEBUGMSGTL(("usm", "extending key\n"));

    if (NULL == user) {
        DEBUGMSGTL(("usm", "null user!\n"));
        return SNMPERR_GENERR;
    }

    pai = sc_get_priv_alg_byoid(user->privProtocol, user->privProtocolLen);
    if (NULL == pai) {
        DEBUGMSGTL(("usm", "privProtocol lookup failed!\n"));
        return SNMPERR_GENERR;
    }

    return netsnmp_extend_kul(pai->proper_length, user->authProtocol,
                              user->authProtocolLen, pai->type, user->engineID,
                              user->engineIDLen, &user->privKey,
                              &user->privKeyLen, privKeyBufSize);
}