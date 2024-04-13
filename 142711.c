get_default_privtype(size_t * len)
{
    if (defaultPrivType == NULL) {
        defaultPrivType = SNMP_DEFAULT_PRIV_PROTO;
        defaultPrivTypeLen = SNMP_DEFAULT_PRIV_PROTOLEN;
    }
    if (len)
        *len = defaultPrivTypeLen;
    return defaultPrivType;
}