static void bpf_prog_ksym_node_del(struct bpf_prog_aux *aux)
{
	if (list_empty(&aux->ksym_lnode))
		return;

	latch_tree_erase(&aux->ksym_tnode, &bpf_tree, &bpf_tree_ops);
	list_del_rcu(&aux->ksym_lnode);
}