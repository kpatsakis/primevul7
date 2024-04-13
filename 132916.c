static int jpc_dec_cp_setfromqcd(jpc_dec_cp_t *cp, jpc_qcd_t *qcd)
{
	int compno;
	jpc_dec_ccp_t *ccp;
	for (compno = 0, ccp = cp->ccps; compno < cp->numcomps;
	  ++compno, ++ccp) {
		jpc_dec_cp_setfromqcx(cp, ccp, &qcd->compparms, 0);
	}
	cp->flags |= JPC_QSET;
	return 0;
}