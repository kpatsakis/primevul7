ospf_ls_upd_send (struct ospf_neighbor *nbr, struct list *update, int flag)
{
  struct ospf_interface *oi;
  struct ospf_lsa *lsa;
  struct prefix_ipv4 p;
  struct route_node *rn;
  struct listnode *node;
  
  oi = nbr->oi;

  p.family = AF_INET;
  p.prefixlen = IPV4_MAX_BITLEN;
  
  /* Decide destination address. */
  if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
    p.prefix = oi->vl_data->peer_addr;
  else if (oi->type == OSPF_IFTYPE_POINTOPOINT) 
     p.prefix.s_addr = htonl (OSPF_ALLSPFROUTERS);
  else if (flag == OSPF_SEND_PACKET_DIRECT)
     p.prefix = nbr->address.u.prefix4;
  else if (oi->state == ISM_DR || oi->state == ISM_Backup)
     p.prefix.s_addr = htonl (OSPF_ALLSPFROUTERS);
  else if (oi->type == OSPF_IFTYPE_POINTOMULTIPOINT)
     p.prefix.s_addr = htonl (OSPF_ALLSPFROUTERS);
  else
     p.prefix.s_addr = htonl (OSPF_ALLDROUTERS);

  if (oi->type == OSPF_IFTYPE_NBMA)
    {
      if (flag == OSPF_SEND_PACKET_INDIRECT)
	zlog_warn ("* LS-Update is directly sent on NBMA network.");
      if (IPV4_ADDR_SAME(&oi->address->u.prefix4, &p.prefix.s_addr))
	zlog_warn ("* LS-Update is sent to myself.");
    }

  rn = route_node_get (oi->ls_upd_queue, (struct prefix *) &p);

  if (rn->info == NULL)
    rn->info = list_new ();

  for (ALL_LIST_ELEMENTS_RO (update, node, lsa))
    listnode_add (rn->info, ospf_lsa_lock (lsa)); /* oi->ls_upd_queue */

  if (oi->t_ls_upd_event == NULL)
    oi->t_ls_upd_event =
      thread_add_event (master, ospf_ls_upd_send_queue_event, oi, 0);
}