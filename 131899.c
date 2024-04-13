fpad8(OutputFile *fo)
{
    unsigned len = fo->st_size();
    unsigned d = 7u & (0 - len);
    upx_uint64_t zero = 0;
    fo->write(&zero, d);
    return d + len;
}