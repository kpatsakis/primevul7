aspath_reconcile_as4 ( struct aspath *aspath, struct aspath *as4path)
{
  struct assegment *seg, *newseg, *prevseg = NULL;
  struct aspath *newpath = NULL, *mergedpath;
  int hops, cpasns = 0;
  
  if (!aspath)
    return NULL;
  
  seg = aspath->segments;
  
  /* CONFEDs should get reconciled too.. */
  hops = (aspath_count_hops (aspath) + aspath_count_confeds (aspath))
         - aspath_count_hops (as4path);
  
  if (hops < 0)
    {
      if (BGP_DEBUG (as4, AS4))
        zlog_warn ("[AS4] Fewer hops in AS_PATH than NEW_AS_PATH");
      /* Something's gone wrong. The RFC says we should now ignore AS4_PATH,
       * which is daft behaviour - it contains vital loop-detection
       * information which must have been removed from AS_PATH.
       */
       hops = aspath_count_hops (aspath);
    }
  
  if (!hops)
   return aspath_dup (as4path);
  
  if ( BGP_DEBUG(as4, AS4))
    zlog_debug("[AS4] got AS_PATH %s and AS4_PATH %s synthesizing now",
               aspath->str, as4path->str);

  while (seg && hops > 0)
    {
      switch (seg->type)
        {
          case AS_SET:
          case AS_CONFED_SET:
            hops--;
            cpasns = seg->length;
            break;
          case AS_CONFED_SEQUENCE:
	    /* Should never split a confed-sequence, if hop-count
	     * suggests we must then something's gone wrong somewhere.
	     *
	     * Most important goal is to preserve AS_PATHs prime function
	     * as loop-detector, so we fudge the numbers so that the entire
	     * confed-sequence is merged in.
	     */
	    if (hops < seg->length)
	      {
	        if (BGP_DEBUG (as4, AS4))
	          zlog_debug ("[AS4] AS4PATHmangle: AS_CONFED_SEQUENCE falls"
	                      " across 2/4 ASN boundary somewhere, broken..");
	        hops = seg->length;
	      }
	  case AS_SEQUENCE:
	    cpasns = MIN(seg->length, hops);
	    hops -= seg->length;
	}
      
      assert (cpasns <= seg->length);
      
      newseg = assegment_new (seg->type, 0);
      newseg = assegment_append_asns (newseg, seg->as, cpasns);

      if (!newpath)
        {
          newpath = aspath_new ();
          newpath->segments = newseg;
        }
      else
        prevseg->next = newseg;

      prevseg = newseg;
      seg = seg->next;
    }
    
  /* We may be able to join some segments here, and we must
   * do this because... we want normalised aspaths in out hash
   * and we do not want to stumble in aspath_put.
   */
  mergedpath = aspath_merge (newpath, aspath_dup(as4path));
  aspath_free (newpath);
  mergedpath->segments = assegment_normalise (mergedpath->segments);
  aspath_str_update (mergedpath);
  
  if ( BGP_DEBUG(as4, AS4))
    zlog_debug ("[AS4] result of synthesizing is %s",
                mergedpath->str);
  
  return mergedpath;
}