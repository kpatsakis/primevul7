force_wrap(void)
{
    oldcol = column;
    trim_trailing();
    strcpy_DYN(&outbuf, trailer);
    column = indent;
}