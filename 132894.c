static int jpc_dec_process_ppm(jpc_dec_t *dec, jpc_ms_t *ms)
{
	jpc_ppm_t *ppm = &ms->parms.ppm;
	jpc_ppxstabent_t *ppmstabent;

	if (!dec->ppmstab) {
		if (!(dec->ppmstab = jpc_ppxstab_create())) {
			return -1;
		}
	}

	if (!(ppmstabent = jpc_ppxstabent_create())) {
		return -1;
	}
	ppmstabent->ind = ppm->ind;
	ppmstabent->data = ppm->data;
	ppm->data = 0;
	ppmstabent->len = ppm->len;
	if (jpc_ppxstab_insert(dec->ppmstab, ppmstabent)) {
		return -1;
	}
	return 0;
}