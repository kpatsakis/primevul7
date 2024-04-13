aspath_private_as_check (struct aspath *aspath)
{
  struct assegment *seg;
  
  if ( !(aspath && aspath->segments) )
    return 0;
    
  seg = aspath->segments;

  while (seg)
    {
      int i;
      
      for (i = 0; i < seg->length; i++)
	{
	  if (!BGP_AS_IS_PRIVATE(seg->as[i]))
	    return 0;
	}
      seg = seg->next;
    }
  return 1;
}