aspath_confed_check (struct aspath *aspath)
{
  struct assegment *seg;

  if ( !(aspath && aspath->segments) )
    return 0;

  seg = aspath->segments;

  while (seg)
    {
      if (seg->type == AS_CONFED_SET || seg->type == AS_CONFED_SEQUENCE)
	  return 1;
      seg = seg->next;
    }
  return 0;
}