bool sk_filter_charge(struct sock *sk, struct sk_filter *fp)
{
	if (!refcount_inc_not_zero(&fp->refcnt))
		return false;

	if (!__sk_filter_charge(sk, fp)) {
		sk_filter_release(fp);
		return false;
	}
	return true;
}