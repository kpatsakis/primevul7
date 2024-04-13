process(struct Interface *ifacel, unsigned char *msg, int len,
	struct sockaddr_in6 *addr, struct in6_pktinfo *pkt_info, int hoplimit)
{
	struct Interface *iface;
	struct icmp6_hdr *icmph;
	char addr_str[INET6_ADDRSTRLEN];

	print_addr(&addr->sin6_addr, addr_str);

	if ( ! pkt_info )
	{
		flog(LOG_WARNING, "received packet with no pkt_info from %s!", addr_str );
		return;
	}

	/*
	 * can this happen?
	 */

	if (len < sizeof(struct icmp6_hdr))
	{
		flog(LOG_WARNING, "received icmpv6 packet with invalid length (%d) from %s",
			len, addr_str);
		return;
	}

	icmph = (struct icmp6_hdr *) msg;

	if (icmph->icmp6_type != ND_ROUTER_SOLICIT &&
	    icmph->icmp6_type != ND_ROUTER_ADVERT)
	{
		/*
		 *	We just want to listen to RSs and RAs
		 */

		flog(LOG_ERR, "icmpv6 filter failed");
		return;
	}

	if (icmph->icmp6_type == ND_ROUTER_ADVERT)
	{
		if (len < sizeof(struct nd_router_advert)) {
			flog(LOG_WARNING, "received icmpv6 RA packet with invalid length (%d) from %s",
				len, addr_str);
			return;
		}

		if (!IN6_IS_ADDR_LINKLOCAL(&addr->sin6_addr)) {
			flog(LOG_WARNING, "received icmpv6 RA packet with non-linklocal source address from %s", addr_str);
			return;
		}
	}

	if (icmph->icmp6_type == ND_ROUTER_SOLICIT)
	{
		if (len < sizeof(struct nd_router_solicit)) {
			flog(LOG_WARNING, "received icmpv6 RS packet with invalid length (%d) from %s",
				len, addr_str);
			return;
		}
	}

	if (icmph->icmp6_code != 0)
	{
		flog(LOG_WARNING, "received icmpv6 RS/RA packet with invalid code (%d) from %s",
			icmph->icmp6_code, addr_str);
		return;
	}

	dlog(LOG_DEBUG, 4, "if_index %u", pkt_info->ipi6_ifindex);

	/* get iface by received if_index */

	for (iface = ifacel; iface; iface=iface->next)
	{
		if (iface->if_index == pkt_info->ipi6_ifindex)
		{
			break;
		}
	}

	if (iface == NULL)
	{
		dlog(LOG_DEBUG, 2, "received packet from unknown interface: %d",
			pkt_info->ipi6_ifindex);
		return;
	}

	if (hoplimit != 255)
	{
		print_addr(&addr->sin6_addr, addr_str);
		flog(LOG_WARNING, "received RS or RA with invalid hoplimit %d from %s",
			hoplimit, addr_str);
		return;
	}

	if (!iface->AdvSendAdvert)
	{
		dlog(LOG_DEBUG, 2, "AdvSendAdvert is off for %s", iface->Name);
		return;
	}

	dlog(LOG_DEBUG, 4, "found Interface: %s", iface->Name);

	if (icmph->icmp6_type == ND_ROUTER_SOLICIT)
	{
		dlog(LOG_DEBUG, 2, "received RS from %s", addr_str);
		process_rs(iface, msg, len, addr);
	}
	else if (icmph->icmp6_type == ND_ROUTER_ADVERT)
	{
		dlog(LOG_DEBUG, 2, "received RA from %s", addr_str);
		process_ra(iface, msg, len, addr);
	}
}