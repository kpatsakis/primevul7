ospf_hello_send (struct ospf_interface *oi)
{
  /* If this is passive interface, do not send OSPF Hello. */
  if (OSPF_IF_PASSIVE_STATUS (oi) == OSPF_IF_PASSIVE)
    return;

  if (oi->type == OSPF_IFTYPE_NBMA)
    {
      struct ospf_neighbor *nbr;
      struct route_node *rn;

      for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
	if ((nbr = rn->info))
	  if (nbr != oi->nbr_self)
	    if (nbr->state != NSM_Down)
	      {
		/*  RFC 2328  Section 9.5.1
		    If the router is not eligible to become Designated Router,
		    it must periodically send Hello Packets to both the
		    Designated Router and the Backup Designated Router (if they
		    exist).  */
		if (PRIORITY(oi) == 0 &&
		    IPV4_ADDR_CMP(&DR(oi),  &nbr->address.u.prefix4) &&
		    IPV4_ADDR_CMP(&BDR(oi), &nbr->address.u.prefix4))
		  continue;

		/*  If the router is eligible to become Designated Router, it
		    must periodically send Hello Packets to all neighbors that
		    are also eligible. In addition, if the router is itself the
		    Designated Router or Backup Designated Router, it must also
		    send periodic Hello Packets to all other neighbors. */

		if (nbr->priority == 0 && oi->state == ISM_DROther)
		  continue;
		/* if oi->state == Waiting, send hello to all neighbors */
		ospf_hello_send_sub (oi, nbr->address.u.prefix4.s_addr);
	      }
    }
  else
    {
      /* Decide destination address. */
      if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
        ospf_hello_send_sub (oi, oi->vl_data->peer_addr.s_addr);
      else
        ospf_hello_send_sub (oi, htonl (OSPF_ALLSPFROUTERS));
    }
}