assegment_free (struct assegment *seg)
{
  if (!seg)
    return;
  
  if (seg->as)
    assegment_data_free (seg->as);
  memset (seg, 0xfe, sizeof(struct assegment));
  XFREE (MTYPE_AS_SEG, seg);
  
  return;
}