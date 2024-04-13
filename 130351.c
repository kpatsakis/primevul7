static __inline__ struct sock *icmpv6_xmit_lock(struct net *net)
{
	struct sock *sk;

	local_bh_disable();

	sk = icmpv6_sk(net);
	if (unlikely(!spin_trylock(&sk->sk_lock.slock))) {
		/* This can happen if the output path (f.e. SIT or
		 * ip6ip6 tunnel) signals dst_link_failure() for an
		 * outgoing ICMP6 packet.
		 */
		local_bh_enable();
		return NULL;
	}
	return sk;
}