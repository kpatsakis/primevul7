static inline void xfrm_dev_state_delete(struct xfrm_state *x)
{
	struct xfrm_state_offload *xso = &x->xso;

	if (xso->dev)
		xso->dev->xfrmdev_ops->xdo_dev_state_delete(x);
}