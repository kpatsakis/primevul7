static int jpc_dec_process_ppt(jpc_dec_t *dec, jpc_ms_t *ms)
{
	jpc_ppt_t *ppt = &ms->parms.ppt;
	jpc_dec_tile_t *tile;
	jpc_ppxstabent_t *pptstabent;

	tile = dec->curtile;
	if (!tile->pptstab) {
		if (!(tile->pptstab = jpc_ppxstab_create())) {
			return -1;
		}
	}
	if (!(pptstabent = jpc_ppxstabent_create())) {
		return -1;
	}
	pptstabent->ind = ppt->ind;
	pptstabent->data = ppt->data;
	ppt->data = 0;
	pptstabent->len = ppt->len;
	if (jpc_ppxstab_insert(tile->pptstab, pptstabent)) {
		return -1;
	}
	return 0;
}