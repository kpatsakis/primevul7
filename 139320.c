static int unix_autobind(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct unix_sock *u = unix_sk(sk);
	static u32 ordernum = 1;
	struct unix_address *addr;
	int err;
	unsigned int retries = 0;

	err = mutex_lock_interruptible(&u->readlock);
	if (err)
		return err;

	err = 0;
	if (u->addr)
		goto out;

	err = -ENOMEM;
	addr = kzalloc(sizeof(*addr) + sizeof(short) + 16, GFP_KERNEL);
	if (!addr)
		goto out;

	addr->name->sun_family = AF_UNIX;
	atomic_set(&addr->refcnt, 1);

retry:
	addr->len = sprintf(addr->name->sun_path+1, "%05x", ordernum) + 1 + sizeof(short);
	addr->hash = unix_hash_fold(csum_partial(addr->name, addr->len, 0));

	spin_lock(&unix_table_lock);
	ordernum = (ordernum+1)&0xFFFFF;

	if (__unix_find_socket_byname(net, addr->name, addr->len, sock->type,
				      addr->hash)) {
		spin_unlock(&unix_table_lock);
		/*
		 * __unix_find_socket_byname() may take long time if many names
		 * are already in use.
		 */
		cond_resched();
		/* Give up if all names seems to be in use. */
		if (retries++ == 0xFFFFF) {
			err = -ENOSPC;
			kfree(addr);
			goto out;
		}
		goto retry;
	}
	addr->hash ^= sk->sk_type;

	__unix_remove_socket(sk);
	u->addr = addr;
	__unix_insert_socket(&unix_socket_table[addr->hash], sk);
	spin_unlock(&unix_table_lock);
	err = 0;

out:	mutex_unlock(&u->readlock);
	return err;
}