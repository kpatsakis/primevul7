static void jpc_dec_cp_resetflags(jpc_dec_cp_t *cp)
{
	int compno;
	jpc_dec_ccp_t *ccp;
	cp->flags &= (JPC_CSET | JPC_QSET);
	for (compno = 0, ccp = cp->ccps; compno < cp->numcomps;
	  ++compno, ++ccp) {
		ccp->flags = 0;
	}
}