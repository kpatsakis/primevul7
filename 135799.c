static inline void skip_same_name(struct cache_entry *ce, struct unpack_trees_options *o)
{
	int len = ce_namelen(ce);
	const struct index_state *index = o->src_index;

	while (o->pos < index->cache_nr) {
		struct cache_entry *next = index->cache[o->pos];
		if (len != ce_namelen(next))
			break;
		if (memcmp(ce->name, next->name, len))
			break;
		o->pos++;
	}
}