fpad4(OutputFile *fo)
{
    unsigned len = fo->st_size();
    unsigned d = 3u & (0 - len);
    unsigned zero = 0;
    fo->write(&zero, d);
    return d + len;
}