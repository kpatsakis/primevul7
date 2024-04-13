assegment_append_asns (struct assegment *seg, as_t *asnos, int num)
{
  as_t *newas;
  
  newas = XREALLOC (MTYPE_AS_SEG_DATA, seg->as,
		      ASSEGMENT_DATA_SIZE (seg->length + num, 1));

  if (newas)
    {
      seg->as = newas;
      memcpy (seg->as + seg->length, asnos, ASSEGMENT_DATA_SIZE(num, 1));
      seg->length += num;
      return seg;
    }

  assegment_free_all (seg);
  return NULL;
}