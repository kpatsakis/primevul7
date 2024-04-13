static int jpc_pi_addpchgfrompoc(jpc_pi_t *pi, jpc_poc_t *poc)
{
	int pchgno;
	jpc_pchg_t *pchg;
	for (pchgno = 0; pchgno < poc->numpchgs; ++pchgno) {
		if (!(pchg = jpc_pchg_copy(&poc->pchgs[pchgno]))) {
			return -1;
		}
		if (jpc_pchglist_insert(pi->pchglist, -1, pchg)) {
			return -1;
		}
	}
	return 0;
}