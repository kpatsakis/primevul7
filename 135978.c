static int nsim_map_delete_elem(struct bpf_offloaded_map *offmap, void *key)
{
	struct nsim_bpf_bound_map *nmap = offmap->dev_priv;
	int idx;

	if (offmap->map.map_type == BPF_MAP_TYPE_ARRAY)
		return -EINVAL;

	mutex_lock(&nmap->mutex);

	idx = nsim_map_key_find(offmap, key);
	if (idx >= 0) {
		kfree(nmap->entry[idx].key);
		kfree(nmap->entry[idx].value);
		memset(&nmap->entry[idx], 0, sizeof(nmap->entry[idx]));
	}

	mutex_unlock(&nmap->mutex);

	return idx < 0 ? idx : 0;
}