zzip_dir_read(ZZIP_DIR * dir, ZZIP_DIRENT * d)
{
    if (! dir || ! dir->hdr || ! d)
        return 0;

    d->d_compr = dir->hdr->d_compr;
    d->d_csize = dir->hdr->d_csize;
    d->st_size = dir->hdr->d_usize;
    d->d_name = dir->hdr->d_name;

    if (! dir->hdr->d_reclen)
    {
        dir->hdr = 0;
    } else
    {
        dir->hdr = (struct zzip_dir_hdr *)
            ((char *) dir->hdr + dir->hdr->d_reclen);
    }

    return 1;
}