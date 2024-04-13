static int jpc_dec_cp_setfromcod(jpc_dec_cp_t *cp, jpc_cod_t *cod)
{
	jpc_dec_ccp_t *ccp;
	int compno;
	cp->flags |= JPC_CSET;
	cp->prgord = cod->prg;
	if (cod->mctrans) {
		cp->mctid = (cod->compparms.qmfbid == JPC_COX_INS) ? (JPC_MCT_ICT) : (JPC_MCT_RCT);
	} else {
		cp->mctid = JPC_MCT_NONE;
	}
	cp->numlyrs = cod->numlyrs;
	cp->csty = cod->csty & (JPC_COD_SOP | JPC_COD_EPH);
	for (compno = 0, ccp = cp->ccps; compno < cp->numcomps;
	  ++compno, ++ccp) {
		jpc_dec_cp_setfromcox(cp, ccp, &cod->compparms, 0);
	}
	cp->flags |= JPC_CSET;
	return 0;
}