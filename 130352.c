static struct dst_entry *icmpv6_route_lookup(struct net *net,
					     struct sk_buff *skb,
					     struct sock *sk,
					     struct flowi6 *fl6)
{
	struct dst_entry *dst, *dst2;
	struct flowi6 fl2;
	int err;

	err = ip6_dst_lookup(net, sk, &dst, fl6);
	if (err)
		return ERR_PTR(err);

	/*
	 * We won't send icmp if the destination is known
	 * anycast.
	 */
	if (ipv6_anycast_destination(dst, &fl6->daddr)) {
		net_dbg_ratelimited("icmp6_send: acast source\n");
		dst_release(dst);
		return ERR_PTR(-EINVAL);
	}

	/* No need to clone since we're just using its address. */
	dst2 = dst;

	dst = xfrm_lookup(net, dst, flowi6_to_flowi(fl6), sk, 0);
	if (!IS_ERR(dst)) {
		if (dst != dst2)
			return dst;
	} else {
		if (PTR_ERR(dst) == -EPERM)
			dst = NULL;
		else
			return dst;
	}

	err = xfrm_decode_session_reverse(skb, flowi6_to_flowi(&fl2), AF_INET6);
	if (err)
		goto relookup_failed;

	err = ip6_dst_lookup(net, sk, &dst2, &fl2);
	if (err)
		goto relookup_failed;

	dst2 = xfrm_lookup(net, dst2, flowi6_to_flowi(&fl2), sk, XFRM_LOOKUP_ICMP);
	if (!IS_ERR(dst2)) {
		dst_release(dst);
		dst = dst2;
	} else {
		err = PTR_ERR(dst2);
		if (err == -EPERM) {
			dst_release(dst);
			return dst2;
		} else
			goto relookup_failed;
	}

relookup_failed:
	if (dst)
		return dst;
	return ERR_PTR(err);
}