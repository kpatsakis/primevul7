static s_bucket *FNAME(find_bucket)(Channel *channel, const unsigned int val)
{
    spice_extra_assert(val < (0x1U << BPC));

    /* The and (&) here is to avoid buffer overflows in case of garbage or malicious
     * attempts. Is much faster then using comparisons and save us from such situations.
     * Note that on normal build the check above won't be compiled as this code path
     * is pretty hot and would cause speed regressions.
     */
    return channel->_buckets_ptrs[val & ((1U << BPC) - 1)];
}