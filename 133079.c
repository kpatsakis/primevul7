ospf_ls_ack (struct ip *iph, struct ospf_header *ospfh,
	     struct stream *s, struct ospf_interface *oi, u_int16_t size)
{
  struct ospf_neighbor *nbr;
  
  /* increment statistics. */
  oi->ls_ack_in++;

  nbr = ospf_nbr_lookup (oi, iph, ospfh);
  if (nbr == NULL)
    {
      zlog_warn ("Link State Acknowledgment: Unknown Neighbor %s.",
		 inet_ntoa (ospfh->router_id));
      return;
    }

  /* Add event to thread. */
  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_PacketReceived);

  if (nbr->state < NSM_Exchange)
    {
      zlog_warn ("Link State Acknowledgment: "
      		 "Neighbor[%s] state %s is less than Exchange",
		 inet_ntoa (ospfh->router_id),
		 LOOKUP(ospf_nsm_state_msg, nbr->state));
      return;
    }
  
  while (size >= OSPF_LSA_HEADER_SIZE)
    {
      struct ospf_lsa *lsa, *lsr;

      lsa = ospf_lsa_new ();
      lsa->data = (struct lsa_header *) STREAM_PNT (s);

      /* lsah = (struct lsa_header *) STREAM_PNT (s); */
      size -= OSPF_LSA_HEADER_SIZE;
      stream_forward_getp (s, OSPF_LSA_HEADER_SIZE);

      if (lsa->data->type < OSPF_MIN_LSA || lsa->data->type >= OSPF_MAX_LSA)
	{
	  lsa->data = NULL;
	  ospf_lsa_discard (lsa);
	  continue;
	}

      lsr = ospf_ls_retransmit_lookup (nbr, lsa);

      if (lsr != NULL && lsr->data->ls_seqnum == lsa->data->ls_seqnum)
        {
#ifdef HAVE_OPAQUE_LSA
          if (IS_OPAQUE_LSA (lsr->data->type))
            ospf_opaque_ls_ack_received (nbr, lsr);
#endif /* HAVE_OPAQUE_LSA */

          ospf_ls_retransmit_delete (nbr, lsr);
        }

      lsa->data = NULL;
      ospf_lsa_discard (lsa);
    }

  return;
}