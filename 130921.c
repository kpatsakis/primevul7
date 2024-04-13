zzip_dir_open(zzip_char_t * filename, zzip_error_t * e)
{
    return zzip_dir_open_ext_io(filename, e, 0, 0);
}