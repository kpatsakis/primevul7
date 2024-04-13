aspath_dup (struct aspath *aspath)
{
  unsigned short buflen = aspath->str_len + 1;
  struct aspath *new;

  new = XCALLOC (MTYPE_AS_PATH, sizeof (struct aspath));

  if (aspath->segments)
    new->segments = assegment_dup_all (aspath->segments);

  if (!aspath->str)
    return new;

  new->str = XMALLOC (MTYPE_AS_STR, buflen);
  new->str_len = aspath->str_len;

  /* copy the string data */
  if (aspath->str_len > 0)
    memcpy (new->str, aspath->str, buflen);
  else
    new->str[0] = '\0';

  return new;
}