aspath_hash_alloc (void *arg)
{
  const struct aspath *aspath = arg;
  struct aspath *new;

  /* Malformed AS path value. */
  assert (aspath->str);
  if (! aspath->str)
    return NULL;

  /* New aspath structure is needed. */
  new = XMALLOC (MTYPE_AS_PATH, sizeof (struct aspath));

  /* Reuse segments and string representation */
  new->refcnt = 0;
  new->segments = aspath->segments;
  new->str = aspath->str;
  new->str_len = aspath->str_len;

  return new;
}