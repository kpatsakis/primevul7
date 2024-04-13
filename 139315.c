static struct sock *unix_next_socket(struct seq_file *seq,
				     struct sock *sk,
				     loff_t *pos)
{
	unsigned long bucket;

	while (sk > (struct sock *)SEQ_START_TOKEN) {
		sk = sk_next(sk);
		if (!sk)
			goto next_bucket;
		if (sock_net(sk) == seq_file_net(seq))
			return sk;
	}

	do {
		sk = unix_from_bucket(seq, pos);
		if (sk)
			return sk;

next_bucket:
		bucket = get_bucket(*pos) + 1;
		*pos = set_bucket_offset(bucket, 1);
	} while (bucket < ARRAY_SIZE(unix_socket_table));

	return NULL;
}