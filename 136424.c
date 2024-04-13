aspath_add_seq_n (struct aspath *aspath, as_t asno, unsigned num)
{
  return aspath_add_asns (aspath, asno, AS_SEQUENCE, num);
}