ospf_db_desc (struct ip *iph, struct ospf_header *ospfh,
	      struct stream *s, struct ospf_interface *oi, u_int16_t size)
{
  struct ospf_db_desc *dd;
  struct ospf_neighbor *nbr;

  /* Increment statistics. */
  oi->db_desc_in++;

  dd = (struct ospf_db_desc *) STREAM_PNT (s);

  nbr = ospf_nbr_lookup (oi, iph, ospfh);
  if (nbr == NULL)
    {
      zlog_warn ("Packet[DD]: Unknown Neighbor %s",
		 inet_ntoa (ospfh->router_id));
      return;
    }

  /* Check MTU. */
  if ((OSPF_IF_PARAM (oi, mtu_ignore) == 0) && 
      (ntohs (dd->mtu) > oi->ifp->mtu))
    {
      zlog_warn ("Packet[DD]: Neighbor %s MTU %u is larger than [%s]'s MTU %u",
		 inet_ntoa (nbr->router_id), ntohs (dd->mtu),
		 IF_NAME (oi), oi->ifp->mtu);
      return;
    }

  /* 
   * XXX HACK by Hasso Tepper. Setting N/P bit in NSSA area DD packets is not
   * required. In fact at least JunOS sends DD packets with P bit clear. 
   * Until proper solution is developped, this hack should help.
   *
   * Update: According to the RFCs, N bit is specified /only/ for Hello
   * options, unfortunately its use in DD options is not specified. Hence some
   * implementations follow E-bit semantics and set it in DD options, and some
   * treat it as unspecified and hence follow the directive "default for 
   * options is clear", ie unset.
   *
   * Reset the flag, as ospfd follows E-bit semantics.
   */
  if ( (oi->area->external_routing == OSPF_AREA_NSSA)
       && (CHECK_FLAG (nbr->options, OSPF_OPTION_NP))
       && (!CHECK_FLAG (dd->options, OSPF_OPTION_NP)) )
    {
      if (IS_DEBUG_OSPF_EVENT) 
        zlog_debug ("Packet[DD]: Neighbour %s: Has NSSA capability, sends with N bit clear in DD options",
                    inet_ntoa (nbr->router_id) );
      SET_FLAG (dd->options, OSPF_OPTION_NP);
    }

#ifdef REJECT_IF_TBIT_ON
  if (CHECK_FLAG (dd->options, OSPF_OPTION_T))
    {
      /*
       * In Hello protocol, optional capability must have checked
       * to prevent this T-bit enabled router be my neighbor.
       */
      zlog_warn ("Packet[DD]: Neighbor %s: T-bit on?", inet_ntoa (nbr->router_id));
      return;
    }
#endif /* REJECT_IF_TBIT_ON */

#ifdef HAVE_OPAQUE_LSA
  if (CHECK_FLAG (dd->options, OSPF_OPTION_O)
      && !CHECK_FLAG (oi->ospf->config, OSPF_OPAQUE_CAPABLE))
    {
      /*
       * This node is not configured to handle O-bit, for now.
       * Clear it to ignore unsupported capability proposed by neighbor.
       */
      UNSET_FLAG (dd->options, OSPF_OPTION_O);
    }
#endif /* HAVE_OPAQUE_LSA */

