static struct bpf_prog *bpf_prog_kallsyms_find(unsigned long addr)
{
	struct latch_tree_node *n;

	if (!bpf_jit_kallsyms_enabled())
		return NULL;

	n = latch_tree_find((void *)addr, &bpf_tree, &bpf_tree_ops);
	return n ?
	       container_of(n, struct bpf_prog_aux, ksym_tnode)->prog :
	       NULL;
}