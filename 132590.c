unsigned long ZEXPORT inflateCodesUsed(strm)
z_streamp strm;
{
    struct inflate_state FAR *state;
    if (strm == Z_NULL || strm->state == Z_NULL) return -1L;
    state = (struct inflate_state FAR *)strm->state;
    return state->next - state->codes;
}