__libxml2_xzread(xzFile file, void *buf, unsigned len)
{
    unsigned got, n;
    xz_statep state;
    lzma_stream *strm;

    /* get internal structure */
    if (file == NULL)
        return -1;
    state = (xz_statep) file;
    strm = &(state->strm);

    /* check that we're reading and that there's no error */
    if (state->err != LZMA_OK)
        return -1;

    /* since an int is returned, make sure len fits in one, otherwise return
     * with an error (this avoids the flaw in the interface) */
    if ((int) len < 0) {
        xz_error(state, LZMA_BUF_ERROR,
                 "requested length does not fit in int");
        return -1;
    }

    /* if len is zero, avoid unnecessary operations */
    if (len == 0)
        return 0;

    /* process a skip request */
    if (state->seek) {
        state->seek = 0;
        if (xz_skip(state, state->skip) == -1)
            return -1;
    }

    /* get len bytes to buf, or less than len if at the end */
    got = 0;
    do {
        /* first just try copying data from the output buffer */
        if (state->have) {
            n = state->have > len ? len : state->have;
            memcpy(buf, state->next, n);
            state->next += n;
            state->have -= n;
        }

        /* output buffer empty -- return if we're at the end of the input */
        else if (state->eof && strm->avail_in == 0)
            break;

        /* need output data -- for small len or new stream load up our output
         * buffer */
        else if (state->how == LOOK || len < (state->size << 1)) {
            /* get more output, looking for header if required */
            if (xz_make(state) == -1)
                return -1;
            continue;           /* no progress yet -- go back to memcpy() above */
            /* the copy above assures that we will leave with space in the
             * output buffer, allowing at least one gzungetc() to succeed */
        }

        /* large len -- read directly into user buffer */
        else if (state->how == COPY) {  /* read directly */
            if (xz_load(state, buf, len, &n) == -1)
                return -1;
        }

        /* large len -- decompress directly into user buffer */
        else {                  /* state->how == LZMA */
            strm->avail_out = len;
            strm->next_out = buf;
            if (xz_decomp(state) == -1)
                return -1;
            n = state->have;
            state->have = 0;
        }

        /* update progress */
        len -= n;
        buf = (char *) buf + n;
        got += n;
        state->pos += n;
    } while (len);

    /* return number of bytes read into user buffer (will fit in int) */
    return (int) got;
}