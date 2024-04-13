static __always_inline int bpf_tree_comp(void *key, struct latch_tree_node *n)
{
	unsigned long val = (unsigned long)key;
	unsigned long symbol_start, symbol_end;
	const struct bpf_prog_aux *aux;

	aux = container_of(n, struct bpf_prog_aux, ksym_tnode);
	bpf_get_prog_addr_region(aux->prog, &symbol_start, &symbol_end);

	if (val < symbol_start)
		return -1;
	if (val >= symbol_end)
		return  1;

	return 0;
}