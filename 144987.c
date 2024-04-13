str_to_uid(pam_handle_t *pamh, const char *value, uid_t default_value, const char *param) {
    unsigned long ul;
    char *ep;
    uid_t uid;

    errno = 0;
    ul = strtoul(value, &ep, 10);
    if (!(ul >= MAX_UID_VALUE
        || (uid_t)ul >= MAX_UID_VALUE
        || (errno != 0 && ul == 0)
        || value == ep
        || *ep != '\0')) {
        uid = (uid_t)ul;
    } else {
        pam_syslog(pamh, LOG_ERR, "Parameter \"%s=%s\" invalid, "
                   "setting to %u", param, value, default_value);
        uid = default_value;
    }

    return uid;
}