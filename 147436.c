qemuProcessReadLog(qemuDomainLogContextPtr logCtxt,
                   char **msg,
                   size_t max)
{
    char *buf;
    ssize_t got;
    char *eol;
    char *filter_next;
    size_t skip;

    if ((got = qemuDomainLogContextRead(logCtxt, &buf)) < 0)
        return -1;

    /* Filter out debug messages from intermediate libvirt process */
    filter_next = buf;
    while ((eol = strchr(filter_next, '\n'))) {
        *eol = '\0';
        if (virLogProbablyLogMessage(filter_next) ||
            strstr(filter_next, "char device redirected to")) {
            skip = (eol + 1) - filter_next;
            memmove(filter_next, eol + 1, buf + got - eol);
            got -= skip;
        } else {
            filter_next = eol + 1;
            *eol = '\n';
        }
    }
    filter_next = NULL; /* silence false coverity warning */

    if (got > 0 &&
        buf[got - 1] == '\n') {
        buf[got - 1] = '\0';
        got--;
    }

    if (max > 0 && got > max) {
        skip = got - max;

        if (buf[skip - 1] != '\n' &&
            (eol = strchr(buf + skip, '\n')) &&
            !virStringIsEmpty(eol + 1))
            skip = eol + 1 - buf;

        memmove(buf, buf + skip, got - skip + 1);
        got -= skip;
    }

    buf = g_renew(char, buf, got + 1);
    *msg = buf;
    return 0;
}