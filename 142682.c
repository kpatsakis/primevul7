snmpv3_authtype_conf(const char *word, char *cptr)
{
    int auth_type = usm_lookup_auth_type(cptr);
    if (auth_type < 0)
        config_perror("Unknown authentication type");
    defaultAuthType = sc_get_auth_oid(auth_type, &defaultAuthTypeLen);
    DEBUGMSGTL(("snmpv3", "set default authentication type: %s\n", cptr));
}