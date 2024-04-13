int jpc_pi_addpchg(jpc_pi_t *pi, jpc_pocpchg_t *pchg)
{
	return jpc_pchglist_insert(pi->pchglist, -1, pchg);
}