static inline int xfrm_sk_clone_policy(struct sock *sk, const struct sock *osk)
{
	sk->sk_policy[0] = NULL;
	sk->sk_policy[1] = NULL;
	if (unlikely(osk->sk_policy[0] || osk->sk_policy[1]))
		return __xfrm_sk_clone_policy(sk, osk);
	return 0;
}