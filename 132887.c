static int jpc_dec_cp_isvalid(jpc_dec_cp_t *cp)
{
	uint_fast16_t compcnt;
	jpc_dec_ccp_t *ccp;

	if (!(cp->flags & JPC_CSET) || !(cp->flags & JPC_QSET)) {
		return 0;
	}
	for (compcnt = cp->numcomps, ccp = cp->ccps; compcnt > 0; --compcnt,
	  ++ccp) {
		/* Is there enough step sizes for the number of bands? */
		if ((ccp->qsty != JPC_QCX_SIQNT && JAS_CAST(int, ccp->numstepsizes) < 3 *
		  ccp->numrlvls - 2) || (ccp->qsty == JPC_QCX_SIQNT &&
		  ccp->numstepsizes != 1)) {
			return 0;
		}
	}
	return 1;
}