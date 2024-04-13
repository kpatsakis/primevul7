zzip_dir_free(ZZIP_DIR * dir)
{
    if (dir->refcount)
        return (dir->refcount); /* still open files attached */

    if (dir->fd >= 0)
        dir->io->fd.close(dir->fd);
    if (dir->hdr0)
        free(dir->hdr0);
    if (dir->cache.fp)
        free(dir->cache.fp);
    if (dir->cache.buf32k)
        free(dir->cache.buf32k);
    if (dir->realname)
        free(dir->realname);
    free(dir);
    return 0;
}