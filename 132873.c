static int jpc_dec_process_crg(jpc_dec_t *dec, jpc_ms_t *ms)
{
	int cmptno;
	jpc_dec_cmpt_t *cmpt;
	jpc_crg_t *crg;

	crg = &ms->parms.crg;
	for (cmptno = 0, cmpt = dec->cmpts; cmptno < dec->numcomps; ++cmptno,
	  ++cmpt) {
		/* Ignore the information in the CRG marker segment for now.
		  This information serves no useful purpose for decoding anyhow.
		  Some other parts of the code need to be changed if these lines
		  are uncommented.
		cmpt->hsubstep = crg->comps[cmptno].hoff;
		cmpt->vsubstep = crg->comps[cmptno].voff;
		*/
	}
	return 0;
}