static int icmpv6_rcv(struct sk_buff *skb)
{
	struct net_device *dev = skb->dev;
	struct inet6_dev *idev = __in6_dev_get(dev);
	const struct in6_addr *saddr, *daddr;
	struct icmp6hdr *hdr;
	u8 type;
	bool success = false;

	if (!xfrm6_policy_check(NULL, XFRM_POLICY_IN, skb)) {
		struct sec_path *sp = skb_sec_path(skb);
		int nh;

		if (!(sp && sp->xvec[sp->len - 1]->props.flags &
				 XFRM_STATE_ICMP))
			goto drop_no_count;

		if (!pskb_may_pull(skb, sizeof(*hdr) + sizeof(struct ipv6hdr)))
			goto drop_no_count;

		nh = skb_network_offset(skb);
		skb_set_network_header(skb, sizeof(*hdr));

		if (!xfrm6_policy_check_reverse(NULL, XFRM_POLICY_IN, skb))
			goto drop_no_count;

		skb_set_network_header(skb, nh);
	}

	__ICMP6_INC_STATS(dev_net(dev), idev, ICMP6_MIB_INMSGS);

	saddr = &ipv6_hdr(skb)->saddr;
	daddr = &ipv6_hdr(skb)->daddr;

	if (skb_checksum_validate(skb, IPPROTO_ICMPV6, ip6_compute_pseudo)) {
		net_dbg_ratelimited("ICMPv6 checksum failed [%pI6c > %pI6c]\n",
				    saddr, daddr);
		goto csum_error;
	}

	if (!pskb_pull(skb, sizeof(*hdr)))
		goto discard_it;

	hdr = icmp6_hdr(skb);

	type = hdr->icmp6_type;

	ICMP6MSGIN_INC_STATS(dev_net(dev), idev, type);

	switch (type) {
	case ICMPV6_ECHO_REQUEST:
		icmpv6_echo_reply(skb);
		break;

	case ICMPV6_ECHO_REPLY:
		success = ping_rcv(skb);
		break;

	case ICMPV6_PKT_TOOBIG:
		/* BUGGG_FUTURE: if packet contains rthdr, we cannot update
		   standard destination cache. Seems, only "advanced"
		   destination cache will allow to solve this problem
		   --ANK (980726)
		 */
		if (!pskb_may_pull(skb, sizeof(struct ipv6hdr)))
			goto discard_it;
		hdr = icmp6_hdr(skb);

		/*
		 *	Drop through to notify
		 */

	case ICMPV6_DEST_UNREACH:
	case ICMPV6_TIME_EXCEED:
	case ICMPV6_PARAMPROB:
		icmpv6_notify(skb, type, hdr->icmp6_code, hdr->icmp6_mtu);
		break;

	case NDISC_ROUTER_SOLICITATION:
	case NDISC_ROUTER_ADVERTISEMENT:
	case NDISC_NEIGHBOUR_SOLICITATION:
	case NDISC_NEIGHBOUR_ADVERTISEMENT:
	case NDISC_REDIRECT:
		ndisc_rcv(skb);
		break;

	case ICMPV6_MGM_QUERY:
		igmp6_event_query(skb);
		break;

	case ICMPV6_MGM_REPORT:
		igmp6_event_report(skb);
		break;

	case ICMPV6_MGM_REDUCTION:
	case ICMPV6_NI_QUERY:
	case ICMPV6_NI_REPLY:
	case ICMPV6_MLD2_REPORT:
	case ICMPV6_DHAAD_REQUEST:
	case ICMPV6_DHAAD_REPLY:
	case ICMPV6_MOBILE_PREFIX_SOL:
	case ICMPV6_MOBILE_PREFIX_ADV:
		break;

	default:
		/* informational */
		if (type & ICMPV6_INFOMSG_MASK)
			break;

		net_dbg_ratelimited("icmpv6: msg of unknown type [%pI6c > %pI6c]\n",
				    saddr, daddr);

		/*
		 * error of unknown type.
		 * must pass to upper level
		 */

		icmpv6_notify(skb, type, hdr->icmp6_code, hdr->icmp6_mtu);
	}

	/* until the v6 path can be better sorted assume failure and
	 * preserve the status quo behaviour for the rest of the paths to here
	 */
	if (success)
		consume_skb(skb);
	else
		kfree_skb(skb);

	return 0;

csum_error:
	__ICMP6_INC_STATS(dev_net(dev), idev, ICMP6_MIB_CSUMERRORS);
discard_it:
	__ICMP6_INC_STATS(dev_net(dev), idev, ICMP6_MIB_INERRORS);
drop_no_count:
	kfree_skb(skb);
	return 0;
}