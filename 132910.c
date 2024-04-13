static int jpc_dec_process_soc(jpc_dec_t *dec, jpc_ms_t *ms)
{
	/* Eliminate warnings about unused variables. */
	ms = 0;

	/* We should expect to encounter a SIZ marker segment next. */
	dec->state = JPC_MHSIZ;

	return 0;
}