nsim_map_lookup_elem(struct bpf_offloaded_map *offmap, void *key, void *value)
{
	struct nsim_bpf_bound_map *nmap = offmap->dev_priv;
	int idx;

	mutex_lock(&nmap->mutex);

	idx = nsim_map_key_find(offmap, key);
	if (idx >= 0)
		memcpy(value, nmap->entry[idx].value, offmap->map.value_size);

	mutex_unlock(&nmap->mutex);

	return idx < 0 ? idx : 0;
}