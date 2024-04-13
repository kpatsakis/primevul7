ospf_make_db_desc (struct ospf_interface *oi, struct ospf_neighbor *nbr,
		   struct stream *s)
{
  struct ospf_lsa *lsa;
  u_int16_t length = OSPF_DB_DESC_MIN_SIZE;
  u_char options;
  unsigned long pp;
  int i;
  struct ospf_lsdb *lsdb;
  
  /* Set Interface MTU. */
  if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
    stream_putw (s, 0);
  else
    stream_putw (s, oi->ifp->mtu);

  /* Set Options. */
  options = OPTIONS (oi);
#ifdef HAVE_OPAQUE_LSA
  if (CHECK_FLAG (oi->ospf->config, OSPF_OPAQUE_CAPABLE))
    {
      if (IS_SET_DD_I (nbr->dd_flags)
      ||  CHECK_FLAG (nbr->options, OSPF_OPTION_O))
        /*
         * Set O-bit in the outgoing DD packet for capablity negotiation,
         * if one of following case is applicable. 
         *
         * 1) WaitTimer expiration event triggered the neighbor state to
         *    change to Exstart, but no (valid) DD packet has received
         *    from the neighbor yet.
         *
         * 2) At least one DD packet with O-bit on has received from the
         *    neighbor.
         */
        SET_FLAG (options, OSPF_OPTION_O);
    }
#endif /* HAVE_OPAQUE_LSA */
  stream_putc (s, options);

  /* DD flags */
  pp = stream_get_endp (s);
  stream_putc (s, nbr->dd_flags);

  /* Set DD Sequence Number. */
  stream_putl (s, nbr->dd_seqnum);

  /* shortcut unneeded walk of (empty) summary LSDBs */
  if (ospf_db_summary_isempty (nbr))
    goto empty;

  /* Describe LSA Header from Database Summary List. */
  lsdb = &nbr->db_sum;

  for (i = OSPF_MIN_LSA; i < OSPF_MAX_LSA; i++)
    {
      struct route_table *table = lsdb->type[i].db;
      struct route_node *rn;

      for (rn = route_top (table); rn; rn = route_next (rn))
	if ((lsa = rn->info) != NULL)
	  {
#ifdef HAVE_OPAQUE_LSA
            if (IS_OPAQUE_LSA (lsa->data->type)
            && (! CHECK_FLAG (options, OSPF_OPTION_O)))
              {
                /* Suppress advertising opaque-informations. */
                /* Remove LSA from DB summary list. */
                ospf_lsdb_delete (lsdb, lsa);
                continue;
              }
#endif /* HAVE_OPAQUE_LSA */

	    if (!CHECK_FLAG (lsa->flags, OSPF_LSA_DISCARD))
	      {
		struct lsa_header *lsah;
		u_int16_t ls_age;
		
		/* DD packet overflows interface MTU. */
		if (length + OSPF_LSA_HEADER_SIZE > ospf_packet_max (oi))
		  break;
		
		/* Keep pointer to LS age. */
		lsah = (struct lsa_header *) (STREAM_DATA (s) +
					      stream_get_endp (s));
		
		/* Proceed stream pointer. */
		stream_put (s, lsa->data, OSPF_LSA_HEADER_SIZE);
		length += OSPF_LSA_HEADER_SIZE;
		
		/* Set LS age. */
		ls_age = LS_AGE (lsa);
		lsah->ls_age = htons (ls_age);
		
	      }
	    
	    /* Remove LSA from DB summary list. */
	    ospf_lsdb_delete (lsdb, lsa);
	  }
    }

  /* Update 'More' bit */
  if (ospf_db_summary_isempty (nbr))
    {
empty:
      if (nbr->state >= NSM_Exchange)
        {
          UNSET_FLAG (nbr->dd_flags, OSPF_DD_FLAG_M);
          /* Rewrite DD flags */
          stream_putc_at (s, pp, nbr->dd_flags);
        }
      else
        {
          assert (IS_SET_DD_M(nbr->dd_flags));
        }
    }
  return length;
}