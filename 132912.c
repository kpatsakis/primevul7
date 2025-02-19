int jpc_pptstabwrite(jas_stream_t *out, jpc_ppxstab_t *tab)
{
	int i;
	jpc_ppxstabent_t *ent;
	for (i = 0; i < tab->numents; ++i) {
		ent = tab->ents[i];
		if (jas_stream_write(out, ent->data, ent->len) != JAS_CAST(int, ent->len)) {
			return -1;
		}
	}
	return 0;
}