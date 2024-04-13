xz_load(xz_statep state, unsigned char *buf, unsigned int len,
        unsigned int *have)
{
    int ret;

    *have = 0;
    do {
        ret = read(state->fd, buf + *have, len - *have);
        if (ret <= 0)
            break;
        *have += ret;
    } while (*have < len);
    if (ret < 0) {
        xz_error(state, -1, strerror(errno));
        return -1;
    }
    if (ret == 0)
        state->eof = 1;
    return 0;
}