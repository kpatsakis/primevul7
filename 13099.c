restore_subexpr(regbehind_T *bp)
{
    int i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    if (!rex.need_clear_subexpr)
    {
	for (i = 0; i < NSUBEXP; ++i)
	{
	    if (REG_MULTI)
	    {
		rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
		rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	    }
	    else
	    {
		rex.reg_startp[i] = bp->save_start[i].se_u.ptr;
		rex.reg_endp[i] = bp->save_end[i].se_u.ptr;
	    }
	}
    }
}