regstack_push(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    if ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	return NULL;
    }
    if (ga_grow(&regstack, sizeof(regitem_T)) == FAIL)
	return NULL;

    rp = (regitem_T *)((char *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += sizeof(regitem_T);
    return rp;
}