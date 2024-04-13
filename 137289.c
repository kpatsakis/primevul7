static __always_inline bool bpf_tree_less(struct latch_tree_node *a,
					  struct latch_tree_node *b)
{
	return bpf_get_prog_addr_start(a) < bpf_get_prog_addr_start(b);
}