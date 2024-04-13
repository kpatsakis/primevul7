static inline struct sock *unix_find_socket_byname(struct net *net,
						   struct sockaddr_un *sunname,
						   int len, int type,
						   unsigned int hash)
{
	struct sock *s;

	spin_lock(&unix_table_lock);
	s = __unix_find_socket_byname(net, sunname, len, type, hash);
	if (s)
		sock_hold(s);
	spin_unlock(&unix_table_lock);
	return s;
}