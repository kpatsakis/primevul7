xz_avail_zstrm(xz_statep state)
{
    int ret;
    state->strm.avail_in = state->zstrm.avail_in;
    state->strm.next_in = state->zstrm.next_in;
    ret = xz_avail(state);
    state->zstrm.avail_in = (uInt) state->strm.avail_in;
    state->zstrm.next_in = (Bytef *) state->strm.next_in;
    return ret;
}