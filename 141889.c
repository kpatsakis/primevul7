int xfrm_init_state(struct xfrm_state *x)
{
	int err;

	err = __xfrm_init_state(x, true, false);
	if (!err)
		x->km.state = XFRM_STATE_VALID;

	return err;
}