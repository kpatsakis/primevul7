static int jpc_dec_process_qcd(jpc_dec_t *dec, jpc_ms_t *ms)
{
	jpc_qcd_t *qcd = &ms->parms.qcd;
	jpc_dec_tile_t *tile;

	switch (dec->state) {
	case JPC_MH:
		jpc_dec_cp_setfromqcd(dec->cp, qcd);
		break;
	case JPC_TPH:
		if (!(tile = dec->curtile)) {
			return -1;
		}
		if (tile->partno > 0) {
			return -1;
		}
		jpc_dec_cp_setfromqcd(tile->cp, qcd);
		break;
	}
	return 0;
}