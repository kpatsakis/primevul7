static int jpc_dec_process_cod(jpc_dec_t *dec, jpc_ms_t *ms)
{
	jpc_cod_t *cod = &ms->parms.cod;
	jpc_dec_tile_t *tile;

	switch (dec->state) {
	case JPC_MH:
		jpc_dec_cp_setfromcod(dec->cp, cod);
		break;
	case JPC_TPH:
		if (!(tile = dec->curtile)) {
			return -1;
		}
		if (tile->partno != 0) {
			return -1;
		}
		jpc_dec_cp_setfromcod(tile->cp, cod);
		break;
	}
	return 0;
}