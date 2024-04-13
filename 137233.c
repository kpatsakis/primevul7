bpf_get_prog_addr_start(struct latch_tree_node *n)
{
	unsigned long symbol_start, symbol_end;
	const struct bpf_prog_aux *aux;

	aux = container_of(n, struct bpf_prog_aux, ksym_tnode);
	bpf_get_prog_addr_region(aux->prog, &symbol_start, &symbol_end);

	return symbol_start;
}