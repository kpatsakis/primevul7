sync_ulong2olcsn(unsigned long chgnr, char *buf) {
    PR_ASSERT(buf);
    unsigned long x = chgnr + CSN_OFFSET;
    time_t epoch = x;
    struct tm t = {0};
    localtime_r(&epoch, &t);
    strftime(buf, 15, "%Y%m%d%H%M%S", &t);
}