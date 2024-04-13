jpc_pchg_t *jpc_pchg_copy(jpc_pchg_t *pchg)
{
	jpc_pchg_t *newpchg;
	if (!(newpchg = jas_malloc(sizeof(jpc_pchg_t)))) {
		return 0;
	}
	*newpchg = *pchg;
	return newpchg;
}