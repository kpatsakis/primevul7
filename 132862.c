static jpc_dec_mstabent_t *jpc_dec_mstab_lookup(uint_fast16_t id)
{
	jpc_dec_mstabent_t *mstabent;
	for (mstabent = jpc_dec_mstab; mstabent->id != 0; ++mstabent) {
		if (mstabent->id == id) {
			break;
		}
	}
	return mstabent;
}