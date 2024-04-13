source_runtime(char_u *name, int flags)
{
    return source_in_path(p_rtp, name, flags, NULL);
}