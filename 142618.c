usm_check_secLevel(int level, struct usmUser *user)
{

    if (user->userStatus != RS_ACTIVE)
        return -1;

    DEBUGMSGTL(("comparex", "Comparing: %" NETSNMP_PRIo "u %" NETSNMP_PRIo "u ",
                usmNoPrivProtocol[0], usmNoPrivProtocol[1]));
    DEBUGMSGOID(("comparex", usmNoPrivProtocol,
                 sizeof(usmNoPrivProtocol) / sizeof(oid)));
    DEBUGMSG(("comparex", "\n"));
    if (level == SNMP_SEC_LEVEL_AUTHPRIV
        && (netsnmp_oid_equals(user->privProtocol, user->privProtocolLen,
                             usmNoPrivProtocol,
                             sizeof(usmNoPrivProtocol) / sizeof(oid)) ==
            0)) {
        DEBUGMSGTL(("usm", "Level: %d\n", level));
        DEBUGMSGTL(("usm", "User (%s) Auth Protocol: ", user->name));
        DEBUGMSGOID(("usm", user->authProtocol, user->authProtocolLen));
        DEBUGMSG(("usm", ", User Priv Protocol: "));
        DEBUGMSGOID(("usm", user->privProtocol, user->privProtocolLen));
        DEBUGMSG(("usm", "\n"));
        return 1;
    }
    if ((level == SNMP_SEC_LEVEL_AUTHPRIV
         || level == SNMP_SEC_LEVEL_AUTHNOPRIV)
        &&
        (netsnmp_oid_equals
         (user->authProtocol, user->authProtocolLen, usmNoAuthProtocol,
          sizeof(usmNoAuthProtocol) / sizeof(oid)) == 0)) {
        DEBUGMSGTL(("usm", "Level: %d\n", level));
        DEBUGMSGTL(("usm", "User (%s) Auth Protocol: ", user->name));
        DEBUGMSGOID(("usm", user->authProtocol, user->authProtocolLen));
        DEBUGMSG(("usm", ", User Priv Protocol: "));
        DEBUGMSGOID(("usm", user->privProtocol, user->privProtocolLen));
        DEBUGMSG(("usm", "\n"));
        return 1;
    }

    return 0;
}                               /* end usm_check_secLevel() */