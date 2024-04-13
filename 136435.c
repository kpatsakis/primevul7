aspath_segment_add (struct aspath *as, int type)
{
  struct assegment *seg = as->segments;
  struct assegment *new = assegment_new (type, 0);

  if (seg)
    {
      while (seg->next)
	seg = seg->next;
      seg->next = new;
    }
  else
    as->segments = new;
}