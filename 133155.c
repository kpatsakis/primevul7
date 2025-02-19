void jpc_pchglist_destroy(jpc_pchglist_t *pchglist)
{
	int pchgno;
	if (pchglist->pchgs) {
		for (pchgno = 0; pchgno < pchglist->numpchgs; ++pchgno) {
			jpc_pchg_destroy(pchglist->pchgs[pchgno]);
		}
		jas_free(pchglist->pchgs);
	}
	jas_free(pchglist);
}