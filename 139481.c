static uint_fast32_t inttobits(jas_seqent_t v, int prec, bool sgnd)
{
	uint_fast32_t ret;
	ret = ((sgnd && v < 0) ? ((1 << prec) + v) : v) & JAS_ONES(prec);
	return ret;
}