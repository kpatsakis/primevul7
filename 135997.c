static void nsim_bpf_map_free(struct bpf_offloaded_map *offmap)
{
	struct nsim_bpf_bound_map *nmap = offmap->dev_priv;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(nmap->entry); i++) {
		kfree(nmap->entry[i].key);
		kfree(nmap->entry[i].value);
	}
	list_del_init(&nmap->l);
	mutex_destroy(&nmap->mutex);
	kfree(nmap);
}