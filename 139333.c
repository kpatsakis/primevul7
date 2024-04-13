static inline void unix_insert_socket(struct hlist_head *list, struct sock *sk)
{
	spin_lock(&unix_table_lock);
	__unix_insert_socket(list, sk);
	spin_unlock(&unix_table_lock);
}