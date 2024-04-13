init_usm_post_config(int majorid, int minorid, void *serverarg,
                     void *clientarg)
{
    size_t          salt_integer_len = sizeof(salt_integer);

    if (sc_random((u_char *) & salt_integer, &salt_integer_len) !=
        SNMPERR_SUCCESS) {
        DEBUGMSGTL(("usm", "sc_random() failed: using time() as salt.\n"));
        salt_integer = (u_int) time(NULL);
    }

#ifdef HAVE_AES
    salt_integer_len = sizeof (salt_integer64_1);
    if (sc_random((u_char *) & salt_integer64_1, &salt_integer_len) !=
        SNMPERR_SUCCESS) {
        DEBUGMSGTL(("usm", "sc_random() failed: using time() as aes1 salt.\n"));
        salt_integer64_1 = (u_int) time(NULL);
    }
    salt_integer_len = sizeof (salt_integer64_1);
    if (sc_random((u_char *) & salt_integer64_2, &salt_integer_len) !=
        SNMPERR_SUCCESS) {
        DEBUGMSGTL(("usm", "sc_random() failed: using time() as aes2 salt.\n"));
        salt_integer64_2 = (u_int) time(NULL);
    }
#endif

#ifndef NETSNMP_DISABLE_MD5
    noNameUser = usm_create_initial_user("", usmHMACMD5AuthProtocol,
                                         OID_LENGTH(usmHMACMD5AuthProtocol),
                                         SNMP_DEFAULT_PRIV_PROTO,
                                         SNMP_DEFAULT_PRIV_PROTOLEN);
#else
    noNameUser = usm_create_initial_user("", usmHMACSHA1AuthProtocol,
                                         OID_LENGTH(usmHMACSHA1AuthProtocol),
                                         SNMP_DEFAULT_PRIV_PROTO,
                                         SNMP_DEFAULT_PRIV_PROTOLEN);
#endif

    if ( noNameUser ) {
        SNMP_FREE(noNameUser->engineID);
        noNameUser->engineIDLen = 0;
    }

    return SNMPERR_SUCCESS;
}                               /* end init_usm_post_config() */