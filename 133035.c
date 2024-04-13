ospf_recv_packet (int fd, struct interface **ifp, struct stream *ibuf)
{
  int ret;
  struct ip *iph;
  u_int16_t ip_len;
  unsigned int ifindex = 0;
  struct iovec iov;
  /* Header and data both require alignment. */
  char buff [CMSG_SPACE(SOPT_SIZE_CMSG_IFINDEX_IPV4())];
  struct msghdr msgh;

  memset (&msgh, 0, sizeof (struct msghdr));
  msgh.msg_iov = &iov;
  msgh.msg_iovlen = 1;
  msgh.msg_control = (caddr_t) buff;
  msgh.msg_controllen = sizeof (buff);
  
  ret = stream_recvmsg (ibuf, fd, &msgh, 0, OSPF_MAX_PACKET_SIZE+1);
  if (ret < 0)
    {
      zlog_warn("stream_recvmsg failed: %s", safe_strerror(errno));
      return NULL;
    }
  if ((unsigned int)ret < sizeof(iph)) /* ret must be > 0 now */
    {
      zlog_warn("ospf_recv_packet: discarding runt packet of length %d "
		"(ip header size is %u)",
		ret, (u_int)sizeof(iph));
      return NULL;
    }
  
  /* Note that there should not be alignment problems with this assignment
     because this is at the beginning of the stream data buffer. */
  iph = (struct ip *) STREAM_DATA(ibuf);
  sockopt_iphdrincl_swab_systoh (iph);
  
  ip_len = iph->ip_len;
  
#if !defined(GNU_LINUX) && (OpenBSD < 200311)
  /*
   * Kernel network code touches incoming IP header parameters,
   * before protocol specific processing.
   *
   *   1) Convert byteorder to host representation.
   *      --> ip_len, ip_id, ip_off
   *
   *   2) Adjust ip_len to strip IP header size!
   *      --> If user process receives entire IP packet via RAW
   *          socket, it must consider adding IP header size to
   *          the "ip_len" field of "ip" structure.
   *
   * For more details, see <netinet/ip_input.c>.
   */
  ip_len = ip_len + (iph->ip_hl << 2);
#endif
  
#if defined(__DragonFly__)
  /*
   * in DragonFly's raw socket, ip_len/ip_off are read 
   * in network byte order.
   * As OpenBSD < 200311 adjust ip_len to strip IP header size!
   */
  ip_len = ntohs(iph->ip_len) + (iph->ip_hl << 2);
#endif

  ifindex = getsockopt_ifindex (AF_INET, &msgh);
  
  *ifp = if_lookup_by_index (ifindex);

  if (ret != ip_len)
    {
      zlog_warn ("ospf_recv_packet read length mismatch: ip_len is %d, "
       		 "but recvmsg returned %d", ip_len, ret);
      return NULL;
    }
  
  return ibuf;
}