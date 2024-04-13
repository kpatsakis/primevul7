static struct sock *esp6_find_tcp_sk(struct xfrm_state *x)
{
	struct xfrm_encap_tmpl *encap = x->encap;
	struct esp_tcp_sk *esk;
	__be16 sport, dport;
	struct sock *nsk;
	struct sock *sk;

	sk = rcu_dereference(x->encap_sk);
	if (sk && sk->sk_state == TCP_ESTABLISHED)
		return sk;

	spin_lock_bh(&x->lock);
	sport = encap->encap_sport;
	dport = encap->encap_dport;
	nsk = rcu_dereference_protected(x->encap_sk,
					lockdep_is_held(&x->lock));
	if (sk && sk == nsk) {
		esk = kmalloc(sizeof(*esk), GFP_ATOMIC);
		if (!esk) {
			spin_unlock_bh(&x->lock);
			return ERR_PTR(-ENOMEM);
		}
		RCU_INIT_POINTER(x->encap_sk, NULL);
		esk->sk = sk;
		call_rcu(&esk->rcu, esp_free_tcp_sk);
	}
	spin_unlock_bh(&x->lock);

	sk = __inet6_lookup_established(xs_net(x), &tcp_hashinfo, &x->id.daddr.in6,
					dport, &x->props.saddr.in6, ntohs(sport), 0, 0);
	if (!sk)
		return ERR_PTR(-ENOENT);

	if (!tcp_is_ulp_esp(sk)) {
		sock_put(sk);
		return ERR_PTR(-EINVAL);
	}

	spin_lock_bh(&x->lock);
	nsk = rcu_dereference_protected(x->encap_sk,
					lockdep_is_held(&x->lock));
	if (encap->encap_sport != sport ||
	    encap->encap_dport != dport) {
		sock_put(sk);
		sk = nsk ?: ERR_PTR(-EREMCHG);
	} else if (sk == nsk) {
		sock_put(sk);
	} else {
		rcu_assign_pointer(x->encap_sk, sk);
	}
	spin_unlock_bh(&x->lock);

	return sk;
}