ospf_ls_upd (struct ip *iph, struct ospf_header *ospfh,
	     struct stream *s, struct ospf_interface *oi, u_int16_t size)
{
  struct ospf_neighbor *nbr;
  struct list *lsas;
  struct listnode *node, *nnode;
  struct ospf_lsa *lsa = NULL;
  /* unsigned long ls_req_found = 0; */

  /* Dis-assemble the stream, update each entry, re-encapsulate for flooding */

  /* Increment statistics. */
  oi->ls_upd_in++;

  /* Check neighbor. */
  nbr = ospf_nbr_lookup (oi, iph, ospfh);
  if (nbr == NULL)
    {
      zlog_warn ("Link State Update: Unknown Neighbor %s on int: %s",
		 inet_ntoa (ospfh->router_id), IF_NAME (oi));
      return;
    }

  /* Add event to thread. */
  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_PacketReceived);

  /* Check neighbor state. */
  if (nbr->state < NSM_Exchange)
    {
      zlog_warn ("Link State Update: "
      		 "Neighbor[%s] state %s is less than Exchange",
		 inet_ntoa (ospfh->router_id),
		 LOOKUP(ospf_nsm_state_msg, nbr->state));
      return;
    }

  /* Get list of LSAs from Link State Update packet. - Also perorms Stages 
   * 1 (validate LSA checksum) and 2 (check for LSA consistent type) 
   * of section 13. 
   */
  lsas = ospf_ls_upd_list_lsa (nbr, s, oi, size);

#ifdef HAVE_OPAQUE_LSA
  /*
   * If self-originated Opaque-LSAs that have flooded before restart
   * are contained in the received LSUpd message, corresponding LSReq
   * messages to be sent may have to be modified.
   * To eliminate possible race conditions such that flushing and normal
   * updating for the same LSA would take place alternately, this trick
   * must be done before entering to the loop below.
   */
   /* XXX: Why is this Opaque specific? Either our core code is deficient
    * and this should be fixed generally, or Opaque is inventing strawman
    * problems */
   ospf_opaque_adjust_lsreq (nbr, lsas);
#endif /* HAVE_OPAQUE_LSA */

