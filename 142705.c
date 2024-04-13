snmpv3_privtype_conf(const char *word, char *cptr)
{
    int priv_type = usm_lookup_priv_type(cptr);
    if (priv_type < 0)
        config_perror("Unknown privacy type");
    defaultPrivType = sc_get_priv_oid(priv_type, &defaultPrivTypeLen);
    DEBUGMSGTL(("snmpv3", "set default privacy type: %s\n", cptr));
}