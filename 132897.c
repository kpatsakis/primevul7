static int jpc_dec_process_poc(jpc_dec_t *dec, jpc_ms_t *ms)
{
	jpc_poc_t *poc = &ms->parms.poc;
	jpc_dec_tile_t *tile;
	switch (dec->state) {
	case JPC_MH:
		if (jpc_dec_cp_setfrompoc(dec->cp, poc, 1)) {
			return -1;
		}
		break;
	case JPC_TPH:
		if (!(tile = dec->curtile)) {
			return -1;
		}
		if (!tile->partno) {
			if (jpc_dec_cp_setfrompoc(tile->cp, poc, (!tile->partno))) {
				return -1;
			}
		} else {
			jpc_pi_addpchgfrompoc(tile->pi, poc);
		}
		break;
	}
	return 0;
}