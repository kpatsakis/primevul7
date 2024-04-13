static inline int xfrm_acquire_is_on(struct net *net)
{
	struct sock *nlsk;
	int ret = 0;

	rcu_read_lock();
	nlsk = rcu_dereference(net->xfrm.nlsk);
	if (nlsk)
		ret = netlink_has_listeners(nlsk, XFRMNLGRP_ACQUIRE);
	rcu_read_unlock();

	return ret;
}