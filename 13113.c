reg_save_equal(regsave_T *save)
{
    if (REG_MULTI)
	return rex.lnum == save->rs_u.pos.lnum
				  && rex.input == rex.line + save->rs_u.pos.col;
    return rex.input == save->rs_u.ptr;
}