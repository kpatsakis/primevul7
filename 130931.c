zzip_dir_close(ZZIP_DIR * dir)
{
    dir->refcount &= ~0x10000000;       /* explicit dir close */
    return zzip_dir_free(dir);
}