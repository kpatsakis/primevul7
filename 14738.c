source_in_path(char_u *path, char_u *name, int flags, int *ret_sid)
{
    return do_in_path_and_pp(path, name, flags, source_callback, ret_sid);
}