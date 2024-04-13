zzip_dir_fdopen_ext_io(int fd, zzip_error_t * errcode_p,
                       zzip_strings_t * ext, const zzip_plugin_io_t io)
{
    zzip_error_t rv;
    ZZIP_DIR *dir;

    if ((dir = zzip_dir_alloc_ext_io(ext, io)) == NULL)
        { rv = ZZIP_OUTOFMEM; goto error; }

    dir->fd = fd;
    if ((rv = __zzip_dir_parse(dir)))
        goto error;

    dir->hdr = dir->hdr0;
    dir->refcount |= 0x10000000;

    if (errcode_p)
        *errcode_p = rv;
    return dir;
  error:
    if (dir)
        zzip_dir_free(dir);
    if (errcode_p)
        *errcode_p = rv;
    return NULL;
}