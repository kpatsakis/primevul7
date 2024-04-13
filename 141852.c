xfrm_state_sort(struct xfrm_state **dst, struct xfrm_state **src, int n,
		unsigned short family)
{
	int i;
	int err = 0;
	struct xfrm_state_afinfo *afinfo = xfrm_state_get_afinfo(family);
	struct net *net = xs_net(*src);

	if (!afinfo)
		return -EAFNOSUPPORT;

	spin_lock_bh(&net->xfrm.xfrm_state_lock);
	if (afinfo->state_sort)
		err = afinfo->state_sort(dst, src, n);
	else
		for (i = 0; i < n; i++)
			dst[i] = src[i];
	spin_unlock_bh(&net->xfrm.xfrm_state_lock);
	rcu_read_unlock();
	return err;
}