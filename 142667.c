usm_check_secLevel_vs_protocols(int level,
                                const oid * authProtocol,
                                u_int authProtocolLen,
                                const oid * privProtocol,
                                u_int privProtocolLen)
{

    if (level == SNMP_SEC_LEVEL_AUTHPRIV
        &&
        (netsnmp_oid_equals
         (privProtocol, privProtocolLen, usmNoPrivProtocol,
          sizeof(usmNoPrivProtocol) / sizeof(oid)) == 0)) {
        DEBUGMSGTL(("usm", "Level: %d\n", level));
        DEBUGMSGTL(("usm", "Auth Protocol: "));
        DEBUGMSGOID(("usm", authProtocol, authProtocolLen));
        DEBUGMSG(("usm", ", Priv Protocol: "));
        DEBUGMSGOID(("usm", privProtocol, privProtocolLen));
        DEBUGMSG(("usm", "\n"));
        return 1;
    }
    if ((level == SNMP_SEC_LEVEL_AUTHPRIV
         || level == SNMP_SEC_LEVEL_AUTHNOPRIV)
        &&
        (netsnmp_oid_equals
         (authProtocol, authProtocolLen, usmNoAuthProtocol,
          sizeof(usmNoAuthProtocol) / sizeof(oid)) == 0)) {
        DEBUGMSGTL(("usm", "Level: %d\n", level));
        DEBUGMSGTL(("usm", "Auth Protocol: "));
        DEBUGMSGOID(("usm", authProtocol, authProtocolLen));
        DEBUGMSG(("usm", ", Priv Protocol: "));
        DEBUGMSGOID(("usm", privProtocol, privProtocolLen));
        DEBUGMSG(("usm", "\n"));
        return 1;
    }

    return 0;

}                               /* end usm_check_secLevel_vs_protocols() */