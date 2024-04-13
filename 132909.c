static int jpc_dec_cp_setfrompoc(jpc_dec_cp_t *cp, jpc_poc_t *poc, int reset)
{
	int pchgno;
	jpc_pchg_t *pchg;
	if (reset) {
		while (jpc_pchglist_numpchgs(cp->pchglist) > 0) {
			pchg = jpc_pchglist_remove(cp->pchglist, 0);
			jpc_pchg_destroy(pchg);
		}
	}
	for (pchgno = 0; pchgno < poc->numpchgs; ++pchgno) {
		if (!(pchg = jpc_pchg_copy(&poc->pchgs[pchgno]))) {
			return -1;
		}
		if (jpc_pchglist_insert(cp->pchglist, -1, pchg)) {
			return -1;
		}
	}
	return 0;
}