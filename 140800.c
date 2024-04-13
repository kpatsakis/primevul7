static struct hlist_nulls_node *ct_get_first(struct seq_file *seq)
{
	struct ct_iter_state *st = seq->private;
	struct hlist_nulls_node *n;

	for (st->bucket = 0;
	     st->bucket < st->htable_size;
	     st->bucket++) {
		n = rcu_dereference(
			hlist_nulls_first_rcu(&st->hash[st->bucket]));
		if (!is_a_nulls(n))
			return n;
	}
	return NULL;
}