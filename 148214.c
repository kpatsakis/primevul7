static struct sock *sco_sock_alloc(struct net *net, struct socket *sock,
				   int proto, gfp_t prio, int kern)
{
	struct sock *sk;

	sk = sk_alloc(net, PF_BLUETOOTH, prio, &sco_proto, kern);
	if (!sk)
		return NULL;

	sock_init_data(sock, sk);
	INIT_LIST_HEAD(&bt_sk(sk)->accept_q);

	sk->sk_destruct = sco_sock_destruct;
	sk->sk_sndtimeo = SCO_CONN_TIMEOUT;

	sock_reset_flag(sk, SOCK_ZAPPED);

	sk->sk_protocol = proto;
	sk->sk_state    = BT_OPEN;

	sco_pi(sk)->setting = BT_VOICE_CVSD_16BIT;
	sco_pi(sk)->codec.id = BT_CODEC_CVSD;
	sco_pi(sk)->codec.cid = 0xffff;
	sco_pi(sk)->codec.vid = 0xffff;
	sco_pi(sk)->codec.data_path = 0x00;

	bt_sock_link(&sco_sk_list, sk);
	return sk;
}