file_byte_getter(Gif_Reader *grr)
{
    int i = getc(grr->f);
    if (i != EOF) {
        ++grr->pos;
        return i;
    } else
        return 0;
}