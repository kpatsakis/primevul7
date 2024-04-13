zzip_dir_alloc_ext_io(zzip_strings_t * ext, const zzip_plugin_io_t io)
{
    ZZIP_DIR *dir;
    if ((dir = (ZZIP_DIR *) calloc(1, sizeof(*dir))) == NULL)
        return 0;

    /* dir->fileext is currently unused - so what, still initialize it */
    dir->fileext = ext ? ext : zzip_get_default_ext();
    dir->io = io ? io : zzip_get_default_io();
    return dir;
}