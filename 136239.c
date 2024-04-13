sync_olcsn2ulong(char *csn) {
    struct tm pt = {0};
    char *ret = strptime(csn, "%Y%m%d%H%M%S", &pt);
    PR_ASSERT(ret);
    if (ret == NULL) {
        return SYNC_INVALID_CHANGENUM;
    }
    time_t pepoch = mktime(&pt);
    unsigned long px = (unsigned long)pepoch;
    PR_ASSERT(px >= CSN_OFFSET);
    if (px < CSN_OFFSET) {
        return SYNC_INVALID_CHANGENUM;
    }
    return px - CSN_OFFSET;
}