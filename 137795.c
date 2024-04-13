file_block_getter(uint8_t *p, uint32_t s, Gif_Reader *grr)
{
    size_t nread = fread(p, 1, s, grr->f);
    if (nread < s)
        memset(p + nread, 0, s - nread);
    grr->pos += nread;
    return nread;
}