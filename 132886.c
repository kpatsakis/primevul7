static int jpc_dec_cp_setfromcoc(jpc_dec_cp_t *cp, jpc_coc_t *coc)
{
	jpc_dec_cp_setfromcox(cp, &cp->ccps[coc->compno], &coc->compparms, JPC_COC);
	return 0;
}