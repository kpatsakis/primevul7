record_block_getter(uint8_t *p, uint32_t s, Gif_Reader *grr)
{
    uint32_t ncopy = (grr->pos + s <= grr->length ? s : grr->length - grr->pos);
    memcpy(p, &grr->v[grr->pos], ncopy);
    grr->pos += ncopy;
    if (ncopy < s)
        memset(p + ncopy, 0, s - ncopy);
    return ncopy;
}