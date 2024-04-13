static int nsim_map_key_find(struct bpf_offloaded_map *offmap, void *key)
{
	struct nsim_bpf_bound_map *nmap = offmap->dev_priv;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(nmap->entry); i++)
		if (nsim_map_key_match(&offmap->map, &nmap->entry[i], key))
			return i;

	return -ENOENT;
}