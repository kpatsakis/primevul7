ospf_hello (struct ip *iph, struct ospf_header *ospfh,
	    struct stream * s, struct ospf_interface *oi, int size)
{
  struct ospf_hello *hello;
  struct ospf_neighbor *nbr;
  int old_state;
  struct prefix p;

  /* increment statistics. */
  oi->hello_in++;

  hello = (struct ospf_hello *) STREAM_PNT (s);

  /* If Hello is myself, silently discard. */
  if (IPV4_ADDR_SAME (&ospfh->router_id, &oi->ospf->router_id))
    {
      if (IS_DEBUG_OSPF_PACKET (ospfh->type - 1, RECV))
        {
          zlog_debug ("ospf_header[%s/%s]: selforiginated, "
                     "dropping.",
                     LOOKUP (ospf_packet_type_str, ospfh->type),
                     inet_ntoa (iph->ip_src));
        }
      return;
    }

  /* get neighbor prefix. */
  p.family = AF_INET;
  p.prefixlen = ip_masklen (hello->network_mask);
  p.u.prefix4 = iph->ip_src;

  /* Compare network mask. */
  /* Checking is ignored for Point-to-Point and Virtual link. */
  if (oi->type != OSPF_IFTYPE_POINTOPOINT 
      && oi->type != OSPF_IFTYPE_VIRTUALLINK)
    if (oi->address->prefixlen != p.prefixlen)
      {
	zlog_warn ("Packet %s [Hello:RECV]: NetworkMask mismatch on %s (configured prefix length is %d, but hello packet indicates %d).",
		   inet_ntoa(ospfh->router_id), IF_NAME(oi),
		   (int)oi->address->prefixlen, (int)p.prefixlen);
	return;
      }

  /* Compare Router Dead Interval. */
  if (OSPF_IF_PARAM (oi, v_wait) != ntohl (hello->dead_interval))
    {
      zlog_warn ("Packet %s [Hello:RECV]: RouterDeadInterval mismatch "
      		 "(expected %u, but received %u).",
		 inet_ntoa(ospfh->router_id),
		 OSPF_IF_PARAM(oi, v_wait), ntohl(hello->dead_interval));
      return;
    }

  /* Compare Hello Interval - ignored if fast-hellos are set. */
  if (OSPF_IF_PARAM (oi, fast_hello) == 0)
    {
      if (OSPF_IF_PARAM (oi, v_hello) != ntohs (hello->hello_interval))
        {
          zlog_warn ("Packet %s [Hello:RECV]: HelloInterval mismatch "
		     "(expected %u, but received %u).",
		     inet_ntoa(ospfh->router_id),
		     OSPF_IF_PARAM(oi, v_hello), ntohs(hello->hello_interval));
          return;
        }
    }
  
  if (IS_DEBUG_OSPF_EVENT)
    zlog_debug ("Packet %s [Hello:RECV]: Options %s",
	       inet_ntoa (ospfh->router_id),
	       ospf_options_dump (hello->options));

  /* Compare options. */
#define REJECT_IF_TBIT_ON	1 /* XXX */
#ifdef REJECT_IF_TBIT_ON
  if (CHECK_FLAG (hello->options, OSPF_OPTION_T))
    {
      /*
       * This router does not support non-zero TOS.
       * Drop this Hello packet not to establish neighbor relationship.
       */
      zlog_warn ("Packet %s [Hello:RECV]: T-bit on, drop it.",
		 inet_ntoa (ospfh->router_id));
      return;
    }
#endif /* REJECT_IF_TBIT_ON */

#ifdef HAVE_OPAQUE_LSA
  if (CHECK_FLAG (oi->ospf->config, OSPF_OPAQUE_CAPABLE)
      && CHECK_FLAG (hello->options, OSPF_OPTION_O))
    {
      /*
       * This router does know the correct usage of O-bit
       * the bit should be set in DD packet only.
       */
      zlog_warn ("Packet %s [Hello:RECV]: O-bit abuse?",
		 inet_ntoa (ospfh->router_id));
#ifdef STRICT_OBIT_USAGE_CHECK
      return;                                     /* Reject this packet. */
#else /* STRICT_OBIT_USAGE_CHECK */
      UNSET_FLAG (hello->options, OSPF_OPTION_O); /* Ignore O-bit. */
#endif /* STRICT_OBIT_USAGE_CHECK */
    }
#endif /* HAVE_OPAQUE_LSA */

  /* new for NSSA is to ensure that NP is on and E is off */

