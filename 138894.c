detached_get_digests(ns_detached_signatures_t *sigs, const char *flavor_name)
{
  digests_t *d = strmap_get(sigs->digests, flavor_name);
  if (!d) {
    d = tor_malloc_zero(sizeof(digests_t));
    strmap_set(sigs->digests, flavor_name, d);
  }
  return d;
}