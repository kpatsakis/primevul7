int xfrm_state_mtu(struct xfrm_state *x, int mtu)
{
	const struct xfrm_type *type = READ_ONCE(x->type);

	if (x->km.state == XFRM_STATE_VALID &&
	    type && type->get_mtu)
		return type->get_mtu(x, mtu);

	return mtu - x->props.header_len;
}