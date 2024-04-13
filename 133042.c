ospf_associate_packet_vl (struct ospf *ospf, struct interface *ifp, 
			  struct ip *iph, struct ospf_header *ospfh)
{
  struct ospf_interface *rcv_oi;
  struct ospf_vl_data *vl_data;
  struct ospf_area *vl_area;
  struct listnode *node;

  if (IN_MULTICAST (ntohl (iph->ip_dst.s_addr)) ||
      !OSPF_IS_AREA_BACKBONE (ospfh))
    return NULL;

  /* look for local OSPF interface matching the destination
   * to determine Area ID. We presume therefore the destination address
   * is unique, or at least (for "unnumbered" links), not used in other 
   * areas
   */
  if ((rcv_oi = ospf_if_lookup_by_local_addr (ospf, NULL, 
                                              iph->ip_dst)) == NULL)
    return NULL;

  for (ALL_LIST_ELEMENTS_RO (ospf->vlinks, node, vl_data))
    {
      vl_area = ospf_area_lookup_by_area_id (ospf, vl_data->vl_area_id);
      if (!vl_area)
	continue;
      
      if (OSPF_AREA_SAME (&vl_area, &rcv_oi->area) &&
	  IPV4_ADDR_SAME (&vl_data->vl_peer, &ospfh->router_id))
	{
	  if (IS_DEBUG_OSPF_EVENT)
	    zlog_debug ("associating packet with %s",
		       IF_NAME (vl_data->vl_oi));
	  if (! CHECK_FLAG (vl_data->vl_oi->ifp->flags, IFF_UP))
	    {
	      if (IS_DEBUG_OSPF_EVENT)
		zlog_debug ("This VL is not up yet, sorry");
	      return NULL;
	    }
	  
	  return vl_data->vl_oi;
	}
    }

  if (IS_DEBUG_OSPF_EVENT)
    zlog_debug ("couldn't find any VL to associate the packet with");
  
  return NULL;
}