record_byte_getter(Gif_Reader *grr)
{
    if (grr->pos < grr->length)
        return grr->v[grr->pos++];
    else
        return 0;
}