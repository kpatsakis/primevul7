assegments_parse (struct stream *s, size_t length, 
                  struct assegment **result, int use32bit)
{
  struct assegment_header segh;
  struct assegment *seg, *prev = NULL, *head = NULL;
  size_t bytes = 0;
  
  /* empty aspath (ie iBGP or somesuch) */
  if (length == 0)
    return 0;
  
  if (BGP_DEBUG (as4, AS4_SEGMENT))
    zlog_debug ("[AS4SEG] Parse aspath segment: got total byte length %lu",
		(unsigned long) length);
  /* basic checks */
  if ((STREAM_READABLE(s) < length)
      || (STREAM_READABLE(s) < AS_HEADER_SIZE) 
      || (length % AS16_VALUE_SIZE ))
    return -1;
  
  while (bytes < length)
    {
      int i;
      size_t seg_size;
      
      if ((length - bytes) <= AS_HEADER_SIZE)
        {
          if (head)
            assegment_free_all (head);
          return -1;
        }
      
      /* softly softly, get the header first on its own */
      segh.type = stream_getc (s);
      segh.length = stream_getc (s);
      
      seg_size = ASSEGMENT_SIZE(segh.length, use32bit);

      if (BGP_DEBUG (as4, AS4_SEGMENT))
	zlog_debug ("[AS4SEG] Parse aspath segment: got type %d, length %d",
                    segh.type, segh.length);
      
      /* check it.. */
      if ( ((bytes + seg_size) > length)
          /* 1771bis 4.3b: seg length contains one or more */
          || (segh.length == 0) 
          /* Paranoia in case someone changes type of segment length.
           * Shift both values by 0x10 to make the comparison operate
           * on more, than 8 bits (otherwise it's a warning, bug #564).
           */
          || ((sizeof segh.length > 1) 
              && (0x10 + segh.length > 0x10 + AS_SEGMENT_MAX)))
        {
          if (head)
            assegment_free_all (head);
          return -1;
        }
      
      switch (segh.type)
        {
          case AS_SEQUENCE:
          case AS_SET:
          case AS_CONFED_SEQUENCE:
          case AS_CONFED_SET:
            break;
          default:
            if (head)
              assegment_free_all (head);
            return -1;
        }
      
      /* now its safe to trust lengths */
      seg = assegment_new (segh.type, segh.length);
      
      if (head)
        prev->next = seg;
      else /* it's the first segment */
        head = prev = seg;
      
      for (i = 0; i < segh.length; i++)
	seg->as[i] = (use32bit) ? stream_getl (s) : stream_getw (s);

      bytes += seg_size;
      
      if (BGP_DEBUG (as4, AS4_SEGMENT))
	zlog_debug ("[AS4SEG] Parse aspath segment: Bytes now: %lu",
	            (unsigned long) bytes);
      
      prev = seg;
    }
 
  *result = assegment_normalise (head);
  return 0;
}