create_syncinfo_value(int type, const char *cookie, const char **uuids)
{
    BerElement *ber;
    struct berval *bvp = NULL;

    if ((ber = der_alloc()) == NULL) {
        return (NULL);
    }

    /*
     * ber_tag_t is an unsigned integer of at least 32 bits
     * used to represent a BER tag. It is commonly equivalent
     * to a unsigned long.
     * ...
     * ber_printf(...)
     * t
     *   Tag of the next element. A pointer to a ber_tag_t should be supplied. 
     */

    ber_tag_t btag = (ber_tag_t)type;

    switch (type) {
    case LDAP_TAG_SYNC_NEW_COOKIE:
        ber_printf(ber, "to", btag, cookie);
        break;
    case LDAP_TAG_SYNC_REFRESH_DELETE:
    case LDAP_TAG_SYNC_REFRESH_PRESENT:
        ber_printf(ber, "t{", btag);
        if (cookie) {
            ber_printf(ber, "s", cookie);
        }
        /* ber_printf(ber, "b",1); */
        ber_printf(ber, "}");
        break;
    case LDAP_TAG_SYNC_ID_SET:
        ber_printf(ber, "t{", btag);
        if (cookie) {
            ber_printf(ber, "s", cookie);
        }
        if (uuids) {
            ber_printf(ber, "b[v]", 1, uuids);
        }
        ber_printf(ber, "}");
        break;
    default:
        break;
    }
    ber_flatten(ber, &bvp);
    ber_free(ber, 1);

    return (bvp);
}