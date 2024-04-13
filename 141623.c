int xfrm_user_policy(struct sock *sk, int optname, u8 __user *optval, int optlen)
{
	int err;
	u8 *data;
	struct xfrm_mgr *km;
	struct xfrm_policy *pol = NULL;

	if (in_compat_syscall())
		return -EOPNOTSUPP;

	if (!optval && !optlen) {
		xfrm_sk_policy_insert(sk, XFRM_POLICY_IN, NULL);
		xfrm_sk_policy_insert(sk, XFRM_POLICY_OUT, NULL);
		__sk_dst_reset(sk);
		return 0;
	}

	if (optlen <= 0 || optlen > PAGE_SIZE)
		return -EMSGSIZE;

	data = memdup_user(optval, optlen);
	if (IS_ERR(data))
		return PTR_ERR(data);

	err = -EINVAL;
	rcu_read_lock();
	list_for_each_entry_rcu(km, &xfrm_km_list, list) {
		pol = km->compile_policy(sk, optname, data,
					 optlen, &err);
		if (err >= 0)
			break;
	}
	rcu_read_unlock();

	if (err >= 0) {
		xfrm_sk_policy_insert(sk, err, pol);
		xfrm_pol_put(pol);
		__sk_dst_reset(sk);
		err = 0;
	}

	kfree(data);
	return err;
}