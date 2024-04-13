xfrm_init_tempstate(struct xfrm_state *x, const struct flowi *fl,
		    const struct xfrm_tmpl *tmpl,
		    const xfrm_address_t *daddr, const xfrm_address_t *saddr,
		    unsigned short family)
{
	struct xfrm_state_afinfo *afinfo = xfrm_state_afinfo_get_rcu(family);

	if (!afinfo)
		return;

	afinfo->init_tempsel(&x->sel, fl);

	if (family != tmpl->encap_family) {
		afinfo = xfrm_state_afinfo_get_rcu(tmpl->encap_family);
		if (!afinfo)
			return;
	}
	afinfo->init_temprop(x, tmpl, daddr, saddr);
}