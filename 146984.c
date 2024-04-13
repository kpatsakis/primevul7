xz_skip(xz_statep state, uint64_t len)
{
    unsigned n;

    /* skip over len bytes or reach end-of-file, whichever comes first */
    while (len)
        /* skip over whatever is in output buffer */
        if (state->have) {
            n = (uint64_t) state->have > len ?
                (unsigned) len : state->have;
            state->have -= n;
            state->next += n;
            state->pos += n;
            len -= n;
        }

    /* output buffer empty -- return if we're at the end of the input */
        else if (state->eof && state->strm.avail_in == 0)
            break;

    /* need more data to skip -- load up output buffer */
        else {
            /* get more output, looking for header if required */
            if (xz_make(state) == -1)
                return -1;
        }
    return 0;
}