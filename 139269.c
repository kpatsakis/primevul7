static inline void unix_remove_socket(struct sock *sk)
{
	spin_lock(&unix_table_lock);
	__unix_remove_socket(sk);
	spin_unlock(&unix_table_lock);
}