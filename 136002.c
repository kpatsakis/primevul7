nsim_map_get_next_key(struct bpf_offloaded_map *offmap,
		      void *key, void *next_key)
{
	struct nsim_bpf_bound_map *nmap = offmap->dev_priv;
	int idx = -ENOENT;

	mutex_lock(&nmap->mutex);

	if (key)
		idx = nsim_map_key_find(offmap, key);
	if (idx == -ENOENT)
		idx = 0;
	else
		idx++;

	for (; idx < ARRAY_SIZE(nmap->entry); idx++) {
		if (nmap->entry[idx].key) {
			memcpy(next_key, nmap->entry[idx].key,
			       offmap->map.key_size);
			break;
		}
	}

	mutex_unlock(&nmap->mutex);

	if (idx == ARRAY_SIZE(nmap->entry))
		return -ENOENT;
	return 0;
}