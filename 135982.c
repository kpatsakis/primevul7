nsim_map_alloc_elem(struct bpf_offloaded_map *offmap, unsigned int idx)
{
	struct nsim_bpf_bound_map *nmap = offmap->dev_priv;

	nmap->entry[idx].key = kmalloc(offmap->map.key_size, GFP_USER);
	if (!nmap->entry[idx].key)
		return -ENOMEM;
	nmap->entry[idx].value = kmalloc(offmap->map.value_size, GFP_USER);
	if (!nmap->entry[idx].value) {
		kfree(nmap->entry[idx].key);
		nmap->entry[idx].key = NULL;
		return -ENOMEM;
	}

	return 0;
}