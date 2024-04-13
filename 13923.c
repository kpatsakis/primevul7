static int ssl_password_cb(char *buf, int size, int flags, void *userdata)
{
    RedsState *reds = (RedsState*) userdata;
    char *pass = reds->config->ssl_parameters.keyfile_password;
    int len = g_strlcpy(buf, pass, size);
    if (len >= size) {
        return 0;
    }

    return len;
}