jpc_pchg_t *jpc_pchglist_remove(jpc_pchglist_t *pchglist, int pchgno)
{
	int i;
	jpc_pchg_t *pchg;
	assert(pchgno < pchglist->numpchgs);
	pchg = pchglist->pchgs[pchgno];
	for (i = pchgno + 1; i < pchglist->numpchgs; ++i) {
		pchglist->pchgs[i - 1] = pchglist->pchgs[i];
	}
	--pchglist->numpchgs;
	return pchg;
}