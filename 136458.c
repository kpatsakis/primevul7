aspath_add_seq (struct aspath *aspath, as_t asno)
{
  return aspath_add_asns (aspath, asno, AS_SEQUENCE, 1);
}