  /* Add event to thread. */
  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_PacketReceived);

  /* Process DD packet by neighbor status. */
  switch (nbr->state)
    {
    case NSM_Down:
    case NSM_Attempt:
    case NSM_TwoWay:
      zlog_warn ("Packet[DD]: Neighbor %s state is %s, packet discarded.",
		 inet_ntoa(nbr->router_id),
		 LOOKUP (ospf_nsm_state_msg, nbr->state));
      break;
    case NSM_Init:
      OSPF_NSM_EVENT_EXECUTE (nbr, NSM_TwoWayReceived);
      /* If the new state is ExStart, the processing of the current
	 packet should then continue in this new state by falling
	 through to case ExStart below.  */
      if (nbr->state != NSM_ExStart)
	break;
    case NSM_ExStart:
      /* Initial DBD */
      if ((IS_SET_DD_ALL (dd->flags) == OSPF_DD_FLAG_ALL) &&
	  (size == OSPF_DB_DESC_MIN_SIZE))
	{
	  if (IPV4_ADDR_CMP (&nbr->router_id, &oi->ospf->router_id) > 0)
	    {
	      /* We're Slave---obey */
	      zlog_info ("Packet[DD]: Neighbor %s Negotiation done (Slave).",
	      		 inet_ntoa(nbr->router_id));
	      nbr->dd_seqnum = ntohl (dd->dd_seqnum);
	      
	      /* Reset I/MS */
	      UNSET_FLAG (nbr->dd_flags, (OSPF_DD_FLAG_MS|OSPF_DD_FLAG_I));
	    }
	  else
	    {
	      /* We're Master, ignore the initial DBD from Slave */
	      zlog_info ("Packet[DD]: Neighbor %s: Initial DBD from Slave, "
	      		 "ignoring.", inet_ntoa(nbr->router_id));
	      break;
	    }
	}
      /* Ack from the Slave */
      else if (!IS_SET_DD_MS (dd->flags) && !IS_SET_DD_I (dd->flags) &&
	       ntohl (dd->dd_seqnum) == nbr->dd_seqnum &&
	       IPV4_ADDR_CMP (&nbr->router_id, &oi->ospf->router_id) < 0)
	{
	  zlog_info ("Packet[DD]: Neighbor %s Negotiation done (Master).",
		     inet_ntoa(nbr->router_id));
          /* Reset I, leaving MS */
          UNSET_FLAG (nbr->dd_flags, OSPF_DD_FLAG_I);
	}
      else
	{
	  zlog_warn ("Packet[DD]: Neighbor %s Negotiation fails.",
		     inet_ntoa(nbr->router_id));
	  break;
	}
      
      /* This is where the real Options are saved */
      nbr->options = dd->options;

#ifdef HAVE_OPAQUE_LSA
      if (CHECK_FLAG (oi->ospf->config, OSPF_OPAQUE_CAPABLE))
        {
          if (IS_DEBUG_OSPF_EVENT)
            zlog_debug ("Neighbor[%s] is %sOpaque-capable.",
		       inet_ntoa (nbr->router_id),
		       CHECK_FLAG (nbr->options, OSPF_OPTION_O) ? "" : "NOT ");

          if (! CHECK_FLAG (nbr->options, OSPF_OPTION_O)
          &&  IPV4_ADDR_SAME (&DR (oi), &nbr->address.u.prefix4))
            {
              zlog_warn ("DR-neighbor[%s] is NOT opaque-capable; "
                         "Opaque-LSAs cannot be reliably advertised "
                         "in this network.",
                         inet_ntoa (nbr->router_id));
              /* This situation is undesirable, but not a real error. */
            }
        }
#endif /* HAVE_OPAQUE_LSA */

      OSPF_NSM_EVENT_EXECUTE (nbr, NSM_NegotiationDone);

      /* continue processing rest of packet. */
      ospf_db_desc_proc (s, oi, nbr, dd, size);
      break;
    case NSM_Exchange:
      if (ospf_db_desc_is_dup (dd, nbr))
	{
	  if (IS_SET_DD_MS (nbr->dd_flags))
	    /* Master: discard duplicated DD packet. */
	    zlog_info ("Packet[DD] (Master): Neighbor %s packet duplicated.",
		       inet_ntoa (nbr->router_id));
	  else
	    /* Slave: cause to retransmit the last Database Description. */
	    {
	      zlog_info ("Packet[DD] [Slave]: Neighbor %s packet duplicated.",
			 inet_ntoa (nbr->router_id));
	      ospf_db_desc_resend (nbr);
	    }
	  break;
	}

      /* Otherwise DD packet should be checked. */
      /* Check Master/Slave bit mismatch */
      if (IS_SET_DD_MS (dd->flags) != IS_SET_DD_MS (nbr->last_recv.flags))
	{
	  zlog_warn ("Packet[DD]: Neighbor %s MS-bit mismatch.",
		     inet_ntoa(nbr->router_id));
	  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_SeqNumberMismatch);
	  if (IS_DEBUG_OSPF_EVENT)
	    zlog_debug ("Packet[DD]: dd->flags=%d, nbr->dd_flags=%d",
		        dd->flags, nbr->dd_flags);
	  break;
	}

      /* Check initialize bit is set. */
      if (IS_SET_DD_I (dd->flags))
	{
	  zlog_info ("Packet[DD]: Neighbor %s I-bit set.",
		     inet_ntoa(nbr->router_id));
	  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_SeqNumberMismatch);
	  break;
	}

      /* Check DD Options. */
      if (dd->options != nbr->options)
	{
#ifdef ORIGINAL_CODING
	  /* Save the new options for debugging */
	  nbr->options = dd->options;
#endif /* ORIGINAL_CODING */
	  zlog_warn ("Packet[DD]: Neighbor %s options mismatch.",
		     inet_ntoa(nbr->router_id));
	  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_SeqNumberMismatch);
	  break;
	}

      /* Check DD sequence number. */
      if ((IS_SET_DD_MS (nbr->dd_flags) &&
	   ntohl (dd->dd_seqnum) != nbr->dd_seqnum) ||
	  (!IS_SET_DD_MS (nbr->dd_flags) &&
	   ntohl (dd->dd_seqnum) != nbr->dd_seqnum + 1))
	{
	  zlog_warn ("Packet[DD]: Neighbor %s sequence number mismatch.",
		     inet_ntoa(nbr->router_id));
	  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_SeqNumberMismatch);
	  break;
	}

      /* Continue processing rest of packet. */
      ospf_db_desc_proc (s, oi, nbr, dd, size);
      break;
    case NSM_Loading:
    case NSM_Full:
      if (ospf_db_desc_is_dup (dd, nbr))
	{
	  if (IS_SET_DD_MS (nbr->dd_flags))
	    {
	      /* Master should discard duplicate DD packet. */
	      zlog_info ("Packet[DD]: Neighbor %s duplicated, "
	                 "packet discarded.",
			inet_ntoa(nbr->router_id));
	      break;
	    }
	  else
	    {
	      struct timeval t, now;
	      quagga_gettime (QUAGGA_CLK_MONOTONIC, &now);
	      t = tv_sub (now, nbr->last_send_ts);
	      if (tv_cmp (t, int2tv (nbr->v_inactivity)) < 0)
		{
		  /* In states Loading and Full the slave must resend
		     its last Database Description packet in response to
		     duplicate Database Description packets received
		     from the master.  For this reason the slave must
		     wait RouterDeadInterval seconds before freeing the
		     last Database Description packet.  Reception of a
		     Database Description packet from the master after
		     this interval will generate a SeqNumberMismatch
		     neighbor event. RFC2328 Section 10.8 */
		  ospf_db_desc_resend (nbr);
		  break;
		}
	    }
	}

      OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_SeqNumberMismatch);
      break;
    default:
      zlog_warn ("Packet[DD]: Neighbor %s NSM illegal status %u.",
		 inet_ntoa(nbr->router_id), nbr->state);
      break;
    }
}