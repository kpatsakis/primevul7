static int jpc_dec_process_unk(jpc_dec_t *dec, jpc_ms_t *ms)
{
	/* Eliminate compiler warnings about unused variables. */
	dec = 0;

	jas_eprintf("warning: ignoring unknown marker segment\n");
	jpc_ms_dump(ms, stderr);
	return 0;
}