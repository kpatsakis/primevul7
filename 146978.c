xz_make(xz_statep state)
{
    lzma_stream *strm = &(state->strm);

    if (state->how == LOOK) {   /* look for lzma / gzip header */
        if (xz_head(state) == -1)
            return -1;
        if (state->have)        /* got some data from xz_head() */
            return 0;
    }
    if (state->how == COPY) {   /* straight copy */
        if (xz_load(state, state->out, state->size << 1, &(state->have)) ==
            -1)
            return -1;
        state->next = state->out;
    } else if (state->how == LZMA || state->how == GZIP) {      /* decompress */
        strm->avail_out = state->size << 1;
        strm->next_out = state->out;
        if (xz_decomp(state) == -1)
            return -1;
    }
    return 0;
}