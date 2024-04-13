	__acquires(RCU_BH)
{
	struct net *net = seq_file_net(seq);
	struct ipv6_route_iter *iter = seq->private;

	rcu_read_lock_bh();
	iter->tbl = ipv6_route_seq_next_table(NULL, net);
	iter->skip = *pos;

	if (iter->tbl) {
		ipv6_route_seq_setup_walk(iter, net);
		return ipv6_route_seq_next(seq, NULL, pos);
	} else {
		return NULL;
	}
}