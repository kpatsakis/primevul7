aspath_as_add (struct aspath *as, as_t asno)
{
  struct assegment *seg = as->segments;

  if (!seg)
    return;
  
  /* Last segment search procedure. */
  while (seg->next)
    seg = seg->next;

  assegment_append_asns (seg, &asno, 1);
}