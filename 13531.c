mb_adjust_opend(oparg_T *oap)
{
    char_u	*p;

    if (oap->inclusive)
    {
	p = ml_get(oap->end.lnum);
	oap->end.col += mb_tail_off(p, p + oap->end.col);
    }
}