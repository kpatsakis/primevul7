aspath_has_as4 (struct aspath *aspath)
{
  struct assegment *seg = aspath->segments;
  unsigned int i;
  
  while (seg)
    {
      for (i = 0; i < seg->length; i++)
        if (seg->as[i] > BGP_AS_MAX)
	  return 1;
      seg = seg->next;
    }
  return 0;
}