ospf_make_hello (struct ospf_interface *oi, struct stream *s)
{
  struct ospf_neighbor *nbr;
  struct route_node *rn;
  u_int16_t length = OSPF_HELLO_MIN_SIZE;
  struct in_addr mask;
  unsigned long p;
  int flag = 0;

  /* Set netmask of interface. */
  if (oi->type != OSPF_IFTYPE_POINTOPOINT &&
      oi->type != OSPF_IFTYPE_VIRTUALLINK)
    masklen2ip (oi->address->prefixlen, &mask);
  else
    memset ((char *) &mask, 0, sizeof (struct in_addr));
  stream_put_ipv4 (s, mask.s_addr);

  /* Set Hello Interval. */
  if (OSPF_IF_PARAM (oi, fast_hello) == 0)
    stream_putw (s, OSPF_IF_PARAM (oi, v_hello));
  else
    stream_putw (s, 0); /* hello-interval of 0 for fast-hellos */

  if (IS_DEBUG_OSPF_EVENT)
    zlog_debug ("make_hello: options: %x, int: %s",
	       OPTIONS(oi), IF_NAME (oi));

  /* Set Options. */
  stream_putc (s, OPTIONS (oi));

  /* Set Router Priority. */
  stream_putc (s, PRIORITY (oi));

  /* Set Router Dead Interval. */
  stream_putl (s, OSPF_IF_PARAM (oi, v_wait));

  /* Set Designated Router. */
  stream_put_ipv4 (s, DR (oi).s_addr);

  p = stream_get_endp (s);

  /* Set Backup Designated Router. */
  stream_put_ipv4 (s, BDR (oi).s_addr);

  /* Add neighbor seen. */
  for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
    if ((nbr = rn->info))
      if (nbr->router_id.s_addr != 0)	/* Ignore 0.0.0.0 node. */
	if (nbr->state != NSM_Attempt)  /* Ignore Down neighbor. */
	if (nbr->state != NSM_Down)     /* This is myself for DR election. */
	  if (!IPV4_ADDR_SAME (&nbr->router_id, &oi->ospf->router_id))
	    {
	      /* Check neighbor is sane? */
	      if (nbr->d_router.s_addr != 0
		  && IPV4_ADDR_SAME (&nbr->d_router, &oi->address->u.prefix4)
		  && IPV4_ADDR_SAME (&nbr->bd_router, &oi->address->u.prefix4))
		flag = 1;

	      stream_put_ipv4 (s, nbr->router_id.s_addr);
	      length += 4;
	    }

  /* Let neighbor generate BackupSeen. */
  if (flag == 1)
    stream_putl_at (s, p, 0); /* ipv4 address, normally */

  return length;
}