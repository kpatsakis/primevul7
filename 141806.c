static struct xfrm_state *__xfrm_state_lookup_byaddr(struct net *net, u32 mark,
						     const xfrm_address_t *daddr,
						     const xfrm_address_t *saddr,
						     u8 proto, unsigned short family)
{
	unsigned int h = xfrm_src_hash(net, daddr, saddr, family);
	struct xfrm_state *x;

	hlist_for_each_entry_rcu(x, net->xfrm.state_bysrc + h, bysrc) {
		if (x->props.family != family ||
		    x->id.proto     != proto ||
		    !xfrm_addr_equal(&x->id.daddr, daddr, family) ||
		    !xfrm_addr_equal(&x->props.saddr, saddr, family))
			continue;

		if ((mark & x->mark.m) != x->mark.v)
			continue;
		if (!xfrm_state_hold_rcu(x))
			continue;
		return x;
	}

	return NULL;
}