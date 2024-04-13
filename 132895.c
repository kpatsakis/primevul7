static int jpc_dec_cp_setfromcox(jpc_dec_cp_t *cp, jpc_dec_ccp_t *ccp,
  jpc_coxcp_t *compparms, int flags)
{
	int rlvlno;

	/* Eliminate compiler warnings about unused variables. */
	cp = 0;

	if ((flags & JPC_COC) || !(ccp->flags & JPC_COC)) {
		ccp->numrlvls = compparms->numdlvls + 1;
		ccp->cblkwidthexpn = JPC_COX_GETCBLKSIZEEXPN(
		  compparms->cblkwidthval);
		ccp->cblkheightexpn = JPC_COX_GETCBLKSIZEEXPN(
		  compparms->cblkheightval);
		ccp->qmfbid = compparms->qmfbid;
		ccp->cblkctx = compparms->cblksty;
		ccp->csty = compparms->csty & JPC_COX_PRT;
		for (rlvlno = 0; rlvlno < compparms->numrlvls; ++rlvlno) {
			ccp->prcwidthexpns[rlvlno] =
			  compparms->rlvls[rlvlno].parwidthval;
			ccp->prcheightexpns[rlvlno] =
			  compparms->rlvls[rlvlno].parheightval;
		}
		ccp->flags |= flags | JPC_CSET;
	}
	return 0;
}