nsim_map_update_elem(struct bpf_offloaded_map *offmap,
		     void *key, void *value, u64 flags)
{
	struct nsim_bpf_bound_map *nmap = offmap->dev_priv;
	int idx, err = 0;

	mutex_lock(&nmap->mutex);

	idx = nsim_map_key_find(offmap, key);
	if (idx < 0 && flags == BPF_EXIST) {
		err = idx;
		goto exit_unlock;
	}
	if (idx >= 0 && flags == BPF_NOEXIST) {
		err = -EEXIST;
		goto exit_unlock;
	}

	if (idx < 0) {
		for (idx = 0; idx < ARRAY_SIZE(nmap->entry); idx++)
			if (!nmap->entry[idx].key)
				break;
		if (idx == ARRAY_SIZE(nmap->entry)) {
			err = -E2BIG;
			goto exit_unlock;
		}

		err = nsim_map_alloc_elem(offmap, idx);
		if (err)
			goto exit_unlock;
	}

	memcpy(nmap->entry[idx].key, key, offmap->map.key_size);
	memcpy(nmap->entry[idx].value, value, offmap->map.value_size);
exit_unlock:
	mutex_unlock(&nmap->mutex);

	return err;
}