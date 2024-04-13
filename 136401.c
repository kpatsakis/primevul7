aspath_highest (struct aspath *aspath)
{
  struct assegment *seg = aspath->segments;
  as_t highest = 0;
  unsigned int i;
  
  while (seg)
    {
      for (i = 0; i < seg->length; i++)
        if (seg->as[i] > highest 
            && !BGP_AS_IS_PRIVATE(seg->as[i]))
	  highest = seg->as[i];
      seg = seg->next;
    }
  return highest;
}