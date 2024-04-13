aspath_cmp (const void *arg1, const void *arg2)
{
  const struct assegment *seg1 = ((const struct aspath *)arg1)->segments;
  const struct assegment *seg2 = ((const struct aspath *)arg2)->segments;
  
  while (seg1 || seg2)
    {
      int i;
      if ((!seg1 && seg2) || (seg1 && !seg2))
	return 0;
      if (seg1->type != seg2->type)
        return 0;      
      if (seg1->length != seg2->length)
        return 0;
      for (i = 0; i < seg1->length; i++)
        if (seg1->as[i] != seg2->as[i])
          return 0;
      seg1 = seg1->next;
      seg2 = seg2->next;
    }
  return 1;
}