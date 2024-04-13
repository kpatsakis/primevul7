static void print_binder_ref_olocked(struct seq_file *m,
				     struct binder_ref *ref)
{
	binder_node_lock(ref->node);
	seq_printf(m, "  ref %d: desc %d %snode %d s %d w %d d %pK\n",
		   ref->data.debug_id, ref->data.desc,
		   ref->node->proc ? "" : "dead ",
		   ref->node->debug_id, ref->data.strong,
		   ref->data.weak, ref->death);
	binder_node_unlock(ref->node);
}