snmpCreateOidFromStr(const char *str, oid **name, int *nl)
{
    char const *delim = ".";

    *name = NULL;
    *nl = 0;
    const char *s = str;

    /* Parse the OID string into oid bits */
    while (size_t len = strcspn(s, delim)) {
        *name = (oid*)xrealloc(*name, sizeof(oid) * ((*nl) + 1));
        (*name)[*nl] = atoi(s); // stops at the '.' delimiter
        ++(*nl);
        // exit with true when the last octet has been parsed
        if (s[len] == '\0')
            return true;
        s += len+1;
    }

    // if we aborted before the lst octet was found, return false.
    safe_free(name);
    return false;
}