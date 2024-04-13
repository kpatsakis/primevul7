aspath_prepend (struct aspath *as1, struct aspath *as2)
{
  struct assegment *seg1;
  struct assegment *seg2;

  if (! as1 || ! as2)
    return NULL;
  
  seg1 = as1->segments;
  seg2 = as2->segments;
  
  /* If as2 is empty, only need to dupe as1's chain onto as2 */
  if (seg2 == NULL)
    {
      as2->segments = assegment_dup_all (as1->segments);
      aspath_str_update (as2);
      return as2;
    }
  
  /* If as1 is empty AS, no prepending to do. */
  if (seg1 == NULL)
    return as2;
  
  /* find the tail as1's segment chain. */
  while (seg1 && seg1->next)
    seg1 = seg1->next;

  /* Delete any AS_CONFED_SEQUENCE segment from as2. */
  if (seg1->type == AS_SEQUENCE && seg2->type == AS_CONFED_SEQUENCE)
    as2 = aspath_delete_confed_seq (as2);
  
  /* as2 may have been updated */
  seg2 = as2->segments;
  
  /* as2 may be empty now due to aspath_delete_confed_seq, recheck */
  if (seg2 == NULL)
    {
      as2->segments = assegment_dup_all (as1->segments);
      aspath_str_update (as2);
      return as2;
    }
  
  /* Compare last segment type of as1 and first segment type of as2. */
  if (seg1->type != seg2->type)
    return aspath_merge (as1, as2);

  if (seg1->type == AS_SEQUENCE)
    {
      /* We have two chains of segments, as1->segments and seg2, 
       * and we have to attach them together, merging the attaching
       * segments together into one.
       * 
       * 1. dupe as1->segments onto head of as2
       * 2. merge seg2's asns onto last segment of this new chain
       * 3. attach chain after seg2
       */
      
      /* dupe as1 onto as2's head */
      seg1 = as2->segments = assegment_dup_all (as1->segments);
      
      /* refind the tail of as2, reusing seg1 */
      while (seg1 && seg1->next)
        seg1 = seg1->next;
      
      /* merge the old head, seg2, into tail, seg1 */
      seg1 = assegment_append_asns (seg1, seg2->as, seg2->length);
      
      /* bypass the merged seg2, and attach any chain after it to
       * chain descending from as2's head
       */
      seg1->next = seg2->next;
      
      /* seg2 is now referenceless and useless*/
      assegment_free (seg2);
      
      /* we've now prepended as1's segment chain to as2, merging
       * the inbetween AS_SEQUENCE of seg2 in the process 
       */
      aspath_str_update (as2);
      return as2;
    }
  else
    {
      /* AS_SET merge code is needed at here. */
      return aspath_merge (as1, as2);
    }
  /* XXX: Ermmm, what if as1 has multiple segments?? */
  
  /* Not reached */
}