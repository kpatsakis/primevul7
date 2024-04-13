static int jpc_dec_cp_setfromqcc(jpc_dec_cp_t *cp, jpc_qcc_t *qcc)
{
	return jpc_dec_cp_setfromqcx(cp, &cp->ccps[qcc->compno], &qcc->compparms, JPC_QCC);
}