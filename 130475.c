wrap_concat1(const char *src)
{
    int need = (int) strlen(src);
    wrap_concat(src, need, w1ST | wEND);
}