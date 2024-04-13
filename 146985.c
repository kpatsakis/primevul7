is_format_xz(xz_statep state)
{
    lzma_stream *strm = &(state->strm);

    return strm->avail_in >= 6 && memcmp(state->in, "\3757zXZ", 6) == 0;
}