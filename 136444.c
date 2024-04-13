aspath_intern (struct aspath *aspath)
{
  struct aspath *find;

  /* Assert this AS path structure is not interned and has the string
     representation built. */
  assert (aspath->refcnt == 0);
  assert (aspath->str);

  /* Check AS path hash. */
  find = hash_get (ashash, aspath, hash_alloc_intern);
  if (find != aspath)
    aspath_free (aspath);

  find->refcnt++;

  return find;
}