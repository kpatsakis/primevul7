__zzip_try_open(zzip_char_t * filename, int filemode,
                zzip_strings_t * ext, zzip_plugin_io_t io)
{
    auto char file[PATH_MAX];
    int fd;
    zzip_size_t len = strlen(filename);

    if (len + 4 >= PATH_MAX)
        { errno = ENAMETOOLONG; return -1; }
    memcpy(file, filename, len + 1);

    if (! io)
        io = zzip_get_default_io();
    if (! ext)
        ext = zzip_get_default_ext();

    for (; *ext; ++ext)
    {
        strcpy(file + len, *ext);
        fd = (io->fd.open)(file, filemode);
        if (fd != -1)
            return fd;
    }
    return -1;
}