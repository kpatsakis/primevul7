static int packet_getname_spkt(struct socket *sock, struct sockaddr *uaddr,
			       int *uaddr_len, int peer)
{
	struct net_device *dev;
	struct sock *sk	= sock->sk;

	if (peer)
		return -EOPNOTSUPP;

	uaddr->sa_family = AF_PACKET;
	memset(uaddr->sa_data, 0, sizeof(uaddr->sa_data));
	rcu_read_lock();
	dev = dev_get_by_index_rcu(sock_net(sk), pkt_sk(sk)->ifindex);
	if (dev)
		strlcpy(uaddr->sa_data, dev->name, sizeof(uaddr->sa_data));
	rcu_read_unlock();
	*uaddr_len = sizeof(*uaddr);

	return 0;
}