grow_assoc_cache(void)
{
	static size_t	prior_sz;
	size_t		new_sz;

	new_sz = prior_sz + 4 * 1024;
	if (0 == prior_sz) {
		new_sz -= 4 * sizeof(void *);
	}
	assoc_cache = erealloc_zero(assoc_cache, new_sz, prior_sz); 
	prior_sz = new_sz;
	assoc_cache_slots = new_sz / sizeof(assoc_cache[0]);
}