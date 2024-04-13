static int jpc_dec_process_eoc(jpc_dec_t *dec, jpc_ms_t *ms)
{
	int tileno;
	jpc_dec_tile_t *tile;

	/* Eliminate compiler warnings about unused variables. */
	ms = 0;

	for (tileno = 0, tile = dec->tiles; tileno < dec->numtiles; ++tileno,
	  ++tile) {
		if (tile->state == JPC_TILE_ACTIVE) {
			if (jpc_dec_tiledecode(dec, tile)) {
				return -1;
			}
		}
		/* If the tile has not yet been finalized, finalize it. */
		// OLD CODE: jpc_dec_tilefini(dec, tile);
		if (tile->state != JPC_TILE_DONE) {
			jpc_dec_tilefini(dec, tile);
		}
	}

	/* We are done processing the code stream. */
	dec->state = JPC_MT;

	return 1;
}