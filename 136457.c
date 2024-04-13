assegment_prepend_asns (struct assegment *seg, as_t asnum, int num)
{
  as_t *newas;
  int i;
  
  if (!num)
    return seg;
  
  if (num >= AS_SEGMENT_MAX)
    return seg; /* we don't do huge prepends */
  
  if ((newas = assegment_data_new (seg->length + num)) == NULL)
    return seg;
  
  for (i = 0; i < num; i++)
    newas[i] = asnum;

  memcpy (newas + num, seg->as, ASSEGMENT_DATA_SIZE (seg->length, 1));
  assegment_data_free (seg->as);
  seg->as = newas;
  seg->length += num;

  return seg;
}