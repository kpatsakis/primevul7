aspath_init (void)
{
  ashash = hash_create_size (32768, aspath_key_make, aspath_cmp);
}