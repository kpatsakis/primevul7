xfrm_dev_state_flush_secctx_check(struct net *net, struct net_device *dev, bool task_valid)
{
	int i, err = 0;

	for (i = 0; i <= net->xfrm.state_hmask; i++) {
		struct xfrm_state *x;
		struct xfrm_state_offload *xso;

		hlist_for_each_entry(x, net->xfrm.state_bydst+i, bydst) {
			xso = &x->xso;

			if (xso->dev == dev &&
			   (err = security_xfrm_state_delete(x)) != 0) {
				xfrm_audit_state_delete(x, 0, task_valid);
				return err;
			}
		}
	}

	return err;
}