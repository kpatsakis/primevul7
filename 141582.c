static inline void xfrm_dev_state_free(struct xfrm_state *x)
{
	struct xfrm_state_offload *xso = &x->xso;
	struct net_device *dev = xso->dev;

	if (dev && dev->xfrmdev_ops) {
		if (dev->xfrmdev_ops->xdo_dev_state_free)
			dev->xfrmdev_ops->xdo_dev_state_free(x);
		xso->dev = NULL;
		dev_put(dev);
	}
}