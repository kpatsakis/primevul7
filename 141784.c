xfrm_tmpl_sort(struct xfrm_tmpl **dst, struct xfrm_tmpl **src, int n,
	       unsigned short family, struct net *net)
{
	int i;
	int err = 0;
	struct xfrm_state_afinfo *afinfo = xfrm_state_get_afinfo(family);
	if (!afinfo)
		return -EAFNOSUPPORT;

	spin_lock_bh(&net->xfrm.xfrm_state_lock); /*FIXME*/
	if (afinfo->tmpl_sort)
		err = afinfo->tmpl_sort(dst, src, n);
	else
		for (i = 0; i < n; i++)
			dst[i] = src[i];
	spin_unlock_bh(&net->xfrm.xfrm_state_lock);
	rcu_read_unlock();
	return err;
}