static inline void xfrm_sk_free_policy(struct sock *sk)
{
	struct xfrm_policy *pol;

	pol = rcu_dereference_protected(sk->sk_policy[0], 1);
	if (unlikely(pol != NULL)) {
		xfrm_policy_delete(pol, XFRM_POLICY_MAX);
		sk->sk_policy[0] = NULL;
	}
	pol = rcu_dereference_protected(sk->sk_policy[1], 1);
	if (unlikely(pol != NULL)) {
		xfrm_policy_delete(pol, XFRM_POLICY_MAX+1);
		sk->sk_policy[1] = NULL;
	}
}