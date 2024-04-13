static int jpc_dec_cp_setfromrgn(jpc_dec_cp_t *cp, jpc_rgn_t *rgn)
{
	jpc_dec_ccp_t *ccp;
	ccp = &cp->ccps[rgn->compno];
	ccp->roishift = rgn->roishift;
	return 0;
}