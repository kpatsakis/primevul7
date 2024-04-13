zzip_dir_fdopen(int fd, zzip_error_t * errcode_p)
{
    return zzip_dir_fdopen_ext_io(fd, errcode_p, 0, 0);
}