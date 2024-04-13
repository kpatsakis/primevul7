jpc_pchglist_t *jpc_pchglist_copy(jpc_pchglist_t *pchglist)
{
	jpc_pchglist_t *newpchglist;
	jpc_pchg_t *newpchg;
	int pchgno;
	if (!(newpchglist = jpc_pchglist_create())) {
		return 0;
	}
	for (pchgno = 0; pchgno < pchglist->numpchgs; ++pchgno) {
		if (!(newpchg = jpc_pchg_copy(pchglist->pchgs[pchgno])) ||
		  jpc_pchglist_insert(newpchglist, -1, newpchg)) {
			jpc_pchglist_destroy(newpchglist);
			return 0;
		}
	}
	return newpchglist;
}