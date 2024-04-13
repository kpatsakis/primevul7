static int jpc_dec_cp_setfromqcx(jpc_dec_cp_t *cp, jpc_dec_ccp_t *ccp,
  jpc_qcxcp_t *compparms, int flags)
{
	int bandno;

	/* Eliminate compiler warnings about unused variables. */
	cp = 0;

	if ((flags & JPC_QCC) || !(ccp->flags & JPC_QCC)) {
		ccp->flags |= flags | JPC_QSET;
		for (bandno = 0; bandno < compparms->numstepsizes; ++bandno) {
			ccp->stepsizes[bandno] = compparms->stepsizes[bandno];
		}
		ccp->numstepsizes = compparms->numstepsizes;
		ccp->numguardbits = compparms->numguard;
		ccp->qsty = compparms->qntsty;
	}
	return 0;
}