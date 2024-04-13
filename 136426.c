aspath_add_confed_seq (struct aspath *aspath, as_t asno)
{
  return aspath_add_asns (aspath, asno, AS_CONFED_SEQUENCE, 1);
}