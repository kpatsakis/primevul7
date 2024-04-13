aspath_add_asns (struct aspath *aspath, as_t asno, u_char type, unsigned num)
{
  struct assegment *assegment = aspath->segments;
  unsigned i;

  if (assegment && assegment->type == type)
    {
      /* extend existing segment */
      aspath->segments = assegment_prepend_asns (aspath->segments, asno, num);
    }
  else 
    {
      /* prepend with new segment */
      struct assegment *newsegment = assegment_new (type, num);
      for (i = 0; i < num; i++)
	newsegment->as[i] = asno;

      /* insert potentially replacing empty segment */
      if (assegment && assegment->length == 0)
	{
	  newsegment->next = assegment->next;
	  assegment_free (assegment);
	}
       else
	  newsegment->next = assegment;
      aspath->segments = newsegment;
    }

  aspath_str_update (aspath);
  return aspath;
}