aspath_str_update (struct aspath *as)
{
  if (as->str)
    XFREE (MTYPE_AS_STR, as->str);
  aspath_make_str_count (as);
}