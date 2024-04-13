is_format_lzma(xz_statep state)
{
    lzma_stream *strm = &(state->strm);

    lzma_filter filter;
    lzma_options_lzma *opt;
    uint32_t dict_size;
    uint64_t uncompressed_size;
    size_t i;

    if (strm->avail_in < 13)
        return 0;

    filter.id = LZMA_FILTER_LZMA1;
    if (lzma_properties_decode(&filter, NULL, state->in, 5) != LZMA_OK)
        return 0;

    opt = filter.options;
    dict_size = opt->dict_size;
    free(opt); /* we can't use xmlFree on a string returned by zlib */

    /* A hack to ditch tons of false positives: We allow only dictionary
     * sizes that are 2^n or 2^n + 2^(n-1) or UINT32_MAX. LZMA_Alone
     * created only files with 2^n, but accepts any dictionary size.
     * If someone complains, this will be reconsidered.
     */
    if (dict_size != UINT32_MAX) {
        uint32_t d = dict_size - 1;

        d |= d >> 2;
        d |= d >> 3;
        d |= d >> 4;
        d |= d >> 8;
        d |= d >> 16;
        ++d;
        if (d != dict_size || dict_size == 0)
            return 0;
    }

    /* Another hack to ditch false positives: Assume that if the
     * uncompressed size is known, it must be less than 256 GiB.
     * Again, if someone complains, this will be reconsidered.
     */
    uncompressed_size = 0;
    for (i = 0; i < 8; ++i)
        uncompressed_size |= (uint64_t) (state->in[5 + i]) << (i * 8);

    if (uncompressed_size != UINT64_MAX
        && uncompressed_size > (UINT64_C(1) << 38))
        return 0;

    return 1;
}