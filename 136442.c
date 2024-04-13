aspath_parse (struct stream *s, size_t length, int use32bit)
{
  struct aspath as;
  struct aspath *find;

  /* If length is odd it's malformed AS path. */
  /* Nit-picking: if (use32bit == 0) it is malformed if odd,
   * otherwise its malformed when length is larger than 2 and (length-2) 
   * is not dividable by 4.
   * But... this time we're lazy
   */
  if (length % AS16_VALUE_SIZE )
    return NULL;

  memset (&as, 0, sizeof (struct aspath));
  if (assegments_parse (s, length, &as.segments, use32bit) < 0)
    return NULL;

  /* If already same aspath exist then return it. */
  find = hash_get (ashash, &as, aspath_hash_alloc);

  /* bug! should not happen, let the daemon crash below */
  assert (find);

  /* if the aspath was already hashed free temporary memory. */
  if (find->refcnt)
    {
      assegment_free_all (as.segments);
      /* aspath_key_make() always updates the string */
      XFREE (MTYPE_AS_STR, as.str);
    }

  find->refcnt++;

  return find;
}