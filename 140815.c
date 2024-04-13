static struct hlist_nulls_node *ct_get_next(struct seq_file *seq,
				      struct hlist_nulls_node *head)
{
	struct ct_iter_state *st = seq->private;

	head = rcu_dereference(hlist_nulls_next_rcu(head));
	while (is_a_nulls(head)) {
		if (likely(get_nulls_value(head) == st->bucket)) {
			if (++st->bucket >= st->htable_size)
				return NULL;
		}
		head = rcu_dereference(
			hlist_nulls_first_rcu(&st->hash[st->bucket]));
	}
	return head;
}