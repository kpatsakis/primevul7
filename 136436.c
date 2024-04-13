aspath_key_make (void *p)
{
  struct aspath *aspath = (struct aspath *) p;
  unsigned int key = 0;

  if (!aspath->str)
    aspath_str_update (aspath);

  key = jhash (aspath->str, aspath->str_len, 2334325);

  return key;
}