#define DISCARD_LSA(L,N) {\
        if (IS_DEBUG_OSPF_EVENT) \
          zlog_debug ("ospf_lsa_discard() in ospf_ls_upd() point %d: lsa %p Type-%d", N, lsa, (int) lsa->data->type); \
        ospf_lsa_discard (L); \
	continue; }

  /* Process each LSA received in the one packet. */
  for (ALL_LIST_ELEMENTS (lsas, node, nnode, lsa))
    {
      struct ospf_lsa *ls_ret, *current;
      int ret = 1;

      if (IS_DEBUG_OSPF_NSSA)
	{
	  char buf1[INET_ADDRSTRLEN];
	  char buf2[INET_ADDRSTRLEN];
	  char buf3[INET_ADDRSTRLEN];

	  zlog_debug("LSA Type-%d from %s, ID: %s, ADV: %s",
		  lsa->data->type,
		  inet_ntop (AF_INET, &ospfh->router_id,
			     buf1, INET_ADDRSTRLEN),
		  inet_ntop (AF_INET, &lsa->data->id,
			     buf2, INET_ADDRSTRLEN),
		  inet_ntop (AF_INET, &lsa->data->adv_router,
			     buf3, INET_ADDRSTRLEN));
	}

      listnode_delete (lsas, lsa); /* We don't need it in list anymore */

      /* Validate Checksum - Done above by ospf_ls_upd_list_lsa() */

      /* LSA Type  - Done above by ospf_ls_upd_list_lsa() */
   
      /* Do not take in AS External LSAs if we are a stub or NSSA. */

      /* Do not take in AS NSSA if this neighbor and we are not NSSA */

      /* Do take in Type-7's if we are an NSSA  */ 
 
      /* If we are also an ABR, later translate them to a Type-5 packet */
 
      /* Later, an NSSA Re-fresh can Re-fresh Type-7's and an ABR will
	 translate them to a separate Type-5 packet.  */

      if (lsa->data->type == OSPF_AS_EXTERNAL_LSA)
        /* Reject from STUB or NSSA */
        if (nbr->oi->area->external_routing != OSPF_AREA_DEFAULT) 
	  {
	    if (IS_DEBUG_OSPF_NSSA)
	      zlog_debug("Incoming External LSA Discarded: We are NSSA/STUB Area");
	    DISCARD_LSA (lsa, 1);
	  }

      if (lsa->data->type == OSPF_AS_NSSA_LSA)
	if (nbr->oi->area->external_routing != OSPF_AREA_NSSA)
	  {
	    if (IS_DEBUG_OSPF_NSSA)
	      zlog_debug("Incoming NSSA LSA Discarded:  Not NSSA Area");
	    DISCARD_LSA (lsa,2);
	  }

      /* Find the LSA in the current database. */

      current = ospf_lsa_lookup_by_header (oi->area, lsa->data);

      /* If the LSA's LS age is equal to MaxAge, and there is currently
	 no instance of the LSA in the router's link state database,
	 and none of router's neighbors are in states Exchange or Loading,
	 then take the following actions. */

      if (IS_LSA_MAXAGE (lsa) && !current &&
	  (ospf_nbr_count (oi, NSM_Exchange) +
	   ospf_nbr_count (oi, NSM_Loading)) == 0)
	{
	  /* Response Link State Acknowledgment. */
	  ospf_ls_ack_send (nbr, lsa);

	  /* Discard LSA. */	  
	  zlog_info ("Link State Update[%s]: LS age is equal to MaxAge.",
		     dump_lsa_key(lsa));
          DISCARD_LSA (lsa, 3);
	}

#ifdef HAVE_OPAQUE_LSA
      if (IS_OPAQUE_LSA (lsa->data->type)
      &&  IPV4_ADDR_SAME (&lsa->data->adv_router, &oi->ospf->router_id))
        {
          /*
           * Even if initial flushing seems to be completed, there might
           * be a case that self-originated LSA with MaxAge still remain
           * in the routing domain.
           * Just send an LSAck message to cease retransmission.
           */
          if (IS_LSA_MAXAGE (lsa))
            {
              zlog_warn ("LSA[%s]: Boomerang effect?", dump_lsa_key (lsa));
              ospf_ls_ack_send (nbr, lsa);
              ospf_lsa_discard (lsa);

              if (current != NULL && ! IS_LSA_MAXAGE (current))
                ospf_opaque_lsa_refresh_schedule (current);
              continue;
            }

          /*
           * If an instance of self-originated Opaque-LSA is not found
           * in the LSDB, there are some possible cases here.
           *
           * 1) This node lost opaque-capability after restart.
           * 2) Else, a part of opaque-type is no more supported.
           * 3) Else, a part of opaque-id is no more supported.
           *
           * Anyway, it is still this node's responsibility to flush it.
           * Otherwise, the LSA instance remains in the routing domain
           * until its age reaches to MaxAge.
           */
          /* XXX: We should deal with this for *ALL* LSAs, not just opaque */
          if (current == NULL)
            {
              if (IS_DEBUG_OSPF_EVENT)
                zlog_debug ("LSA[%s]: Previously originated Opaque-LSA,"
                            "not found in the LSDB.", dump_lsa_key (lsa));

              SET_FLAG (lsa->flags, OSPF_LSA_SELF);
              
              ospf_opaque_self_originated_lsa_received (nbr, lsa);
              ospf_ls_ack_send (nbr, lsa);
              
              continue;
            }
        }
#endif /* HAVE_OPAQUE_LSA */

      /* It might be happen that received LSA is self-originated network LSA, but
       * router ID is cahnged. So, we should check if LSA is a network-LSA whose
       * Link State ID is one of the router's own IP interface addresses but whose
       * Advertising Router is not equal to the router's own Router ID
       * According to RFC 2328 12.4.2 and 13.4 this LSA should be flushed.
       */

      if(lsa->data->type == OSPF_NETWORK_LSA)
      {
        struct listnode *oinode, *oinnode;
        struct ospf_interface *out_if;
        int Flag = 0;

        for (ALL_LIST_ELEMENTS (oi->ospf->oiflist, oinode, oinnode, out_if))
        {
          if(out_if == NULL)
            break;

          if((IPV4_ADDR_SAME(&out_if->address->u.prefix4, &lsa->data->id)) &&
              (!(IPV4_ADDR_SAME(&oi->ospf->router_id, &lsa->data->adv_router))))
          {
            if(out_if->network_lsa_self)
            {
              ospf_lsa_flush_area(lsa,out_if->area);
              if(IS_DEBUG_OSPF_EVENT)
                zlog_debug ("ospf_lsa_discard() in ospf_ls_upd() point 9: lsa %p Type-%d",
                            lsa, (int) lsa->data->type);
              ospf_lsa_discard (lsa);
              Flag = 1;
            }
            break;
          }
        }
        if(Flag)
          continue;
      }

      /* (5) Find the instance of this LSA that is currently contained
	 in the router's link state database.  If there is no
	 database copy, or the received LSA is more recent than
	 the database copy the following steps must be performed. */

      if (current == NULL ||
	  (ret = ospf_lsa_more_recent (current, lsa)) < 0)
	{
	  /* Actual flooding procedure. */
	  if (ospf_flood (oi->ospf, nbr, current, lsa) < 0)  /* Trap NSSA later. */
	    DISCARD_LSA (lsa, 4);
	  continue;
	}

      /* (6) Else, If there is an instance of the LSA on the sending
	 neighbor's Link state request list, an error has occurred in
	 the Database Exchange process.  In this case, restart the
	 Database Exchange process by generating the neighbor event
	 BadLSReq for the sending neighbor and stop processing the
	 Link State Update packet. */

      if (ospf_ls_request_lookup (nbr, lsa))
	{
	  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_BadLSReq);
	  zlog_warn("LSA[%s] instance exists on Link state request list",
	  	    dump_lsa_key(lsa));

	  /* Clean list of LSAs. */
          ospf_upd_list_clean (lsas);
	  /* this lsa is not on lsas list already. */
	  ospf_lsa_discard (lsa);
	  return;
	}

      /* If the received LSA is the same instance as the database copy
	 (i.e., neither one is more recent) the following two steps
	 should be performed: */

      if (ret == 0)
	{
	  /* If the LSA is listed in the Link state retransmission list
	     for the receiving adjacency, the router itself is expecting
	     an acknowledgment for this LSA.  The router should treat the
	     received LSA as an acknowledgment by removing the LSA from
	     the Link state retransmission list.  This is termed an
	     "implied acknowledgment". */

	  ls_ret = ospf_ls_retransmit_lookup (nbr, lsa);

	  if (ls_ret != NULL)
	    {
	      ospf_ls_retransmit_delete (nbr, ls_ret);

	      /* Delayed acknowledgment sent if advertisement received
		 from Designated Router, otherwise do nothing. */
	      if (oi->state == ISM_Backup)
		if (NBR_IS_DR (nbr))
		  listnode_add (oi->ls_ack, ospf_lsa_lock (lsa));

              DISCARD_LSA (lsa, 5);
	    }
	  else
	    /* Acknowledge the receipt of the LSA by sending a
	       Link State Acknowledgment packet back out the receiving
	       interface. */
	    {
	      ospf_ls_ack_send (nbr, lsa);
	      DISCARD_LSA (lsa, 6);
	    }
	}

      /* The database copy is more recent.  If the database copy
	 has LS age equal to MaxAge and LS sequence number equal to
	 MaxSequenceNumber, simply discard the received LSA without
	 acknowledging it. (In this case, the LSA's LS sequence number is
	 wrapping, and the MaxSequenceNumber LSA must be completely
	 flushed before any new LSA instance can be introduced). */

      else if (ret > 0)  /* Database copy is more recent */
	{ 
	  if (IS_LSA_MAXAGE (current) &&
	      current->data->ls_seqnum == htonl (OSPF_MAX_SEQUENCE_NUMBER))
	    {
	      DISCARD_LSA (lsa, 7);
	    }
	  /* Otherwise, as long as the database copy has not been sent in a
	     Link State Update within the last MinLSArrival seconds, send the
	     database copy back to the sending neighbor, encapsulated within
	     a Link State Update Packet. The Link State Update Packet should
	     be sent directly to the neighbor. In so doing, do not put the
	     database copy of the LSA on the neighbor's link state
	     retransmission list, and do not acknowledge the received (less
	     recent) LSA instance. */
	  else
	    {
	      struct timeval now;
	      
	      quagga_gettime (QUAGGA_CLK_MONOTONIC, &now);
	      
	      if (tv_cmp (tv_sub (now, current->tv_orig), 
			  int2tv (OSPF_MIN_LS_ARRIVAL)) >= 0)
		/* Trap NSSA type later.*/
		ospf_ls_upd_send_lsa (nbr, current, OSPF_SEND_PACKET_DIRECT);
	      DISCARD_LSA (lsa, 8);
	    }
	}
    }
#undef DISCARD_LSA

  assert (listcount (lsas) == 0);
  list_delete (lsas);
}