stuffReadbuffLen(char_u *s, long len)
{
    add_buff(&readbuf1, s, len);
}