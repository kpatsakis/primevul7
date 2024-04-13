struct xfrm_state_afinfo *xfrm_state_afinfo_get_rcu(unsigned int family)
{
	if (unlikely(family >= NPROTO))
		return NULL;

	return rcu_dereference(xfrm_state_afinfo[family]);
}