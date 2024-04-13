assegment_normalise (struct assegment *head)
{
  struct assegment *seg = head, *pin;
  struct assegment *tmp;
  
  if (!head)
    return head;
  
  while (seg)
    {
      pin = seg;
      
      /* Sort values SET segments, for determinism in paths to aid
       * creation of hash values / path comparisons
       * and because it helps other lesser implementations ;)
       */
      if (seg->type == AS_SET || seg->type == AS_CONFED_SET)
      	{
	  int tail = 0;
	  int i;
	  
	  qsort (seg->as, seg->length, sizeof(as_t), int_cmp);
	  
	  /* weed out dupes */
	  for (i=1; i < seg->length; i++)
	    {
	      if (seg->as[tail] == seg->as[i])
	      	continue;
	      
	      tail++;
	      if (tail < i)
	      	seg->as[tail] = seg->as[i];
	    }
	  /* seg->length can be 0.. */
	  if (seg->length)
	    seg->length = tail + 1;
	}

      /* read ahead from the current, pinned segment while the segments
       * are packable/mergeable. Append all following packable segments
       * to the segment we have pinned and remove these appended
       * segments.
       */      
      while (pin->next && ASSEGMENT_TYPES_PACKABLE(pin, pin->next))
        {
          tmp = pin->next;
          seg = pin->next;
          
          /* append the next sequence to the pinned sequence */
          pin = assegment_append_asns (pin, seg->as, seg->length);
          
          /* bypass the next sequence */
          pin->next = seg->next;
          
          /* get rid of the now referenceless segment */
          assegment_free (tmp);
          
        }

      seg = pin->next;
    }
  return head;
}