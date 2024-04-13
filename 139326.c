static struct sock *unix_from_bucket(struct seq_file *seq, loff_t *pos)
{
	unsigned long offset = get_offset(*pos);
	unsigned long bucket = get_bucket(*pos);
	struct sock *sk;
	unsigned long count = 0;

	for (sk = sk_head(&unix_socket_table[bucket]); sk; sk = sk_next(sk)) {
		if (sock_net(sk) != seq_file_net(seq))
			continue;
		if (++count == offset)
			break;
	}

	return sk;
}