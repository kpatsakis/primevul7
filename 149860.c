pair_byte_cmp(const void *pair1, const void *pair2)
{
    long diff = ((pair_t*)pair1)->byte_pos - ((pair_t*)pair2)->byte_pos;
#if SIZEOF_LONG > SIZEOF_INT
    return diff ? diff > 0 ? 1 : -1 : 0;
#else
    return (int)diff;
#endif
}