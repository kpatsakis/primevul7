ospf_ls_req (struct ip *iph, struct ospf_header *ospfh,
	     struct stream *s, struct ospf_interface *oi, u_int16_t size)
{
  struct ospf_neighbor *nbr;
  u_int32_t ls_type;
  struct in_addr ls_id;
  struct in_addr adv_router;
  struct ospf_lsa *find;
  struct list *ls_upd;
  unsigned int length;

  /* Increment statistics. */
  oi->ls_req_in++;

  nbr = ospf_nbr_lookup (oi, iph, ospfh);
  if (nbr == NULL)
    {
      zlog_warn ("Link State Request: Unknown Neighbor %s.",
		 inet_ntoa (ospfh->router_id));
      return;
    }

  /* Add event to thread. */
  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_PacketReceived);

  /* Neighbor State should be Exchange or later. */
  if (nbr->state != NSM_Exchange &&
      nbr->state != NSM_Loading &&
      nbr->state != NSM_Full)
    {
      zlog_warn ("Link State Request received from %s: "
      		 "Neighbor state is %s, packet discarded.",
		 inet_ntoa (ospfh->router_id),
		 LOOKUP (ospf_nsm_state_msg, nbr->state));
      return;
    }

  /* Send Link State Update for ALL requested LSAs. */
  ls_upd = list_new ();
  length = OSPF_HEADER_SIZE + OSPF_LS_UPD_MIN_SIZE;

  while (size >= OSPF_LSA_KEY_SIZE)
    {
      /* Get one slice of Link State Request. */
      ls_type = stream_getl (s);
      ls_id.s_addr = stream_get_ipv4 (s);
      adv_router.s_addr = stream_get_ipv4 (s);

      /* Verify LSA type. */
      if (ls_type < OSPF_MIN_LSA || ls_type >= OSPF_MAX_LSA)
	{
	  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_BadLSReq);
	  list_delete (ls_upd);
	  return;
	}

      /* Search proper LSA in LSDB. */
      find = ospf_lsa_lookup (oi->area, ls_type, ls_id, adv_router);
      if (find == NULL)
	{
	  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_BadLSReq);
	  list_delete (ls_upd);
	  return;
	}

      /* Packet overflows MTU size, send immediately. */
      if (length + ntohs (find->data->length) > ospf_packet_max (oi))
	{
	  if (oi->type == OSPF_IFTYPE_NBMA)
	    ospf_ls_upd_send (nbr, ls_upd, OSPF_SEND_PACKET_DIRECT);
	  else
	    ospf_ls_upd_send (nbr, ls_upd, OSPF_SEND_PACKET_INDIRECT);

	  /* Only remove list contents.  Keep ls_upd. */
	  list_delete_all_node (ls_upd);

	  length = OSPF_HEADER_SIZE + OSPF_LS_UPD_MIN_SIZE;
	}

      /* Append LSA to update list. */
      listnode_add (ls_upd, find);
      length += ntohs (find->data->length);

      size -= OSPF_LSA_KEY_SIZE;
    }

  /* Send rest of Link State Update. */
  if (listcount (ls_upd) > 0)
    {
      if (oi->type == OSPF_IFTYPE_NBMA)
	ospf_ls_upd_send (nbr, ls_upd, OSPF_SEND_PACKET_DIRECT);
      else
	ospf_ls_upd_send (nbr, ls_upd, OSPF_SEND_PACKET_INDIRECT);

      list_delete (ls_upd);
    }
  else
    list_free (ls_upd);
}