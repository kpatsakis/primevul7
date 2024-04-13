reg_save(regsave_T *save, garray_T *gap)
{
    if (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.input - rex.line);
	save->rs_u.pos.lnum = rex.lnum;
    }
    else
	save->rs_u.ptr = rex.input;
    save->rs_len = gap->ga_len;
}