zzip_dir_open_ext_io(zzip_char_t * filename, zzip_error_t * e,
                     zzip_strings_t * ext, zzip_plugin_io_t io)
{
    int fd;

    if (! io)
        io = zzip_get_default_io();
    if (! ext)
        ext = zzip_get_default_ext();

    fd = (io->fd.open)(filename, O_RDONLY | O_BINARY);
    if (fd != -1)
    {
        return zzip_dir_fdopen_ext_io(fd, e, ext, io);
    } else
    {
        fd = __zzip_try_open(filename, O_RDONLY | O_BINARY, ext, io);
        if (fd != -1)
        {
            return zzip_dir_fdopen_ext_io(fd, e, ext, io);
        } else
        {
            if (e)
                { *e = ZZIP_DIR_OPEN; }
            return 0;
        }
    }
}