__libxml2_xzopen(const char *path, const char *mode)
{
    return xz_open(path, -1, mode);
}