xfrm_state_lookup(struct net *net, u32 mark, const xfrm_address_t *daddr, __be32 spi,
		  u8 proto, unsigned short family)
{
	struct xfrm_state *x;

	rcu_read_lock();
	x = __xfrm_state_lookup(net, mark, daddr, spi, proto, family);
	rcu_read_unlock();
	return x;
}