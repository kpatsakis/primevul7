aspath_aggregate_as_set_add (struct aspath *aspath, struct assegment *asset,
			     as_t as)
{
  int i;

  /* If this is first AS set member, create new as-set segment. */
  if (asset == NULL)
    {
      asset = assegment_new (AS_SET, 1);
      if (! aspath->segments)
	aspath->segments = asset;
      else
        {
          struct assegment *seg = aspath->segments;
          while (seg->next)
            seg = seg->next;
          seg->next = asset;
        }
      asset->type = AS_SET;
      asset->length = 1;
      asset->as[0] = as;
    }
  else
    {
      /* Check this AS value already exists or not. */
      for (i = 0; i < asset->length; i++)
	if (asset->as[i] == as)
	  return asset;
      
      asset->length++;
      asset->as = XREALLOC (MTYPE_AS_SEG_DATA, asset->as, 
                            asset->length * AS_VALUE_SIZE);
      asset->as[asset->length - 1] = as;
    }
  

  return asset;
}