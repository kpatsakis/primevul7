ospf_write (struct thread *thread)
{
  struct ospf *ospf = THREAD_ARG (thread);
  struct ospf_interface *oi;
  struct ospf_packet *op;
  struct sockaddr_in sa_dst;
  struct ip iph;
  struct msghdr msg;
  struct iovec iov[2];
  u_char type;
  int ret;
  int flags = 0;
  struct listnode *node;
#ifdef WANT_OSPF_WRITE_FRAGMENT
  static u_int16_t ipid = 0;
#endif /* WANT_OSPF_WRITE_FRAGMENT */
  u_int16_t maxdatasize;
#define OSPF_WRITE_IPHL_SHIFT 2
  
  ospf->t_write = NULL;

  node = listhead (ospf->oi_write_q);
  assert (node);
  oi = listgetdata (node);
  assert (oi);

#ifdef WANT_OSPF_WRITE_FRAGMENT
  /* seed ipid static with low order bits of time */
  if (ipid == 0)
    ipid = (time(NULL) & 0xffff);
#endif /* WANT_OSPF_WRITE_FRAGMENT */

  /* convenience - max OSPF data per packet,
   * and reliability - not more data, than our
   * socket can accept
   */
  maxdatasize = MIN (oi->ifp->mtu, ospf->maxsndbuflen) -
    sizeof (struct ip);
  
  /* Get one packet from queue. */
  op = ospf_fifo_head (oi->obuf);
  assert (op);
  assert (op->length >= OSPF_HEADER_SIZE);

  if (op->dst.s_addr == htonl (OSPF_ALLSPFROUTERS)
      || op->dst.s_addr == htonl (OSPF_ALLDROUTERS))
      ospf_if_ipmulticast (ospf, oi->address, oi->ifp->ifindex);
    
  /* Rewrite the md5 signature & update the seq */
  ospf_make_md5_digest (oi, op);

  /* Retrieve OSPF packet type. */
  stream_set_getp (op->s, 1);
  type = stream_getc (op->s);
  
  /* reset get pointer */
  stream_set_getp (op->s, 0);

  memset (&iph, 0, sizeof (struct ip));
  memset (&sa_dst, 0, sizeof (sa_dst));
  
  sa_dst.sin_family = AF_INET;
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
  sa_dst.sin_len = sizeof(sa_dst);
#endif /* HAVE_STRUCT_SOCKADDR_IN_SIN_LEN */
  sa_dst.sin_addr = op->dst;
  sa_dst.sin_port = htons (0);

  /* Set DONTROUTE flag if dst is unicast. */
  if (oi->type != OSPF_IFTYPE_VIRTUALLINK)
    if (!IN_MULTICAST (htonl (op->dst.s_addr)))
      flags = MSG_DONTROUTE;

  iph.ip_hl = sizeof (struct ip) >> OSPF_WRITE_IPHL_SHIFT;
  /* it'd be very strange for header to not be 4byte-word aligned but.. */
  if ( sizeof (struct ip) 
        > (unsigned int)(iph.ip_hl << OSPF_WRITE_IPHL_SHIFT) )
    iph.ip_hl++; /* we presume sizeof struct ip cant overflow ip_hl.. */
  
  iph.ip_v = IPVERSION;
  iph.ip_tos = IPTOS_PREC_INTERNETCONTROL;
  iph.ip_len = (iph.ip_hl << OSPF_WRITE_IPHL_SHIFT) + op->length;

#if defined(__DragonFly__)
  /*
   * DragonFly's raw socket expects ip_len/ip_off in network byte order.
   */
  iph.ip_len = htons(iph.ip_len);
#endif

#ifdef WANT_OSPF_WRITE_FRAGMENT
  /* XXX-MT: not thread-safe at all..
   * XXX: this presumes this is only programme sending OSPF packets 
   * otherwise, no guarantee ipid will be unique
   */
  iph.ip_id = ++ipid;
#endif /* WANT_OSPF_WRITE_FRAGMENT */

  iph.ip_off = 0;
  if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
    iph.ip_ttl = OSPF_VL_IP_TTL;
  else
    iph.ip_ttl = OSPF_IP_TTL;
  iph.ip_p = IPPROTO_OSPFIGP;
  iph.ip_sum = 0;
  iph.ip_src.s_addr = oi->address->u.prefix4.s_addr;
  iph.ip_dst.s_addr = op->dst.s_addr;

  memset (&msg, 0, sizeof (msg));
  msg.msg_name = (caddr_t) &sa_dst;
  msg.msg_namelen = sizeof (sa_dst); 
  msg.msg_iov = iov;
  msg.msg_iovlen = 2;
  iov[0].iov_base = (char*)&iph;
  iov[0].iov_len = iph.ip_hl << OSPF_WRITE_IPHL_SHIFT;
  iov[1].iov_base = STREAM_PNT (op->s);
  iov[1].iov_len = op->length;
  
  /* Sadly we can not rely on kernels to fragment packets because of either
   * IP_HDRINCL and/or multicast destination being set.
   */
#ifdef WANT_OSPF_WRITE_FRAGMENT
  if ( op->length > maxdatasize )
    ospf_write_frags (ospf->fd, op, &iph, &msg, maxdatasize, 
                      oi->ifp->mtu, flags, type);
#endif /* WANT_OSPF_WRITE_FRAGMENT */

  /* send final fragment (could be first) */
  sockopt_iphdrincl_swab_htosys (&iph);
  ret = sendmsg (ospf->fd, &msg, flags);
  sockopt_iphdrincl_swab_systoh (&iph);
  
  if (ret < 0)
    zlog_warn ("*** sendmsg in ospf_write failed to %s, "
	       "id %d, off %d, len %d, interface %s, mtu %u: %s",
	       inet_ntoa (iph.ip_dst), iph.ip_id, iph.ip_off, iph.ip_len,
	       oi->ifp->name, oi->ifp->mtu, safe_strerror (errno));

  /* Show debug sending packet. */
  if (IS_DEBUG_OSPF_PACKET (type - 1, SEND))
    {
      if (IS_DEBUG_OSPF_PACKET (type - 1, DETAIL))
	{
	  zlog_debug ("-----------------------------------------------------");
	  ospf_ip_header_dump (&iph);
	  stream_set_getp (op->s, 0);
	  ospf_packet_dump (op->s);
	}

      zlog_debug ("%s sent to [%s] via [%s].",
		 LOOKUP (ospf_packet_type_str, type), inet_ntoa (op->dst),
		 IF_NAME (oi));

      if (IS_DEBUG_OSPF_PACKET (type - 1, DETAIL))
	zlog_debug ("-----------------------------------------------------");
    }

  /* Now delete packet from queue. */
  ospf_packet_delete (oi);

  if (ospf_fifo_head (oi->obuf) == NULL)
    {
      oi->on_write_q = 0;
      list_delete_node (ospf->oi_write_q, node);
    }
  
  /* If packets still remain in queue, call write thread. */
  if (!list_isempty (ospf->oi_write_q))
    ospf->t_write =                                              
      thread_add_write (master, ospf_write, ospf, ospf->fd);

  return 0;
}