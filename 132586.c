int ZEXPORT inflateUndermine(strm, subvert)
z_streamp strm;
int subvert;
{
    struct inflate_state FAR *state;

    if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;
    state = (struct inflate_state FAR *)strm->state;
#ifdef INFLATE_ALLOW_INVALID_DISTANCE_TOOFAR_ARRR
    state->sane = !subvert;
    return Z_OK;
#else
    state->sane = 1;
    return Z_DATA_ERROR;
#endif
}