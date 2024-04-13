static jas_seqent_t bitstoint(uint_fast32_t v, int prec, bool sgnd)
{
	jas_seqent_t ret;
	v &= JAS_ONES(prec);
	ret = (sgnd && (v & (1 << (prec - 1)))) ? (v - (1 << prec)) : v;
	return ret;
}