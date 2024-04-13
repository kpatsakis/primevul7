static void icmpv6_echo_reply(struct sk_buff *skb)
{
	struct net *net = dev_net(skb->dev);
	struct sock *sk;
	struct inet6_dev *idev;
	struct ipv6_pinfo *np;
	const struct in6_addr *saddr = NULL;
	struct icmp6hdr *icmph = icmp6_hdr(skb);
	struct icmp6hdr tmp_hdr;
	struct flowi6 fl6;
	struct icmpv6_msg msg;
	struct dst_entry *dst;
	struct ipcm6_cookie ipc6;
	int err = 0;
	u32 mark = IP6_REPLY_MARK(net, skb->mark);
	struct sockcm_cookie sockc_unused = {0};

	saddr = &ipv6_hdr(skb)->daddr;

	if (!ipv6_unicast_destination(skb) &&
	    !(net->ipv6.sysctl.anycast_src_echo_reply &&
	      ipv6_anycast_destination(skb_dst(skb), saddr)))
		saddr = NULL;

	memcpy(&tmp_hdr, icmph, sizeof(tmp_hdr));
	tmp_hdr.icmp6_type = ICMPV6_ECHO_REPLY;

	memset(&fl6, 0, sizeof(fl6));
	fl6.flowi6_proto = IPPROTO_ICMPV6;
	fl6.daddr = ipv6_hdr(skb)->saddr;
	if (saddr)
		fl6.saddr = *saddr;
	fl6.flowi6_oif = skb->dev->ifindex;
	fl6.fl6_icmp_type = ICMPV6_ECHO_REPLY;
	fl6.flowi6_mark = mark;
	security_skb_classify_flow(skb, flowi6_to_flowi(&fl6));

	sk = icmpv6_xmit_lock(net);
	if (!sk)
		return;
	sk->sk_mark = mark;
	np = inet6_sk(sk);

	if (!fl6.flowi6_oif && ipv6_addr_is_multicast(&fl6.daddr))
		fl6.flowi6_oif = np->mcast_oif;
	else if (!fl6.flowi6_oif)
		fl6.flowi6_oif = np->ucast_oif;

	err = ip6_dst_lookup(net, sk, &dst, &fl6);
	if (err)
		goto out;
	dst = xfrm_lookup(net, dst, flowi6_to_flowi(&fl6), sk, 0);
	if (IS_ERR(dst))
		goto out;

	idev = __in6_dev_get(skb->dev);

	msg.skb = skb;
	msg.offset = 0;
	msg.type = ICMPV6_ECHO_REPLY;

	ipc6.hlimit = ip6_sk_dst_hoplimit(np, &fl6, dst);
	ipc6.tclass = ipv6_get_dsfield(ipv6_hdr(skb));
	ipc6.dontfrag = np->dontfrag;
	ipc6.opt = NULL;

	err = ip6_append_data(sk, icmpv6_getfrag, &msg, skb->len + sizeof(struct icmp6hdr),
				sizeof(struct icmp6hdr), &ipc6, &fl6,
				(struct rt6_info *)dst, MSG_DONTWAIT,
				&sockc_unused);

	if (err) {
		__ICMP6_INC_STATS(net, idev, ICMP6_MIB_OUTERRORS);
		ip6_flush_pending_frames(sk);
	} else {
		err = icmpv6_push_pending_frames(sk, &fl6, &tmp_hdr,
						 skb->len + sizeof(struct icmp6hdr));
	}
	dst_release(dst);
out:
	icmpv6_xmit_unlock(sk);
}