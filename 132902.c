static jpc_dec_cp_t *jpc_dec_cp_copy(jpc_dec_cp_t *cp)
{
	jpc_dec_cp_t *newcp;
	jpc_dec_ccp_t *newccp;
	jpc_dec_ccp_t *ccp;
	int compno;

	if (!(newcp = jpc_dec_cp_create(cp->numcomps))) {
		return 0;
	}
	newcp->flags = cp->flags;
	newcp->prgord = cp->prgord;
	newcp->numlyrs = cp->numlyrs;
	newcp->mctid = cp->mctid;
	newcp->csty = cp->csty;
	jpc_pchglist_destroy(newcp->pchglist);
	newcp->pchglist = 0;
	if (!(newcp->pchglist = jpc_pchglist_copy(cp->pchglist))) {
		jas_free(newcp);
		return 0;
	}
	for (compno = 0, newccp = newcp->ccps, ccp = cp->ccps;
	  compno < cp->numcomps;
	  ++compno, ++newccp, ++ccp) {
		*newccp = *ccp;
	}
	return newcp;
}