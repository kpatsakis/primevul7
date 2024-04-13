static void __unix_remove_socket(struct sock *sk)
{
	sk_del_node_init(sk);
}