  if (oi->area->external_routing == OSPF_AREA_NSSA) 
    {
      if (! (CHECK_FLAG (OPTIONS (oi), OSPF_OPTION_NP)
	     && CHECK_FLAG (hello->options, OSPF_OPTION_NP)
	     && ! CHECK_FLAG (OPTIONS (oi), OSPF_OPTION_E)
	     && ! CHECK_FLAG (hello->options, OSPF_OPTION_E)))
	{
	  zlog_warn ("NSSA-Packet-%s[Hello:RECV]: my options: %x, his options %x", inet_ntoa (ospfh->router_id), OPTIONS (oi), hello->options);
	  return;
	}
      if (IS_DEBUG_OSPF_NSSA)
        zlog_debug ("NSSA-Hello:RECV:Packet from %s:", inet_ntoa(ospfh->router_id));
    }
  else    
    /* The setting of the E-bit found in the Hello Packet's Options
       field must match this area's ExternalRoutingCapability A
       mismatch causes processing to stop and the packet to be
       dropped. The setting of the rest of the bits in the Hello
       Packet's Options field should be ignored. */
    if (CHECK_FLAG (OPTIONS (oi), OSPF_OPTION_E) !=
	CHECK_FLAG (hello->options, OSPF_OPTION_E))
      {
	zlog_warn ("Packet %s [Hello:RECV]: my options: %x, his options %x",
		   inet_ntoa(ospfh->router_id), OPTIONS (oi), hello->options);
	return;
      }
  
  /* get neighbour struct */
  nbr = ospf_nbr_get (oi, ospfh, iph, &p);

  /* neighbour must be valid, ospf_nbr_get creates if none existed */
  assert (nbr);

  old_state = nbr->state;

  /* Add event to thread. */
  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_PacketReceived);

  /*  RFC2328  Section 9.5.1
      If the router is not eligible to become Designated Router,
      (snip)   It	must also send an Hello	Packet in reply	to an
      Hello Packet received from any eligible neighbor (other than
      the	current	Designated Router and Backup Designated	Router).  */
  if (oi->type == OSPF_IFTYPE_NBMA)
    if (PRIORITY(oi) == 0 && hello->priority > 0
	&& IPV4_ADDR_CMP(&DR(oi),  &iph->ip_src)
	&& IPV4_ADDR_CMP(&BDR(oi), &iph->ip_src))
      OSPF_NSM_TIMER_ON (nbr->t_hello_reply, ospf_hello_reply_timer,
			 OSPF_HELLO_REPLY_DELAY);

  /* on NBMA network type, it happens to receive bidirectional Hello packet
     without advance 1-Way Received event.
     To avoid incorrect DR-seletion, raise 1-Way Received event.*/
  if (oi->type == OSPF_IFTYPE_NBMA &&
      (old_state == NSM_Down || old_state == NSM_Attempt))
    {
      OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_OneWayReceived);
      nbr->priority = hello->priority;
      nbr->d_router = hello->d_router;
      nbr->bd_router = hello->bd_router;
      return;
    }

  if (ospf_nbr_bidirectional (&oi->ospf->router_id, hello->neighbors,
			      size - OSPF_HELLO_MIN_SIZE))
    {
      OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_TwoWayReceived);
      nbr->options |= hello->options;
    }
  else
    {
      OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_OneWayReceived);
      /* Set neighbor information. */
      nbr->priority = hello->priority;
      nbr->d_router = hello->d_router;
      nbr->bd_router = hello->bd_router;
      return;
    }

  /* If neighbor itself declares DR and no BDR exists,
     cause event BackupSeen */
  if (IPV4_ADDR_SAME (&nbr->address.u.prefix4, &hello->d_router))
    if (hello->bd_router.s_addr == 0 && oi->state == ISM_Waiting)
      OSPF_ISM_EVENT_SCHEDULE (oi, ISM_BackupSeen);

  /* neighbor itself declares BDR. */
  if (oi->state == ISM_Waiting &&
      IPV4_ADDR_SAME (&nbr->address.u.prefix4, &hello->bd_router))
    OSPF_ISM_EVENT_SCHEDULE (oi, ISM_BackupSeen);

  /* had not previously. */
  if ((IPV4_ADDR_SAME (&nbr->address.u.prefix4, &hello->d_router) &&
       IPV4_ADDR_CMP (&nbr->address.u.prefix4, &nbr->d_router)) ||
      (IPV4_ADDR_CMP (&nbr->address.u.prefix4, &hello->d_router) &&
       IPV4_ADDR_SAME (&nbr->address.u.prefix4, &nbr->d_router)))
    OSPF_ISM_EVENT_SCHEDULE (oi, ISM_NeighborChange);

  /* had not previously. */
  if ((IPV4_ADDR_SAME (&nbr->address.u.prefix4, &hello->bd_router) &&
       IPV4_ADDR_CMP (&nbr->address.u.prefix4, &nbr->bd_router)) ||
      (IPV4_ADDR_CMP (&nbr->address.u.prefix4, &hello->bd_router) &&
       IPV4_ADDR_SAME (&nbr->address.u.prefix4, &nbr->bd_router)))
    OSPF_ISM_EVENT_SCHEDULE (oi, ISM_NeighborChange);

  /* Neighbor priority check. */
  if (nbr->priority >= 0 && nbr->priority != hello->priority)
    OSPF_ISM_EVENT_SCHEDULE (oi, ISM_NeighborChange);

  /* Set neighbor information. */
  nbr->priority = hello->priority;
  nbr->d_router = hello->d_router;
  nbr->bd_router = hello->bd_router;
}