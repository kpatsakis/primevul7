ospf_ls_ack_send_delayed (struct ospf_interface *oi)
{
  struct in_addr dst;
  
  /* Decide destination address. */
  /* RFC2328 Section 13.5                           On non-broadcast
	networks, delayed Link State Acknowledgment packets must be
	unicast	separately over	each adjacency (i.e., neighbor whose
	state is >= Exchange).  */
  if (oi->type == OSPF_IFTYPE_NBMA)
    {
      struct ospf_neighbor *nbr;
      struct route_node *rn;

      for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
	if ((nbr = rn->info) != NULL)
	  if (nbr != oi->nbr_self && nbr->state >= NSM_Exchange)
	    while (listcount (oi->ls_ack))
	      ospf_ls_ack_send_list (oi, oi->ls_ack, nbr->address.u.prefix4);
      return;
    }
  if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
    dst.s_addr = oi->vl_data->peer_addr.s_addr;
  else if (oi->state == ISM_DR || oi->state == ISM_Backup)
    dst.s_addr = htonl (OSPF_ALLSPFROUTERS);
  else if (oi->type == OSPF_IFTYPE_POINTOPOINT)
    dst.s_addr = htonl (OSPF_ALLSPFROUTERS);
  else if (oi->type == OSPF_IFTYPE_POINTOMULTIPOINT)
    dst.s_addr = htonl (OSPF_ALLSPFROUTERS);
  else
    dst.s_addr = htonl (OSPF_ALLDROUTERS);

  while (listcount (oi->ls_ack))
    ospf_ls_ack_send_list (oi, oi->ls_ack, dst);
}