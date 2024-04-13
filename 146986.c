gz_next4(xz_statep state, unsigned long *ret)
{
    int ch;
    unsigned long val;
    z_streamp strm = &(state->zstrm);

    val = NEXTZ();
    val += (unsigned) NEXTZ() << 8;
    val += (unsigned long) NEXTZ() << 16;
    ch = NEXTZ();
    if (ch == -1)
        return -1;
    val += (unsigned long) ch << 24;
    *ret = val;
    return 0;
}