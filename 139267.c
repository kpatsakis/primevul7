static int unix_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct unix_sock *u = unix_sk(sk);
	struct sockaddr_un *sunaddr = (struct sockaddr_un *)uaddr;
	char *sun_path = sunaddr->sun_path;
	int err, name_err;
	unsigned int hash;
	struct unix_address *addr;
	struct hlist_head *list;
	struct path path;
	struct dentry *dentry;

	err = -EINVAL;
	if (sunaddr->sun_family != AF_UNIX)
		goto out;

	if (addr_len == sizeof(short)) {
		err = unix_autobind(sock);
		goto out;
	}

	err = unix_mkname(sunaddr, addr_len, &hash);
	if (err < 0)
		goto out;
	addr_len = err;

	name_err = 0;
	dentry = NULL;
	if (sun_path[0]) {
		/* Get the parent directory, calculate the hash for last
		 * component.
		 */
		dentry = kern_path_create(AT_FDCWD, sun_path, &path, 0);

		if (IS_ERR(dentry)) {
			/* delay report until after 'already bound' check */
			name_err = PTR_ERR(dentry);
			dentry = NULL;
		}
	}

	err = mutex_lock_interruptible(&u->readlock);
	if (err)
		goto out_path;

	err = -EINVAL;
	if (u->addr)
		goto out_up;

	if (name_err) {
		err = name_err == -EEXIST ? -EADDRINUSE : name_err;
		goto out_up;
	}

	err = -ENOMEM;
	addr = kmalloc(sizeof(*addr)+addr_len, GFP_KERNEL);
	if (!addr)
		goto out_up;

	memcpy(addr->name, sunaddr, addr_len);
	addr->len = addr_len;
	addr->hash = hash ^ sk->sk_type;
	atomic_set(&addr->refcnt, 1);

	if (dentry) {
		struct path u_path;
		umode_t mode = S_IFSOCK |
		       (SOCK_INODE(sock)->i_mode & ~current_umask());
		err = unix_mknod(dentry, &path, mode, &u_path);
		if (err) {
			if (err == -EEXIST)
				err = -EADDRINUSE;
			unix_release_addr(addr);
			goto out_up;
		}
		addr->hash = UNIX_HASH_SIZE;
		hash = d_backing_inode(dentry)->i_ino & (UNIX_HASH_SIZE - 1);
		spin_lock(&unix_table_lock);
		u->path = u_path;
		list = &unix_socket_table[hash];
	} else {
		spin_lock(&unix_table_lock);
		err = -EADDRINUSE;
		if (__unix_find_socket_byname(net, sunaddr, addr_len,
					      sk->sk_type, hash)) {
			unix_release_addr(addr);
			goto out_unlock;
		}

		list = &unix_socket_table[addr->hash];
	}

	err = 0;
	__unix_remove_socket(sk);
	u->addr = addr;
	__unix_insert_socket(list, sk);

out_unlock:
	spin_unlock(&unix_table_lock);
out_up:
	mutex_unlock(&u->readlock);
out_path:
	if (dentry)
		done_path_create(&path, dentry);

out:
	return err;
}