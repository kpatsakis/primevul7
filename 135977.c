nsim_map_key_match(struct bpf_map *map, struct nsim_map_entry *e, void *key)
{
	return e->key && !memcmp(key, e->key, map->key_size);
}