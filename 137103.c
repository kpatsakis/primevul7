static void bpf_prog_ksym_node_add(struct bpf_prog_aux *aux)
{
	WARN_ON_ONCE(!list_empty(&aux->ksym_lnode));
	list_add_tail_rcu(&aux->ksym_lnode, &bpf_kallsyms);
	latch_tree_insert(&aux->ksym_tnode, &bpf_tree, &bpf_tree_ops);
}