usm_parse_config_usmUser(const char *token, char *line)
{
    struct usmUser *uptr;

    uptr = usm_read_user(line);
    if ( uptr)
        usm_add_user(uptr);
}